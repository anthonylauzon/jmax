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
package ircam.jmax.editors.table;

/**
 * The interface used to communicate between the LinerTool and its
 * LineDrawer interface module.
 */
public interface LineListener {

  public abstract void lineStart(int x, int y, int modifiers);
  public abstract void lineEnd(int x, int y);
}
