# vi: set filetype=sh :: Vi/ViM
#-
# Copyright (c) 2012-2013 Parker Lee Ranney TTEE
# Copyright (c) 2017-2019 Devin Teske <dteske@FreeBSD.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
############################################################ IDENT(1)
#
# $Title: nsadmin functions for user/bind9 interaction $
# $Id: nsadmin-op.inc,v 1.3 2012/05/10 23:25:13 root Exp $
# $Copyright: 2017-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/nsadmin-op.inc 2019-10-05 00:51:50 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# nsadmin-op.inc -- Functions for use by nsadmin to operate the
# 	dns change mechanism and interact with the user.
#
############################################################ FUNCTIONS

# dnsreload
#
# Reload the master Bind named process.
#
dnsreload()
{
	local file
	local res
	local view

#?	clear # Testing -- Parker
	cat <<-EOF
	$sep
	# nsadmin: Local Bind Reload
	$sep

	EOF

	msg "Reloading Bind: $( date )"

	echo
	sudo service named restart
	sleep 1
	if ! pgrep named > /dev/null; then
		msg "ERROR: named is not running after reload: $( date )"

		msg "ERROR: attempting manual restart: $( date )"
		sudo service named start
		if pgrep named > /dev/null; then
			msg "Manual restart succeeded: $( date )"
			update=0
			return
		fi

		msg "ERROR: manual restart failed: $( date )"
		msg "ERROR: attempting to roll-back: $( date )"

		cd "$nameddir"
		for view in $views; do
			for file in $( find "$view" -name ".bak-*" ); do
				res=$( echo "$file" | sed -e "s/.bak-//" )
				sudo mv "$file" "$res"
			done
		done
		cd - > /dev/null

		sudo service named start
		if pgrep named > /dev/null; then
			msg "Roll-back succeeded: $( date )"
			update=0
			return
		fi

		echo "$USER: $( date )" |
			mail -s "nsadmin: roll-back failed" "$critical"
		fatal "roll-back failed ... giving up" # NOTREACHED
		update=2

	fi

	update=0 # successful update/reload
}

# fatal
#
# Generate a fatal error message and exit this program.
#
fatal()
{
	logger -p user.crit -t "FATAL ERROR" "$*"
	echo
	echo " Fatal Error: $*" | tee -a "$log"
	echo
	exit 1
}

# msg
#
# Provide a supplied message to the user and the log file.
#
msg()
{
	logger -p user.info -t Info "$*"
	cat <<-EOF | tee -a "$log"

	 $*

	EOF
}

# sigquit
#
# Process a set of commands when a signal is caught by trap.
#
sigquit()
{
	local excl
	local file
	local res
	local skip
	local subj

	[ "$ransig" ] && return

	stty echo

	LESSSECURE=

	cd "$nsadmindir"

	if [ ${update:=0} -eq 1 ]; then
		msg "ERROR: nsadmin killed after changes were" \
		    "made to master files: rolling back"
		for file in .bak-*; do
			[ -e "$file" ] || continue
			res=$( echo "$file" | sed -e s/.bak-// )
			cp "$file" "$res" > /dev/null 2>&1
		done
		checkin
		subj="nsadmin: ERROR: $( date ): $USER"
	elif [ ${update:=0} -eq 2 ]; then
		msg "ERROR: nsadmin killed after files were" \
		    "updated in the NAMEDB directory: rolling back"
		for file in .bak-*; do
			[ -e "$file" ] || continue
			res=$( echo "$file" | sed -e s/.bak-// )
			cp "$file" "$res"
		done
		checkin
		msg "ERROR":" rolling back NAMEDB zone files"
		subj="nsadmin: ERROR: $( date ): $USER"
	else
		checkin
		subj="nsadmin: $( date ): $USER"
	fi

	echo
	echo >> "$log"
	echo "$sep" >> "$log"
	echo " Finished: $( date )" | tee -a "$log"
	echo "$sep" >> "$log"
	echo >> "$log"
	cat "$log" | mail -s "$subj" "$admin"

	sudo rm -rf "$lock" "$log" "$tmp" .bak-* .revunq .updzones .view*

	# XXX Save the older version in case of emergency.
	#sudo find "$nameddir" -name ".bak-*" -exec rm -f {} \;

	#
	# Make sure files are not left in a write state as it will
	# generate errors.
	#
	for file in *; do
		[ -e "$file" ] || continue
		skip=
		for excl in $exclude; do
			[ "$file" = "$excl" ] || continue
			skip=1
			break
		done
		[ "$skip" ] || chmod a-w "$file" > /dev/null 2>&1
	done

	cd - > /dev/null

	ransig=1
}

# zonemenu
#
# Print the list of zones and prompt for user selection.
#
zonemenu()
{
	local zone

	while :; do
		zonelist

		printf "\n Please choose a zone to"
		if [ "$editmode" ]; then
			printf " edit "
		elif [ "$sync" ]; then
			printf " sync "
		else
			printf " view "
		fi
		printf "or 'q' to quit: "
		read zone

		if [ "$zone" = Q ] || [ "$zone" = q ]; then
			break
		fi

		echo
		if [ ! "$zone" ] || [ ! -f "$zone" ]; then
			msg "Zone file not found: '$zone'"
			continue
		fi

		if [ "$editmode" ]; then
			cp "$zone" ".bak-$zone" > /dev/null 2>&1
			editmaster "$zone"
		elif [ "$sync" ]; then
			if [ "$updzones" ]; then
				if ( ! echo "$updzones" | egrep \
		    		    "(^| |	)$zone(	| |$)" \
		    		    > /dev/null 2>&1 ); then
					updzones="$updzones $zone"
				fi
			else
				updzones="$zone"
			fi
		else
			${PAGER:-less} "$zone"
		fi
	done
}

################################################################################
# END
################################################################################
