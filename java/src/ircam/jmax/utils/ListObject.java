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

public class ListObject {
  public Object itsObject;
  public ListObject next;
  public ListObject previous;
  
  public ListObject(Object theObject) {
    itsObject = theObject;
    next = null;
    previous = null;
  }
}
