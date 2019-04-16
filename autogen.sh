#!/bin/bash

name=ssr2json
ver=1.0.0
bug_report_address=https://github.com/fkxxyz/ssr2json/issues

extra_tool='ssr2jsonsave ssrsub ssrsubsave'

[ "$1" == "clean" ] && \
	rm -rf aclocal.m4 autom4te.cache compile configure install-sh \
	autoscan.log config.h.in config.log configure.ac missing configure.scan \
	Makefile.am config.status COPYING depcomp INSTALL Makefile.in Makefile \
	stamp-h1 config.h .deps src/{Makefile.am,Makefile,Makefile.in,.deps} \
	bin/{Makefile.am,Makefile,Makefile.in,.deps} && \
	exit

bin_PROGRAMS=${name}
eval ${bin_PROGRAMS}_SOURCES=\"$(cd src && echo *.cpp)\"

echo "
AUTOMAKE_OPTIONS = foreign
SUBDIRS = src bin
" > Makefile.am

echo "
bin_PROGRAMS = ${bin_PROGRAMS}
${bin_PROGRAMS}_SOURCES = $(eval echo \${${bin_PROGRAMS}_SOURCES})
" > src/Makefile.am

echo "
dist_bin_SCRIPTS = ${extra_tool}
" > bin/Makefile.am

autoscan
sed -e "/AC_INIT(.*)/aAM_INIT_AUTOMAKE" \
	-e "s/AC_INIT(.*)/AC_INIT([${name}], [${ver}], [${bug_report_address}])/g" \
	configure.scan \
	> configure.ac
autoreconf -if
