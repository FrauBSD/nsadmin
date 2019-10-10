#!/bin/sh
#-
############################################################ IDENT(1)
#
# $Title: Script to install nsadmin/nsslave into sandboxen for packaging $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/build_fraubsd.sh 2019-10-10 12:48:56 -0700 freebsdfrau $
#
############################################################ GLOBALS

NSADMIN_DESTDIR=admin-install
NSSLAVE_DESTDIR=slave-install

#
# ANSI
#
ESC=$( :| awk 'BEGIN { printf "%c", 27 }' )
ANSI_BLD_ON="$ESC[1m"
ANSI_BLD_OFF="$ESC[22m"
ANSI_GRN_ON="$ESC[32m"
ANSI_FGC_OFF="$ESC[39m"

############################################################ FUNCTIONS

exec 3>&1
eval2()
{
	echo "$ANSI_BLD_ON$ANSI_GRN_ON==>$ANSI_FGC_OFF $*$ANSI_BLD_OFF" >&3
	eval "$@"
}

############################################################ MAIN

set -e

#
# Install dependencies
#
items_needed=
#	bin=someprog:pkg=somepkg \
#	file=/path/to/some_file:pkg=somepkg \
#	lib=somelib.so:pkg=somepkg \
for entry in \
	bin=make:pkg=make \
; do
	check="${entry%%:*}"
	item="${check#*=}"
	case "$check" in
	 bin=*) type "$item" > /dev/null 2>&1 && continue ;;
	file=*) [ -e "$item" ] && continue ;;
	 lib=*) ldconfig -p | awk -v lib="$item" \
		'$1==lib{exit f++}END{exit !f}' && continue ;;
	     *) continue
	esac
	pkg="${entry#*:}"
	pkgname="${pkg#*=}"
	items_needed="$items_needed $pkgname"
done
[ "$items_needed" ] && eval2 sudo yum install $items_needed

#
# Prepare packages
#
eval2 mkdir -pv $NSADMIN_DESTDIR
eval2 mkdir -pv $NSSLAVE_DESTDIR

#
# Build package
#
eval2 make DESTDIR="$NSADMIN_DESTDIR" install-admin
eval2 make DESTDIR="$NSSLAVE_DESTDIR" install-slave

eval2 : SUCCESS

################################################################################
# END
################################################################################
