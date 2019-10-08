############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-08 13:43:47 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=	/usr/local/bin
CONFDIR=	/etc
NSADMINDIR=	$(CONFDIR)/nsadmin

############################################################ PATHS

CAT=		cat
CP_F=		cp -f
CP_N=		cp -n
MKDIR_P=	mkdir -p

############################################################ OBJECTS

NSADMIN=	nsadmin
NSADMIN_CONF=	nsadmin.conf

NSSLAVE=	nsslave
NSSLAVE_CONF=	nsslave.conf

############################################################ TARGETS

all install:
	@printf "Options:\n"
	@printf "\tmake install-admin\tInstall nsadmin\n"
	@printf "\tmake install-slave\tInstall nsslave\n"

install-admin:
	$(CP_F) $(NSADMIN) $(DESTDIR)/
	$(CP_N) $(NSADMIN_CONF) $(CONFDIR)/

install-slave:
	$(CP_F) $(NSSLAVE) $(DESTDIR)/
	$(CP_N) $(NSSLAVE_CONF) $(CONFDIR)/

################################################################################
# END
################################################################################
