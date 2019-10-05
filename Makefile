############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-05 15:08:16 -0700 freebsdfrau $
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

NSADMIN=	nsadmin \
		zone2rev.awk
NSADMIN_CONF=	nsadmin.conf

NSSLAVE=	nsslave
NSSLAVE7=	nsslave-centos7
NSSLAVE_CONF=	nsslave.conf

############################################################ TARGETS

all install:
	@printf "Options:\n"
	@printf "\tmake install-admin\tInstall nsadmin\n"
	@printf "\tmake install-slave\tInstall nsslave\n"

install-admin:
	$(CP_F) $(NSADMIN) $(DESTDIR)/
	$(CP_N) $(NSADMIN_CONF) $(CONFDIR)/
	$(MKDIR_P) $(NSADMINDIR)/RCS

install-slave:
	$(CP_F) $(NSSLAVE) $(DESTDIR)/
	$(CP_N) $(NSSLAVE_CONF) $(CONFDIR)/

################################################################################
# END
################################################################################
