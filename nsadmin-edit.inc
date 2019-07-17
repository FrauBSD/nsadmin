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
# $Title: nsadmin functions for managing edits on zone files $
# $Id: nsadmin-edit.inc,v 1.1 2012/05/09 21:39:36 root Exp $
# $Copyright: 2017-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/nsadmin-edit.inc 2019-07-16 18:44:24 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# nsadmin-edit.inc -- functions for use by nsadmin to edit and change
# 	the master files used to generate zone files and rev maps.
#
############################################################ FUNCTIONS

# editmaster zone
#
# Edit the master files.
#
editmaster()
{
	local file="$1"

	cwd="$PWD"

	cat >> "$log" <<-EOF
	$sepdbl
	Editing $file
	$sepdbl

	EOF

	cd "$nsadmindir"
	checkout "$file"

	${EDITOR:-vi} "$file"
	verify "$file"

	cd "$cwd"
}

# getlock
#
# Establish an exclusive lock for master file edits.
#
getlock()
{
	local clobber=
	local res

	if [ -f "$lock" ]; then
		res=$( ls -l "$lock" | awk '{print $3}' )
		fatal "lock file exists, owner is $res" # NOTREACHED
	fi

	[ "$-" = "${-#*C}" ] && clobber=1
	set -C
	echo $$ > "$lock" || fatal "could not create '$lock'" # NOTREACHED
	[ "$clobber" ] && set +C
}

# predit
#
# Prepare the editing of the master files.
#
predit()
{
	trap "sigquit; exit" EXIT SIGHUP SIGINT SIGQUIT SIGTERM

	msg "$USER starting edits: $( date )"

	sudo rm -rf "$lock" "$log" "$tmp" .bak-* .revunq .updzones .view*

	cat > "$log" <<-EOF

	$sep
	 Starting: $( date )
	$sep

	EOF

	cd "$nsadmindir"
	mkdir -p "$tmp"
}

################################################################################
# END
################################################################################
