#!/bin/sh
set -x
JMAX_TOPDIR=`echo $PWD`
aclocal -I m4
libtoolize --automake --copy
autoconf
automake --foreign -v --add-missing --copy
(
    cd client
    aclocal
    autoconf
)
./configure --prefix=${JMAX_TOPDIR}-install $*



