############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/GNUmakefile 2019-11-01 14:59:51 -0700 freebsdfrau $
#
############################################################ INFORMATION
#
# DO NOT USE GNU EXTENSIONS IN THIS FILE
# THIS FILE MUST REMAIN USABLE BY NON-GNU MAKE
#
############################################################ CONFIGURATION

DESTDIR=	
BINDIR=		$(DESTDIR)/usr/local/bin
CONFDIR=	$(DESTDIR)/etc

############################################################ PATHS

CMP_S=		cmp -s
CP_F=		cp -f
CP_N=		cp -n
MKDIR_P=	mkdir -p
RM_F=		rm -f

############################################################ OBJECTS

NSADMIN=	nsadmin
NSADMIN_CONF=	nsadmin.conf

NSAXFR=		nsaxfr
NSAXFR_CONF=	nsaxfr.conf

NSNEXT=		nsnext
NSNEXT_CONF=	nsnext.conf

############################################################ TARGETS

all install uninstall:
	@printf "Options:\n"
	@printf "\tmake install-admin\tInstall nsadmin\n"
	@printf "\tmake install-axfr\tInstall nsaxfr\n"
	@printf "\tmake install-next\tInstall nsnext\n"
	@printf "\tmake uninstall-admin\tUninstall nsadmin\n"
	@printf "\tmake uninstall-axfr\tUninstall nsaxfr\n"
	@printf "\tmake uninstall-next\tUninstall nsnext\n"

install-admin:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSADMIN) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSADMIN_CONF) $(CONFDIR)/$(NSADMIN_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSADMIN_CONF).sample $(CONFDIR)/$(NSADMIN_CONF)

install-axfr:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSAXFR) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSAXFR_CONF) $(CONFDIR)/$(NSAXFR_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSAXFR_CONF).sample $(CONFDIR)/$(NSAXFR_CONF)

install-next:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSNEXT) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSNEXT_CONF) $(CONFDIR)/$(NSNEXT_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSNEXT_CONF).sample $(CONFDIR)/$(NSNEXT_CONF)

uninstall-admin:
	$(RM_F) $(BINDIR)/$(NSADMIN)
	CONF=$(CONFDIR)/$(NSADMIN_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSADMIN_CONF).sample

uninstall-axfr:
	$(RM_F) $(BINDIR)/$(NSAXFR)
	CONF=$(CONFDIR)/$(NSAXFR_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSAXFR_CONF).sample

uninstall-next:
	$(RM_F) $(BINDIR)/$(NSNEXT)
	CONF=$(CONFDIR)/$(NSNEXT_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSNEXT_CONF).sample

################################################################################
# END
################################################################################
