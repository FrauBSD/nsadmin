############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-25 19:37:38 -0700 freebsdfrau $
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

NSNEXT=		nsnext
NSNEXT_CONF=	nsnext.conf

NSSLAVE=	nsslave
NSSLAVE_CONF=	nsslave.conf

############################################################ TARGETS

all install uninstall:
	@printf "Options:\n"
	@printf "\tmake install-admin\tInstall nsadmin\n"
	@printf "\tmake install-next\tInstall nsnext\n"
	@printf "\tmake install-slave\tInstall nsslave\n"
	@printf "\tmake uninstall-admin\tUninstall nsadmin\n"
	@printf "\tmake uninstall-next\tUninstall nsnext\n"
	@printf "\tmake uninstall-slave\tUninstall nsslave\n"

install-admin:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSADMIN) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSADMIN_CONF) $(CONFDIR)/$(NSADMIN_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSADMIN_CONF).sample $(CONFDIR)/$(NSADMIN_CONF)

install-admin:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSNEXT) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSNEXT_CONF) $(CONFDIR)/$(NSNEXT_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSNEXT_CONF).sample $(CONFDIR)/$(NSNEXT_CONF)

install-slave:
	$(MKDIR_P) $(BINDIR)
	$(CP_F) $(NSSLAVE) $(BINDIR)/
	$(MKDIR_P) $(CONFDIR)
	$(CP_F) $(NSSLAVE_CONF) $(CONFDIR)/$(NSSLAVE_CONF).sample
	$(CP_N) $(CONFDIR)/$(NSSLAVE_CONF).sample $(CONFDIR)/$(NSSLAVE_CONF)

uninstall-admin:
	$(RM_F) $(BINDIR)/$(NSADMIN)
	CONF=$(CONFDIR)/$(NSADMIN_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSADMIN_CONF).sample

uninstall-next:
	$(RM_F) $(BINDIR)/$(NSNEXT)
	CONF=$(CONFDIR)/$(NSNEXT_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSNEXT_CONF).sample

uninstall-slave:
	$(RM_F) $(BINDIR)/$(NSSLAVE)
	CONF=$(CONFDIR)/$(NSSLAVE_CONF); \
		! $(CMP_S) $$CONF.sample $$CONF || $(RM_F) -v $$CONF
	$(RM_F) $(CONFDIR)/$(NSSLAVE_CONF).sample

################################################################################
# END
################################################################################
