############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-10 11:38:59 -0700 freebsdfrau $
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
	$(CP_F) $(NSADMIN_CONF) $(CONFDIR)/$(NSADMIN_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSADMIN_CONF).sample $(CONFDIR)/$(NSADMIN_CONF)

install-slave:
	$(CP_F) $(NSSLAVE) $(BINDIR)/
	$(CP_F) $(NSSLAVE_CONF) $(CONFDIR)/$(NSSLAVE_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSSLAVE_CONF).sample $(CONFDIR)/$(NSSLAVE_CONF)

################################################################################
# END
################################################################################
