package ircam.jmax.fts;

import tcl.lang.*;

/**
 * A property representation is an object able to convert
 * a property value expressed by a TclObject in the proper Java representation
 * and a Java property value in the proper TclObject; it is used
 * for parsing and unparsing of properties, both in saving/loading
 * and in the set and get command; each is installed 
 * with the static method installPropertyRepresentation of the class ???
 */

public interface FtsTclPropertyParser
{
  String unparse(Object value);
  Object parse(Interp interp, TclObject value);
}
