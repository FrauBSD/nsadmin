#!/usr/bin/awk -f
#-
# Copyright (c) 2018-2019 Devin Teske <dteske@FreeBSD.org>
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
# $Title: Generate PTR records from nsadmin zone file $
# $Copyright: 2018-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/zone2rev.awk 2019-07-16 18:44:24 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# Usage:
# 	-v file=path		nsadmin zone file to read
# Options:
# 	-v console=1		Enable ANSI color. Implies debug=1
# 	-v debug=1		Enable debug messages printed to stderr
# 	-v initialize=1		Initialize files to zero length
# 	-v list_subnets=1	List subnets on stdout and exit
# 	-v verify=1		Verify contents and exit. Implies debug=1
#
############################################################ FUNCTIONS

function err(str)
{
	if (verify) vstatus = 1
	if (!debug) return
	if (console)
		printf "\033[35m%s\033[36m:\033[32m%d\033[36m:\033[m %s\n",
			file, NR, str > "/dev/stderr"
	else
		printf "%s:%d: %s\n", file, NR, str > "/dev/stderr"
	fflush()
}

# _asorti(src, dest)
#
# Like GNU awk's asorti() but works with any awk(1)
# NB: Named _asorti() to prevent conflict with GNU awk
#
function _asorti(src, dest,        k, nitems, i, idx)
{
	k = nitems = 0
	for (i in src) dest[++nitems] = i
	for (i = 1; i <= nitems; k = i++) {
		idx = dest[i]
		while ((k > 0) && (dest[k] > idx)) {
			dest[k+1] = dest[k]; k--
		}
		dest[k+1] = idx
	}
	return nitems
}

# validate_ipaddr4(ip)
#
# Returns zero if the given argument (an IP address) is of the proper format.
#
# The return value for invalid IP address is one of:
# 	1	One or more individual octets within the IP address (separated
# 		by dots) contains one or more invalid characters.
# 	2	One or more individual octets within the IP address are null
# 		and/or missing.
# 	3	One or more individual octets within the IP address exceeds the
# 		maximum of 255 (or 2^8-1, being an octet comprised of 8 bits).
# 	4	The IP address has either too few or too many octets.
#
function validate_ipaddr4(ip,        octets, noctets, n, octet)
{
	# Split on `dot'
	noctets = split(ip, octets, /\./)
	if (noctets != 4) return 4

	for (n = 1; n <= noctets; n++) {
		octet = octets[n]

		# Return error if the octet is null
		if (octet == "") return 2

		# Return error if not a whole/positive integer
		if (octet ~ /[^0-9]/) return 1

		# Return error if the octet exceeds 255
		if (octet > 255) return 3
	}

	return 0
}

