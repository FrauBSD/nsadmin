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
# $Title: nsadmin functions for vcs/zone interaction $
# $Id: nsadmin-zone.inc,v 1.4 2012/05/11 03:18:25 root Exp $
# $Copyright: 2017-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/nsadmin-zone.inc 2019-07-16 18:44:24 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# nsadmin-zone.inc -- Functions for use by nsadmin to interact with
# 	the master files and generated zones.
#
############################################################ FUNCTIONS

# checkin
#
# Using RCS, check in the edited and verified master files.
#
checkin()
{
	local cnt=1

	while [ "${colist[$cnt]}" ]; do
		echo "$USER: $( date )" |
			ci -u "${colist[$cnt]}" #? > /dev/null 2>&1
		cnt=$(( $cnt + 1 ))
	done
}

# checkout file
#
# Using RCS, check out the master file for editing.
#
checkout()
{
	local cnt=1
	local file="$1"

	while [ "${colist[$cnt]}" ]; do
		[ "$file" = "${colist[$cnt]}" ] && return
		cnt=$(( $cnt + 1 ))
	done

	colist[$cnt]="$file"
	co -l "$file" > /dev/null 2>&1
}

# getsubnets file
#
# Get only the subnets that changed in the master file edit.
#
getsubnets()
{
	local file="$1"
	local subnet

	for subnet in $( rcsdiff -uq "$file" | awk 'NR>2&&sub(/^[+-]/,"")' |
		zone2rev.awk -v file=- -v list_subnets=1 )
	do
		# Only insert subnet if not existing in $subnets
		echo "$subnets" | grep -q "\\<$(
			echo "$subnet" | sed -e 's/\./\\&/g'
		)$subnet\\>" || subnets="$subnets $subnet"
	done
	subnets="${subnets# }"
}

# pushzones
#
# Update the Bind zone files with the finished master files.
#
pushzones()
{
	local cmd
	local file
	local res
	local slave
	local view

	cwd="$PWD"

#?	clear # Testing -- Parker
	cat <<-EOF
	$sep
	# nsadmin: Zone Push
	$sep

	EOF
	msg " Executing local Push:"

	msg "	Backing up previous versions: $( date )"
	cd "$nameddir"
	for view in $views; do
		for file in $( find "$view" -type f ! -name ".bak-*" ); do
			res=$( echo "$file" |
			    sed -e 's/\(.*\/\)\(.*\)/\1.bak-\2/' )
			sudo cp "$file" "$res"
		done
	done

	msg "	Pushing local zones: $( date )"
	cd "$tmp"
	sudo chown -R "$binduser:$bindgroup" *
	sudo chmod -R g-w *
	find . -type f -exec sudo cp {} "$nameddir/{}" \;

	msg "	Building zone tarball: $( date )"
	cd "$tmp"
	tar czf zone.tgz *
#?	sleep 2 # Testing -- Parker

	for slave in $slaves; do
#?		clear # Testing -- Parker
		cat <<-EOF
		$sep
		# nsadmin: Zone Push
		$sep

		EOF
		msg " Executing remove push to $slave: $( date )"

		msg "	Copying zone tarball to $slave: $( date )"
		cmd="scp $scpopts $tmp/zone.tgz $user@$slave:tmp"
		sudo su - $user -c "$cmd"

		msg "	Executing push on $slave: $( date )"
		cmd="ssh $sshopts $slave"
		cmd="$cmd 'sudo /usr/local/bin/nsslave'"
		sudo su - $user -c "$cmd"

#?		sleep 2 # Testing -- Parker
	done

	cd "$cwd"
}

# verify file
#
# Verify changes in the zone files.
#
verify()
{
	local ans
	local file="$1"
	local res

	if rcsdiff -q "$file"; then
		ci -u "$file" > /dev/null 2>&1
		msg "No changes made to $file."
		stty -echo
		echo " Press return to continue."
		read ans # Ignored
		stty echo
		return
	fi

	if res=$( zone2rev.awk -v file="$file" -v verify=1 ); then
		vprompt "$file"
		return
	fi

#?	clear # Testing -- Parker
	cat <<-EOF
	$sep
	# nsadmin: Verify Failure
	$sep

	EOF

	msg "ERROR: incorrect A/AAAA record format"
	echo

	echo "$res" | tee -a "$log"
	rcsdiff -u "$file" | tee -a "$log"

	echo
	echo
	stty -echo
	echo " Press return to continue."
	read ans # Ignored
	stty echo

	editmaster "$file"
}

