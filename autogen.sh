#!/bin/sh
rm aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs
set -x
aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
libtoolize --force --automake --copy || glibtoolize --force --automake --copy
#autoheader
autoconf
automake --foreign --add-missing --copy
(
    cd client
    # client configure use m4 macro defined in jmax root m4 directory (jar -u support)
    aclocal -I ../m4
#    autoheader
    autoconf
    automake --foreign --add-missing --copy
)



