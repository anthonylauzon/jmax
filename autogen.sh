#!/bin/sh
set -x
aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
libtoolize --automake --copy 
autoconf
automake --foreign --add-missing --copy
(
    cd client
    aclocal
    autoconf
    automake --foreign --add-missing --copy
)



