package ircam.jmax.fts;

/** This class represent a void atom value in the protocol.
  There is nothing to know about a void value; it just means
  that there is no value (the atom equivalent of a null).
  */


public class FtsVoid
{
  static FtsVoid voidValue = new FtsVoid();
  
  FtsVoid()
  {
  }
}
