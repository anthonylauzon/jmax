#!/bin/sh

# $Id$
#
# This autogen script calls the auto* tools in a specific place

PERLLIB=
PERL5LIB=

prefix=/usr/bin/

echo "[auto-tools version in $prefix on $HOST]"
${prefix}aclocal    --version | head -1
${prefix}automake   --version | head -1
${prefix}autom4te   --version | head -1
${prefix}autoconf   --version | head -1
${prefix}libtoolize --version | head -1
${prefix}libtool    --version | head -1

rm -fr aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache
set -x

${prefix}aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
${prefix}libtoolize --force --automake --copy || glibtoolize --force --automake --copy
#autoheader
${prefix}autoconf
${prefix}automake --foreign --add-missing --copy
(
    cd client
    rm -fr aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache
    # client configure use m4 macro defined in jmax root m4 directory (jar -u support)
    ${prefix}aclocal -I ../m4
#    autoheader
    ${prefix}autoconf
    ${prefix}automake --foreign --add-missing --copy
)

# then do:
# ./configure --prefix $PWD/../jmax-install --enable-debug --disable-libtool-lock