# viewtest zone
#
# Test the view syntax in the master files.
#
viewtest()
{
	local file="$nsadmindir/$1"
	local view

	cat <<-EOF

	 Testing View Syntax:

	EOF

	rm -f .view*

	grep --line-number VIEW "$file" > .view
	grep -v "; VIEW: " .view >> .view-out
	grep , .view | grep -v ", " >> .view-out

	grep "; VIEW: " .view |
		sed -e "s/.*VIEW: //;s/,//g" > .view-list
	for view in $views; do
		sed -e "s/$view//g" .view-list > .n
		mv .n .view-list
	done
	egrep -v '^( $|$)' .view-list > .n
	mv .n .view-list

	# XXX YUK
	if [ -s .view-out ] || [ -s .view-list ]; then
		if [ -s .view-out ]; then
			sed -e 's/^\([0-9]*\):/line \1: /;s/^/	/' .view-out
			echo
			msg "ERROR: invalid view syntax, must use" \
			    "'; VIEW: <view>[, <view> ...]'"
		fi

		if [ -s .view-list ]; then
			sed -e "s/^/	unknown view: /" .view-list
			echo
			msg "ERROR: unknown views, valid views are" "'$views'"
		fi

		failtype=master
		failfile="$1"

		return
	fi

	echo "	OK"
	echo
}

# vprompt file
#
# User interaction within the verify() function.
#
vprompt()
{
	local ans
	local file="$1"

	clear
	cat <<-EOF
	$sep
	# nsadmin: $file Changes
	$sep

	EOF

	rcsdiff -u "$file" | tee -a "$log"

	echo
	printf " Accept these changes [(y)es | (n)o | (E)dit]: "
	read ans
	echo

	case "$ans" in
	[nN])	msg "$USER canceled changes: $( date )"
		cp ".bak-$file" "$file" > /dev/null 2>&1
		;;
	[yY])	echo
		msg "$USER accepted changes: $( date )"
		getsubnets "$file"
		update=1 # First stage of dns updates
		if [ "$updzones" ] && ! echo "$updzones" |
		    egrep "(^| |	)$file(	| |$)" > /dev/null 2>&1
		then
			updzones="$updzones $file"
		else
			updzones="$file"
		fi
		echo
		;;
	*)	msg "$USER re-editing $file; $( date )"
		editmaster "$file"
	esac
}

# zonelist
#
# List the currently active zones.
#
zonelist()
{
	local cnt
	local col=0
	local tab=
	local zone

	clear

	cat <<-EOF
	$sep
	# nsadmin: Zone List
	$sep

	EOF

	for zone in [a-z0-9]*; do
		[ -e "$zone" ] || continue
		if echo "$exclude" |
		    egrep "(^| |	)$zone(	| |$)" \
		    > /dev/null 2>&1
		then
			continue
		fi

		cnt=$(( ${#zone} + 1 ))
		if [ $cnt -lt 8 ]; then
			tab='\t\t\t'
		elif [ $cnt -lt 16 ]; then
			tab='\t\t'
		else
			tab='\t'
		fi
		if [ $col -ge 3 ]; then
			echo
			col=0
		fi
		printf " %s$tab" "$zone"
		col=$(( $col + 1 ))
	done
	echo
}

# zonetest
#
# Test the generated zones and rev maps.
#
zonetest()
{
	local file
	local _IFS="$IFS" IFS
	local res

	failfile=
	failtype=

	for file in $( find "$tmp" -type f ); do
		IFS=/
		set -- ${file#"$tmp/"}
		IFS="$_IFS"

		clear
		cat <<-EOF
		$sep
		# nsadmin: Zone Tests
		$sep

		 View:	$1
		 Type:	$2
		 Zone:	$3

		EOF

		if [ "$2" = master ]; then
			viewtest "$3"
			[ "$failtype" ] && return
		fi

		echo
		echo " Testing Bind Syntax:"

		if ! res=$( exec 2>&1
			name="$3"
			if [ "$2" = rev ]; then
				if [ "$3" != "${3#*.*.*.*.*}" ]; then
					name="$name.ip6.arpa"
				else
					name="$name.in-addr.arpa"
				fi
			fi
			named-checkzone "$name" "$file"
		) && [ $( echo "$res" | wc -l ) -gt 2 ]; then
			failtype="$2"
			failfile="$3"

			echo "$res" | sed -e 's/.*\///;s/^/	/'
			echo

			echo
			echo " Master File Diff:"
			rcsdiff -u "$nsadmindir/$3" 2>&1 |
				sed -e "s/^/	/"
			echo

			msg "ERROR: zone test failed on $failfile"

			return
		fi

		echo "$res" | sed -e "s/^/	/"
		echo

#?		sleep 2 # Testing -- Parker
	done
}

################################################################################
# END
################################################################################
