#!/bin/sh
. ./autogen.sh
JMAX_TOPDIR=`echo $PWD`
./configure --prefix=${JMAX_TOPDIR}-install --enable-debug $*

