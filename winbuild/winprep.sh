#!/bin/sh
# Create fts/version.c and java/src/ircam/jmax/JMaxVersion files
# Run this script in jmax top source directory.

. ../JMAX-VERSION 
JMAX_VERSION=${JMAX_MAJOR_VERSION}.${JMAX_MINOR_VERSION}.${JMAX_PATCH_VERSION}${JMAX_VERSION_STATUS}
sed -e 's/@VERSION@/'${JMAX_VERSION}/ < ../fts/version.c.in > ../fts/version.c
sed -e 's/@VERSION@/'${JMAX_VERSION}/ < ../java/src/ircam/jmax/JMaxVersion.java.in > ../java/src/ircam/jmax/JMaxVersion.java
bison -d ../fts/parser.y -o ../fts/parser.c
flex -o../fts/tokenizer.c ../fts/tokenizer.l
