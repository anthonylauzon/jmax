#!/usr/bin/perl -p

# $Id: fake_jmax.pl,v 1.1 2004/05/21 18:20:55 diemo Exp $
#
# $Log: fake_jmax.pl,v $
# Revision 1.1  2004/05/21 18:20:55  diemo
# input filter for doxygen to fake certain definitions for automatic
# documentation:
# - convert @fn documented macros to functions
# - todo: fake classes
#


# fake function
/\@fn\s(.+)/  &&  do {
    # keep given @fn prototype, 
    $fakefn = "$1;";
    # printf stderr "faked $_";
};

/\#define/  &&  $fakefn  &&  do {
    # overwrite macro definition with @fn
    $_ = "$fakefn\n";
    $fakefn = "";
};
