## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

foobar_serverdocdir = ${prefix}/doc/foobar_server
foobar_serverdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(foobar_serverdoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

