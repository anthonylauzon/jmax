#! /bin/sh
DSTDIR=/usr/local/www/ircam/equipes/temps-reel/jmax

set -x

rm -rf ${DSTDIR}/*

cat <<EOF > ${DSTDIR}/index.html
<HTML>
<BODY>
<CENTER> <BLINK> <H1> Update in progress. Please wait ... </H1> </BLINK> </CENTER>
</BODY>
</HTML>
EOF

copy_file ()
{
    cp $1 $2
    chmod 664 $2
}

mkdir ${DSTDIR}/images
for i in *.html images/*.gif
do
    copy_file $i ${DSTDIR}/$i
done

for d in `find doc -follow -type d \! \( -name CVS -prune \) `
do
    mkdir -p ${DSTDIR}/$d
    chmod 775 ${DSTDIR}/$d
done

for i in `find doc -follow -type f \( -name '*.html' -o -name '*.gif' \)`
do
    copy_file $i ${DSTDIR}/$i
done


