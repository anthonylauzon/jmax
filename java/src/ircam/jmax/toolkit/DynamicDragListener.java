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

package ircam.jmax.toolkit;

/**
 * The interface of the objects that wants to 
 * dynamically follow drag operations. Unlike the DragListener interface,
 * this IM tracks the beginning and every movement, not just the end.
 * @see DragListener 
 * @see InteractionModule*/
public interface DynamicDragListener extends DragListener {
  
  /**
   * Begin the drag */
  public abstract void dragStart(int x, int y);

  /**
   * Continue the drag...*/
  public abstract void dynamicDrag(int x, int y);

}
