#!/bin/sh
JMAX_TOPDIR=`echo $PWD`
aclocal -I m4
autoconf
automake --foreign
(
    cd client
    aclocal
    autoconf
)
./configure --prefix=${JMAX_TOPDIR}-install $*



