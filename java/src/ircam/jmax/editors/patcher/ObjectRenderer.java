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
package ircam.jmax.editors.patcher;

import java.awt.*;

//
// The edit field contained in the editable objects (Message, Standard).
//

public interface ObjectRenderer
{
  public void update();

  public boolean canResizeWidthTo(int width);

  public int getHeight();

  public int getWidth();

  public void setBackground(Color color);

  public void render(Graphics g, int x, int y, int w, int h);
}



