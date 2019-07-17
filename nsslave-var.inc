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
# $Title: nsslave configuration file $
# $Id: nsslave-var.inc,v 1.1 2012/05/09 21:39:42 root Exp root $
# $Copyright: 2017-2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/nsslave-var.inc 2019-07-16 18:44:24 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# nsslave-var.inc -- Variables for use by nsslave.
#
############################################################ GLOBALS

PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin

#
# Global exit status
#
SUCCESS=0
FAILURE=1

############################################################ CONFIGURATION

admin=ops@example.com			# Admin email address
critical=page-ops@example.com		# Critical failure email address
group=cm				# Headless user group
user=cm					# Headless user for execution
views="ext int"				# Views to create
cwd="$PWD"				# Current working directory
home="/home/$user"			# Home directory
tmp="$home/tmp"				# Temporary directory

############################################################ MAIN

#
# Bind's namedb directory
#
case "$( uname )" in
OpenBSD)
	nameddir=/var/named/etc
	owner=named
	;;
Linux)
	nameddir=/var/named/chroot/var/named
	owner=named
	;;
FreeBSD)
	group=wheel
	user=root
	home=/root
	tmp="$home/tmp"
	nameddir=/var/named
	owner=root
	;;
*)
	nameddir=/var/named/etc/namedb
	owner=named
esac

################################################################################
# END
################################################################################
