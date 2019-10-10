#!/bin/sh
############################################################ IDENT(1)
#
# $Title: Script to clean nsadmin/nsslave $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/clean_fraubsd.sh 2019-10-10 12:48:56 -0700 freebsdfrau $
#
############################################################ GLOBALS

#
# ANSI
#
ESC=$( :| awk 'BEGIN { printf "%c", 27 }' )
ANSI_BLD_ON="$ESC[1m"
ANSI_BLD_OFF="$ESC[22m"
ANSI_GRN_ON="$ESC[32m"
ANSI_FGC_OFF="$ESC[39m"

############################################################ FUNCTIONS

eval2()
{
        echo "$ANSI_BLD_ON$ANSI_GRN_ON==>$ANSI_FGC_OFF $*$ANSI_BLD_OFF"
        eval "$@"
}

############################################################ MAIN

set -e
for item in \
	admin-install \
	slave-install \
;do
        eval2 rm -Rf "$item"
done

################################################################################
# END
################################################################################
