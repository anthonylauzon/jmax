#!/bin/sh
#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

help ()
{
    cat << HELP_EOF

Usage:
    INSTALL

Do all the post-detar stuff to install jMax, mainly
chown and chmod on fts executable to allow high priority execution.

Note:
 - this script must be run as root.

Arguments: none
HELP_EOF
}

if [ `whoami` != root ]
then
    echo "This script must be run as root."
    exit 2
fi

# Try to change mode to setuid for the FTS executables
for fts in `find . -name fts -type f -perm -u+x`
do
    echo "Changing mode of $fts to allow high priority execution"
    chown root $fts
    chgrp root $fts
    chmod u+s $fts
done

# On SGI, check maximum lockable memory
if [ -x /usr/sbin/systune ]
then
    MAXLKMEM=`/usr/sbin/systune maxlkmem | awk '{ print $3;}'`
    if [ $MAXLKMEM -lt 2000 ]
    then
	echo ""
	echo "WARNING:"
	echo "System parameter \"maxlkmem\" has value: $MAXLKMEM"
	echo "This value may degrade real-time performance for some jMax applications."
	echo "Please consider increasing it with the command :"
	echo "\"/usr/sbin/systune maxlkmem VALUE\""
	echo ""
	echo "See \"man systune\" for further details."
    fi
fi
