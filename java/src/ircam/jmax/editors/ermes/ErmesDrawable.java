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
package ircam.jmax.editors.ermes;

import java.awt.Graphics;

/**
 * The interface of all the object to paint in the sketch.
 * It handles the dirty flag (is to repaint?)
 */
public interface ErmesDrawable {
  abstract public void Paint(Graphics g);
}
