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
package  ircam.jmax.fts;

/** 
 * An FtsUpdateGroupListener is informed when 
 * an update group start, and when it finish.
 * 
 */

public interface FtsUpdateGroupListener
{
  abstract public void updateGroupStart();
  abstract public void updateGroupEnd();
}
