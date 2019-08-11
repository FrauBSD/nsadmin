############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-08-11 16:57:41 -0700 root $
#
############################################################ CONFIGURATION

DESTDIR=	/usr/local/bin
NSADMINDIR=	/etc/nsadmin

############################################################ PATHS

CAT=		cat
CP_F=		cp -f
MKDIR_P=	mkdir -p

############################################################ FUNCTIONS

EVAL2=		exec 3<&1; eval2(){ echo "$$*"; eval "$$@"; }

############################################################ OBJECTS

NSADMIN=	nsadmin \
		nsadmin-edit.inc \
		nsadmin-gen.inc \
		nsadmin-op.inc \
		nsadmin-var.inc \
		nsadmin-zone.inc \
		zone2rev.awk

NSSLAVE=	nsslave \
		nsslave-var.inc

NSSLAVE7=	nsslave-centos7 \
		nsslave-var.inc

############################################################ TARGETS

all install:
	@printf "Options:\n"
	@printf "\tmake install-admin\tInstall nsadmin\n"
	@printf "\tmake install-slave\tInstall nsslave\n"

install-admin:
	$(CP_F) $(NSADMIN) $(DESTDIR)/
	$(MKDIR_P) $(NSADMINDIR)/

install-slave:
	@$(EVAL2); \
	 case "$$( eval2 $(CAT) /etc/redhat-release )" in \
	 *" 6."*) set -- $(NSSLAVE) ;; \
	 *) set -- $(NSSLAVE7); \
	 esac; \
	 eval2 $(CP_F) -f $$* $(DESTDIR)/

################################################################################
# END
################################################################################
