############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin on non-GNU systems $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-10-31 09:10:49 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=
TARGETS=	all \
		install \
		install-admin \
		install-next \
		install-axfr \
		uninstall \
		uninstall-admin \
		uninstall-next \
		uninstall-axfr

############################################################ TARGETS

$(TARGETS):
	$(MAKE) -f GNUmakefile CONFDIR=$(DESTDIR)/usr/local/etc $(MFLAGS) $(@)

################################################################################
# END
################################################################################
