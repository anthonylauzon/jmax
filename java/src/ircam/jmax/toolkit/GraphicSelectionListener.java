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
 * Interface of the objects that wants to listen to a geometric
 * selection in a plane
 */ 
public interface GraphicSelectionListener {
  
  /** a rectangular selection have been choosen */
  public abstract void selectionChoosen(int x, int y, int w, int h);

  /** a point have been choosen */
  public abstract void selectionPointChoosen(int x, int y, int modifiers);
}


