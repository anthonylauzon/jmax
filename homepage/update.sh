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
    chmod 664 $2/`basename $1`
}

for i in *.html
do
    copy_file $i ${DSTDIR}
done

for d in `find doc -type d`
do
    echo "directory " $d
    mkdir -p ${DSTDIR}/$d
    chmod 775 ${DSTDIR}/$d
done

for i in `find doc -type f \( -name '*.html' -o -name '*.gif' \)`
do
    echo "file " $d
    copy_file $i ${DSTDIR}
done


