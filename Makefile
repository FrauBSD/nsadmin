############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin on non-GNU systems $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-24 07:18:08 +0000 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=
TARGETS=	all \
		install \
		install-admin \
		install-next \
		install-slave \
		uninstall \
		uninstall-admin \
		uninstall-next \
		uninstall-slave

############################################################ TARGETS

$(TARGETS):
	$(MAKE) -f GNUmakefile CONFDIR=$(DESTDIR)/usr/local/etc $(MFLAGS) $(@)

################################################################################
# END
################################################################################
