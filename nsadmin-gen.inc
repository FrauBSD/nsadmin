# vi: set filetype=sh :: Vi/ViM
#-
# Copyright (c) 2012 Parker Lee Ranney TTEE
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
# $Title: nsadmin functions for generating bind9 master/rev zone files $
# $Id: nsadmin-gen.inc,v 1.7 2012/05/11 03:20:58 root Exp $
# $Copyright: 2017-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/nsadmin-gen.inc 2019-10-05 00:51:50 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# nsadmin-gen.inc -- Functions for use by nsadmin to generate zone
# 	files and reverse maps.
#
############################################################ FUNCTIONS

# genmaster
#
# Generate the zone files and rev maps.
#
genmaster()
{
	local ans
	local file
	local subnet

	#
	# Save the master files that have changes to make sure they
	# are not lost in the loop'ing below.
	#
	echo "$updzones" > "$nsadmindir/.updzones"

	#
	# Generate a list of all subnets in the master file
	#
	if [ "$sync" ]; then
		for subnet in $(
			for file in $updzones; do
				zone2rev.awk -v file="$file" -v list_subnets=1
			done | sort -u
		); do
			# Only insert subnet if not existing in $subnets
			echo "$subnets" | grep -q "\\<$(
				echo "$subnet" | sed -e 's/\./\\&/g'
			)$subnet\\>" || subnets="$subnets $subnet"
		done
		subnets="${subnets# }"
	fi

	#
	# Loop through the zone file generation until each error is
	# fixed. Slave systems will not sync bad zone files nor will
	# the master load them.
	#
	while :; do
		echo
		msg "Building zone files and revmaps: $( date )"
		genzone $( cat "$nsadmindir/.updzones" )
		genrev

		zonetest
		[ "$failfile" ] || break

		printf "\nPress return when ready to re-edit."
		stty -echo
		read ans # ignored
		stty echo

		# Clean up the previous attempt
		find "$tmp" -type f -exec rm -f {} \;

		#
		# XXX Use the rcsdiff output to find the correct
		# XXX zone to edit instead of relying on the user to
		# XXX remember which file had errors.
		#
		if [ "$failtype" = rev ]; then
			zonemenu
		else
			editmaster "$failfile"
		fi
	done
}

# genrev
#
# Generate the reverse maps.
#
genrev()
{
	local excl
	local file
	local list
	local origin
	local rev
	local revfile
	local subnet
	local view

	cd "$nsadmindir"
	for view in $views; do

		list=$( ls -d [a-z0-9]* )
		for excl in $exclude; do
			list=$( echo "$list" | sed -e "s/$excl//" )
		done

		for file in $list; do
			for subnet in $subnets; do
				sudo zone2rev.awk -v view="$view" \
					-v file="$file" -v subnet="$subnet" \
					> /dev/null 2>&1
			done
		done

		for file in .new-*; do
			[ -e "$file" ] || continue

			revfile=$( echo "$file" | sed -e "s/^.new-//" )
			case "$revfile" in
			*.*.*.*.*) origin="$revfile.ip6.arpa." ;;
			*) origin="$revfile.in-addr.arpa." ;;
			esac
			rev="$tmp/$view/rev/$revfile"

			revunq "$file"

			awk 'sub(/^\\t/,"\t")||1' > "$rev" <<-EOF
			\$TTL $ttl
			@ IN SOA $primary. $contact. (
			\t		$serial	; serial
			\t		$refresh		; refresh
			\t		$retry		; retry
			\t		$expire		; expire
			\t		$minimum )		; minimum
			\t		IN NS $primary.
			\t		IN NS $secondary.
			\$ORIGIN $origin
			EOF
			grep -v ";" "$file" >> "$rev"

			rm -f "$file"
		done
	done
	cd - > /dev/null
}

# genzone zone ...
#
# Generate the master zone files.
#
genzone()
{
	local file
	local list="$*" # edited zones
	local origin
	local view

	for view in $views; do
		mkdir -p "$tmp/$view/master" "$tmp/$view/rev"
	done

	for view in $views; do
		cd "$tmp/$view/master"

		for file in $list; do

			origin="$file."

			awk 'sub(/^\\t/,"\t")||1' > "$file" <<-EOF
			\$TTL $ttl
			@ IN SOA $primary. $contact. (
			\t		$serial	; serial
			\t		$refresh		; refresh
			\t		$retry		; retry
			\t		$expire		; expire
			\t		$minimum )		; minimum
			\t		IN NS $primary.
			\t		IN NS $secondary.
			\$ORIGIN $origin
			EOF

			#
			# Keep any semicolons within a quoted TXT
			# record. Clear away all others.
			#
			awk -v view="$view" '
			!/^[[:space:]]*(;|$)/ {
				if (/;*VIEW:/ && $0 !~ view) next
				if (/".*;.*"/)
					sub(/; VIEW:.*/, "")
				else
					sub(/;.*/, "")
				print
			}' "$nsadmindir/$file" >> "$file"
		done

		cd - > /dev/null
	done
}

# revunq file
#
# Generate a rev map file with unique PTR records based on resulting hostname
# pointers.
#
revunq()
{
	local file="$1"

	awk -v file="$file" '
	function posnum(str)
	{
		if (str ~ /ns[0-9]*\.example\.com/) return 6
		if (str ~ /mx[0-9]*\.googlemail\.com/) return 6
		if (str ~ /\..*\.example\.com/) return 5
		if (str ~ /\.example\.com/) return 4
		if (str ~ /\.example\./) return 3
		return 1
	}
	BEGIN {
		delete host
		while (getline < file > 0) {
			if (!($1 in host)) host[$1] = $3
			else if (posnum(host[$1]) < posnum($3)) host[$1] = $3
		}
		for (unit in host) printf("%s%s\t\tPTR\t%s\n",
			unit, length(unit) <= 3 ? "\t\t\t" : "", host[unit])
	}
	' > .revunq

	sudo mv .revunq "$file"
}

################################################################################
# END
################################################################################
