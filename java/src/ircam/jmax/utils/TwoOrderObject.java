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

package ircam.jmax.utils;

/**
 * The interface of objects that can be inserted in a TDTree.
 * These objects have two comparison functions, used
 * at alternatives levels of the TDT.
 */
public interface TwoOrderObject {
  abstract public boolean firstLessOrEqual(TwoOrderObject obj);
  abstract public boolean secondLessOrEqual(TwoOrderObject obj);
  abstract public int getFirst();
  abstract public int getSecond();

  public final int FIRST = 0;
  public final int SECOND = 1;
}



