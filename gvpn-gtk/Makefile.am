## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po 

EXTRA_DIST = $(gvpndoc_DATA) \
	$(link_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

desktopdir = $(datadir)/applications
desktop_in_files = src/GVPN.desktop
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
