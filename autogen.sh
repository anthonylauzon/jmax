#!/bin/sh
aclocal -I m4
autoconf
automake --foreign
./configure --prefix=`pwd`-install $*



