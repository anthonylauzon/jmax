//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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
