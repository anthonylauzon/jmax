//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

/** The generic exception for fts.
 * Very primitive at the moment.
 */

public class FtsException extends Exception
{
  FtsException(String detail)
  {
    super(detail);
  }
}
