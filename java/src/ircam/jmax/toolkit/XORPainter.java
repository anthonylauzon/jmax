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
package ircam.jmax.toolkit;

/**
 * The interface of the graphic clients of a XORHandler object.
 * Such clients are informed when an erase or a draw operation is
 * needed. For instruction,
 * @see XORHandler
 */ 
public interface XORPainter {

  /**
   * Undo the last drawing */
  public abstract void XORErase();
  /**
   * make a new draw in the new position */
  public abstract void XORDraw(int dx, int dy);
}
