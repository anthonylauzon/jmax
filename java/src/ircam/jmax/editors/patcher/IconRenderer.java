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
import java.awt.event.*;

import ircam.jmax.editors.patcher.objects.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (Message, Standard).
// That show an icon.
//

public class IconRenderer implements ObjectRenderer
{
  Icon icon;
  private Editable owner;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public IconRenderer(Editable object, Icon i)
  {
    super();

    owner = object;
    icon = i;
  }

  public void update()
  {
  }

  public boolean canResizeWidthTo(int width)
  {
    return width ==  icon.getIconWidth();
  }

  public int getHeight()
  {
    return icon.getIconHeight();
  }

  public int getWidth()
  {
    return icon.getIconWidth();
  }

  public void setBackground(Color color)
  {
  }

  public void prepareToRender()
  {
  }

  public void render(Graphics g, int x, int y, int w, int h)
  {
    icon.paintIcon(owner.getSketchPad(), g, x, y);
  }
}

