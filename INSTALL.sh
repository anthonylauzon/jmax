#!/bin/sh
# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
