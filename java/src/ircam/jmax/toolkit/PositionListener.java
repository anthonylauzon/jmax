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
 * The interface of the objects that needs to be informed 
 * when a position have been choosen by the user.
 * @see MouseTracker
 */
public interface PositionListener {
  /**
   * A position have been choosen.
   * This callback communicates also the modifier keys
   * pressed at the moment of the choice.
   */
  public abstract void positionChoosen(int x, int y, int modifier);
}
