#!/bin/sh
rm aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs
set -x
aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
libtoolize --force --automake --copy || glibtoolize --force --automake --copy
autoconf
automake --foreign --add-missing --copy
(
    cd client
    aclocal
    autoconf
    automake --foreign --add-missing --copy
)



