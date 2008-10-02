## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gvpn_gtk/glade
glade_DATA = \
	gvpn.glade

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GVPN_GTK_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gvpn_gtk

gvpn_gtk_SOURCES = \
	main.cc   \
	cfg.c   \
	cfg.h   \
	gvpn-ui.cpp   \
	gvpn-ui.hpp   \
	comm-client.hpp   \
	comm-client.cpp   \
	gvpn-core.cpp   \
	gvpn-core.hpp   \
	gvpn-config.hpp   \
	gvpn-config.cpp

gvpn_gtk_LDFLAGS = 

gvpn_gtk_LDADD = $(GVPN_GTK_LIBS)

EXTRA_DIST = $(glade_DATA) \
	icon.png\
	icon_c.png\
	icon_d.png
