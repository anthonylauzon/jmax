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

import ircam.jmax.mda.*;

/**
 * A specialized listener that is called when 
 * a new data value is available for an object
 * New  Data Listener are installed and remove globally
 * in the Fts class.
 */

public interface FtsNewDataListener
{
  public void newDataFor(FtsObject obj, MaxData data);
}
