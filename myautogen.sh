#!/bin/sh
. ./autogen.sh
JMAX_TOPDIR=`echo $PWD`
./configure --with-javac=javac --with-java=java --prefix=${JMAX_TOPDIR}-install $*

