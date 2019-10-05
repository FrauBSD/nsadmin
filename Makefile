############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-05 13:21:08 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=	/usr/local/bin
CONFDIR=	/etc
NSADMINDIR=	/etc/nsadmin

############################################################ PATHS

CAT=		cat
CP_F=		cp -f
CP_N=		cp -n
MKDIR_P=	mkdir -p

############################################################ FUNCTIONS

EVAL2=		exec 3<&1; eval2(){ echo "$$*"; eval "$$@"; }

############################################################ OBJECTS

NSADMIN=	nsadmin \
		nsadmin-zone.inc \
		zone2rev.awk
NSADMIN_CONF=	nsadmin.conf

NSSLAVE=	nsslave
NSSLAVE7=	nsslave-centos7
NSSLAVE_VAR=	nsslave-var.inc

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
	@$(EVAL2); \
	 case "$$( eval2 $(CAT) /etc/redhat-release )" in \
	 *" 6."*) NSSLAVE=$(NSSLAVE) ;; \
	 *) NSSLAVE=$(NSSLAVE7); \
	 esac; \
	 eval2 $(CP_F) $$NSSLAVE $(DESTDIR)/nsslave; \
	 eval2 $(CP_N) $(NSSLAVE_VAR) $(DESTDIR)/

################################################################################
# END
################################################################################