# validate_ipaddr6(ip)
#
# Returns zero if the given argument (an IPv6 address) is of the proper format.
#
# The return value for invalid IP address is one of:
# 	1	One or more individual segments with the IP address
# 		(separated by colons) contains one or more invalid characters.
# 		Segments must contain only combinations of the characters 0-9,
# 		A-F, or a-f.
# 	2	Too many/incorrent null segments. A single null segment is
# 		allowed within the IP address (separated by colons) but not
# 		allowed at the beginning or end (unless a double-null segment;
# 		i.e., "::*" or "*::").
# 	3	One or more individual segments within the IP address
# 		(separated by colons) exceeds the length of 4 hex-digits.
# 	4	The IP address entered has either too few (less than 3), too
# 		many (more than 8), or not enough segments, separated by
# 		colons.
# 	5	The IPv4 address at the end of the IPv6 address is invalid.
#
function validate_ipaddr6(ip,
	segments, nsegments, n, segment, h, short, nulls,
	contains_ipv4_segment, maxsegments)
{
	sub(/%.*$/, "", ip) # remove interface spec if-present

	# Split on `colon'
	nsegments = split(ip, segments, /:/)

	# Return error if too many or too few segments
	# Using 9 as max in case of leading or trailing null spanner
	if (nsegments > 9 || nsegments < 3) return 4

	h = "[0-9A-Fa-f]"
	short = sprintf("^(%s|%s|%s|%s)$", h, h h, h h h, h h h h)

	nulls = contains_ipv4_segment = 0

	for (n = 1; n <= nsegments; n++) {
		segment = segments[n]

		#
		# Return error if this segment makes one null too-many. A
		# single null segment is allowed anywhere in the middle as well
		# as double null segments are allowed at the beginning or end
		# (but not both).
		#
		if (segment == "") {
			nulls++
			if (nulls == 3) {
				# Only valid syntax for 3 nulls is `::'
				if (ip != "::") return 2
			} else if (nulls == 2) {
				# Only valid if begins/ends with `::'
				if (ip !~ /(^::|::$)/) return 2
			}
			continue
		}

		#
		# Return error if not a valid hexadecimal short
		#
		if (segment ~ short) continue # Valid segment of 1-4 hex digits
		if (segment ~ /[^0-9A-Fa-f]/) {
			# Segment contains at least one invalid char

			# Return error immediately if not last segment
			if (n < nsegments) return 1

			# Otherwise, check for legacy IPv4 notation
			if (segment ~ /[^0-9.]/) {
				# Segment contains at least one invalid
				# character even for an IPv4 address
				return 1
			}

			# Return error if not enough segments
			if (nulls == 0) {
				if (nsegments != 7) return 4
			}

			contains_ipv4_segment=1

			# Validate ipv4_segment
			if (validate_ipaddr4(segment)) return 5
		} else {
			# Segment characters are all valid but too many
			return 3
		}
	}

	if (nulls == 1) {
		# Single null segment cannot be at beginning/end
		if (ip ~ /(^:|:$)/) return 2
	}

	#
	# A legacy IPv4 address can span the last two 16-bit segments,
	# reducing the amount of maximum allowable segments by-one.
	#
	maxsegments = contains_ipv4_segment ? 7 : 8

	if (nulls == 0) {
		# Return error if missing segments with no null spanner
		if (nsegments != maxsegments) return 4
	} else if (nulls == 1) {
		# Return error if null spanner with too many segments
		if (nsegments > maxsegments) return 4
	} else if (nulls == 2) {
		# Return error if leading/trailing `::' with too many segments
		if (nsegments > (maxsegments + 1)) return 4
	}

	return 0
}

