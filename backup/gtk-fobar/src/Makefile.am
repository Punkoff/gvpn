## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gtk_foobar/glade
glade_DATA = gtk_foobar.glade

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GTK_FOOBAR_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gtk_foobar

gtk_foobar_SOURCES = \
	main.cc

gtk_foobar_LDFLAGS = 

gtk_foobar_LDADD = $(GTK_FOOBAR_LIBS)

EXTRA_DIST = $(glade_DATA)
