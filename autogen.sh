#!/bin/sh

# unsetenv PERLLIB PERL5LIB
export PATH=/opt/local/bin:$PATH
# display versions
echo "[auto-tools versions in your path on $HOST]"
for f in aclocal automake autoconf autoheader autom4te libtoolize libtool; do
    $f --version | head -1
    # ls -lFgG `which $f`
done

rm -rf aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache

set -x
aclocal -I m4
# On Mac OS X, libtoolize is glibtoolize
libtoolize --force --automake --copy || glibtoolize --force --automake --copy
# autoheader
autoconf
automake --foreign --add-missing --copy

(
    cd client

    rm -rf aclocal.m4 config.* install-sh libtool ltconfig ltmain.sh missing mkinstalldirs autom4te.cache

    # client configure uses m4 macros defined in jmax root m4 directory (jar -u support)
    aclocal -I ../m4
#   autoheader
    autoconf
    automake --foreign --add-missing --copy
)



