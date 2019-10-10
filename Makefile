############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-10 11:36:29 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=	
BINDIR=		$(DESTDIR)/usr/local/bin
CONFDIR=	$(DESTDIR)/etc
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
	$(CP_F) $(NSADMIN) $(BINDIR)/
	$(CP_N) $(NSADMIN_CONF) $(CONFDIR)/

install-slave:
	$(CP_F) $(NSSLAVE) $(BINDIR)/
	$(CP_N) $(NSSLAVE_CONF) $(CONFDIR)/

################################################################################
# END
################################################################################
