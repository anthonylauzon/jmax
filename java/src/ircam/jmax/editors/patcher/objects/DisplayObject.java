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

package ircam.jmax.editors.patcher.objects;


import java.awt.*;

/** The interface that everything representing a part of the screen
  (objects, connections, sensible areas) 
  should implement. For the moment is empty ... it may stay empty
  or it can get more complex
  */


public interface DisplayObject
{
  public SensibilityArea getSensibilityAreaAt( int mouseX, int mouseY);
  abstract public void paint(Graphics g);
  //  abstract public void redraw();
  abstract public boolean intersects(Rectangle r);
}
