#! /bin/sh
DSTDIR=/usr/local/www/ircam/equipes/temps-reel/jmax
rm -rf ${DSTDIR}/*
cat <<EOF > ${DSTDIR}/index.html
<HTML>
<BODY>
<CENTER> <BLINK> <H1> Update in progress. Please wait ... </H1> </BLINK> </CENTER>
</BODY>
</HTML>
EOF
cd ${DSTDIR}
cvs checkout max/homepage
mv -f max/homepage/* .
/bin/rm -rf CVS

