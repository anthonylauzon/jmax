#!/usr/bin/perl -p

# $Id$
#
# $Log$


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
