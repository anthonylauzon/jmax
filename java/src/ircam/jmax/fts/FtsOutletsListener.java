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

/**
 * A specialized listener that listen to changes of the number of outlets
 * of an object.
 */

public interface FtsOutletsListener
{
  public void outletsChanged(int value);
}
