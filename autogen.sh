#!/bin/sh
automake --foreign
aclocal -I m4
autoconf
./configure --prefix=`pwd`-install $*



