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
 * The interface used by objects that listen to dragging operations,
 * such as the moving tools. The information passed back is limited 
 * to the end of the interaction.
 * Use DynamicDragListener for richer set of informations.
 * @see SelectionMover
 * @see DynamicDragListener 
 */
public interface DragListener {
  
  /**
   * informs that the drag ended at the point x,y
   */
  public abstract void dragEnd(int x, int y);
}



