############################################################ IDENT(1)
#
# $Title: Makefile for installing nsadmin on non-GNU systems $
# $Copyright: 2019 Devin Teske. All rights reserved. $
# $FrauBSD: nsadmin/Makefile 2019-11-01 14:55:33 -0700 freebsdfrau $
#
############################################################ CONFIGURATION

DESTDIR=
TARGETS=	all \
		install \
		install-admin \
		install-axfr \
		install-next \
		uninstall \
		uninstall-admin \
		uninstall-axfr \
		uninstall-next

############################################################ TARGETS

$(TARGETS):
	$(MAKE) -f GNUmakefile CONFDIR=$(DESTDIR)/usr/local/etc $(MFLAGS) $(@)

################################################################################
# END
################################################################################