# split6(ip, array)
#
# Split the elements of IPv6 ip into 32 hex-nibbles stored in array.
#
function split6(ip, nibbles,        n, ip4, octs, s, i, nibs, nib, k)
{
	for (n = 1; n <= 32; n++) nibbles[n] = 0
	if (match(ip, /:[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$/)) {
		ip4 = substr(ip, RSTART + 1)
		ip = substr(ip, 1, RSTART - 1)
		split(ip4, octs, /\./)
		ip = sprintf("%s:%02x%02x:%02x%02x",
			ip, octs[1], octs[2], octs[3], octs[4])
	}
	if (sub(/^::/, "", ip)) {
		n = 32
		ip = sprintf("%04s", ip)
		for (k = 4; k > 0; k--) nibbles[n--] = substr(ip, k, 1)
	} else if (sub(/::$/, "", ip)) {
		n = 1
		ip = sprintf("%04s", ip)
		for (k = 1; k <= 4; k++) nibbles[n++] = substr(ip, k, 1)
	} else if (ip ~ /::/) {
		left = right = ip
		sub(/::.*$/, "", left)
		sub(/^.*::/, "", right)
		s = split(left, nibs, /:/)
		for (i = 1; i <= s; i++) {
			n = (i - 1) * 4 + 1
			nib = sprintf("%04s", nibs[i])
			for (k = 1; k <= 4; k++)
				nibbles[n++] = substr(nib, k, 1)
		}
		s = split(right, nibs, /:/)
		n = 32
		for (i = s; i > 0; i--) {
			nib = sprintf("%04s", nibs[i])
			for (k = 4; k > 0; k--)
				nibbles[n--] = substr(nib, k, 1)
		}
	} else {
		s = split(ip, nibs, /:/)
		for (i = 1; i <= s; i++) {
			n = (i - 1) * 4 + 1
			nib = sprintf("%04s", nibs[i])
			for (k = 1; k <= 4; k++)
				nibbles[n++] = substr(nib, k, 1)
		}
	}
}

############################################################ MAIN

BEGIN {
	if (console || verify) debug = 1
	vstatus = 0

	subnet = tolower(subnet)
	delete initialized
	delete files
	NR = 0
	while (getline < file > 0) {
		NR++
		if (/^[[:space:]]*;/) continue
		if ($1 ~ "^" file) continue
		if (/;*VIEW:/ && $0 !~ view) continue
		if (/\*/) continue
		if ($2 == "A") {
			if ((errno = validate_ipaddr4(ip = $3)) != 0) {
				err(sprintf("bad A record `%s' for `%s' " \
					"(ERR#%u)", ip, $1, errno))
				continue
			}
			split(ip, oct, /\./)
			net = sprintf("%u.%u.%u", oct[1], oct[2], oct[3])
			if (!list_subnets && subnet && net != subnet) continue
			rev = sprintf(".new-%u.%u.%u", oct[3], oct[2], oct[1])
			ptr = sprintf("%u\t\t\t\t\tPTR\t%s.%s.",
				oct[4], $1, file)
		} else if ($2 == "AAAA") {
			if ((errno = validate_ipaddr6(ip = $3)) != 0) {
				err(sprintf("bad AAAA record `%s' for `%s' " \
					"(ERR#%u)", ip, $1, errno))
				continue
			}
			delete nib
			split6(ip, nib)
			net = sprintf("%s%s%s%s:%s%s%s%s:%s%s%s%s:%s%s%s%s",
				nib[1], nib[2], nib[3], nib[4],
				nib[5], nib[6], nib[7], nib[8],
				nib[9], nib[10], nib[11], nib[12],
				nib[13], nib[14], nib[15], nib[16])
			if (!list_subnets && subnet && tolower(net) != subnet)
				continue
			rev = tolower(sprintf(".new-" \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s",
				nib[16], nib[15], nib[14], nib[13],
				nib[12], nib[11], nib[10], nib[9],
				nib[8], nib[7], nib[6], nib[5],
				nib[4], nib[3], nib[2], nib[1]))
			ptr = sprintf( \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s." \
				"%s.%s.%s.%s" \
				"\t\tPTR\t%s.%s.",
				nib[32], nib[31], nib[30], nib[29],
				nib[28], nib[27], nib[26], nib[25],
				nib[24], nib[23], nib[22], nib[21],
				nib[20], nib[19], nib[18], nib[17],
				$1, file)
		} else {
			continue
		}
		if (list_subnets) {
			subnets[net] = 1
			continue
		}
		if (initialize && !(rev in initialized)) {
			printf "" > rev
			initialized[rev] = 1
		}
		files[rev] = 1
		print ptr >> rev
	}
	exit
}

END {
	if (verify) exit vstatus
	if (list_subnets) {
		n = _asorti(subnets, subnets_sorted)
		for (i = 1; i <= n; i++) print subnets_sorted[i]
		exit
	}

	if (!debug) exit
	fmt = console ? "\033[32m%5d\033[36m:\033[m %s\n" : "%5d: %s\n"
	n = _asorti(files, files_sorted)
	for (i = 1; i <= n; i++) {
		rev = files_sorted[i]
		if (console)
			printf "\033[32m>\033[m %s\n", rev > "/dev/stderr"
		else
			printf "> %s\n", rev > "/dev/stderr"
		while (getline < rev > 0)
			printf fmt, ++NRr[rev], $0 > "/dev/stderr"
	}
}

################################################################################
# END
################################################################################
