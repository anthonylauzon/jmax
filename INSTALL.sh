#!/bin/sh
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

