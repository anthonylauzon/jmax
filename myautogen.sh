#!/bin/sh
. ./autogen.sh
JMAX_TOPDIR=`echo $PWD`
./configure --prefix=${JMAX_TOPDIR}-install --with-javac=javac --with-java=java $*

