## Process this file with automake to produce Makefile.in

man5_MANS	=
man8_MANS	=

RPCPREFIX	=
KPREFIX		= @kprefix@
sbin_PROGRAMS	= spnfsd

EXTRA_DIST = \
	$(man5_MANS) \
	$(man8_MANS) \
	spnfsd.conf

spnfsd_SOURCES = \
	atomicio.c \
	spnfsd_ops.c \
	cfg.c \
	spnfsd.c \
	strlcat.c \
	strlcpy.c \
	\
	cfg.h \
	spnfsd.h \
	spnfsd_queue.h \
	queue.h

spnfsd_LDADD = -levent ../../support/nfs/libnfs.a

MAINTAINERCLEANFILES = Makefile.in
