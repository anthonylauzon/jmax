#!/bin/sh

echo "[auto-tools versions in your path on $HOST]"
aclocal    --version | head -1
automake   --version | head -1
autom4te   --version | head -1
autoconf   --version | head -1
libtoolize --version | head -1
libtool    --version | head -1

rm -rf aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache

set -x
aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
libtoolize --force --automake --copy || glibtoolize --force --automake --copy
# autoheader
autoconf
automake --foreign --add-missing --copy

(
    cd client

    rm -rf aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache

    # client configure uses m4 macros defined in jmax root m4 directory (jar -u support)
    aclocal -I ../m4
#   autoheader
    autoconf
    automake --foreign --add-missing --copy
)



