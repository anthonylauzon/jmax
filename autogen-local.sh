#!/bin/sh

# $Id: autogen-local.sh,v 1.1 2005/02/03 16:33:12 diemo Exp $
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
/opt/local/bin/libtoolize --version | head -1
/opt/local/bin/libtool    --version | head -1

rm -fr aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache
set -x

${prefix}aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
/opt/local/bin/libtoolize --force --automake --copy || glibtoolize --force --automake --copy
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


