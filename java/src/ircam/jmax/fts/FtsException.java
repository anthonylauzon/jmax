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

/** 
 * The generic exception for fts.
 * Very primitive at the moment, just keep a string explaining the detail
 * of the exception; should at least embed the native exception generating
 * the original error (if any), and should may be code also the 
 * gravity of the situation (style, should the server be just closed, should
 * the user be warned, can we safely abort the current operation but continue
 * to use the server, and so on).
 */

public class FtsException extends Exception
{
  FtsException(String detail)
  {
    super(detail);
  }
}
