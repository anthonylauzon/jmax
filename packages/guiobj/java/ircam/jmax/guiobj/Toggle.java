 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.*;

import ircam.jmax.fts.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "toggle" graphic object.
//

public class Toggle extends GraphicObject implements FtsIntValueListener
{
  static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 15;

  private static final Color itsCrossColor = new Color(0, 0, 128);

  private boolean isToggled = false;

  public Toggle(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);
  }

  public void setDefaults()
  {
    setWidth( getWidth());
  }

  // redefined from base class

  public void setWidth( int theWidth)
  {
    if ( theWidth <= 0)
      setWidth( DEFAULT_WIDTH);
    else if ( theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class

  public void setHeight( int theHeight)
  {
  }

  public boolean isSquare()
  {
    return true;
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
	((FtsToggleObject)ftsObject).sendBang();
  }

  public void valueChanged(int value) 
  {
    isToggled = (value == 1);

    updateRedraw();
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( !isSelected())
	g.setColor( Settings.sharedInstance().getUIColor());
    else
	g.setColor( Settings.sharedInstance().getUIColor().darker());
    
    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    if (isToggled) 
      {
	g.setColor( itsCrossColor);
	g.drawLine( x + 4, y + 4, x + w - 6, y + h - 6);
	g.drawLine( x + w - 6, y + 4, x + 4,y + h - 6);
      }
    
    super.paint( g);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.setColor( Settings.sharedInstance().getUIColor());

    g.fillRect( x + 3, y + 3, w - 6, h - 6);

    if (isToggled) 
      {
	g.setColor( itsCrossColor);
	g.drawLine( x + 4, y + 4, x + w - 6, y + h - 6);
	g.drawLine( x + w - 6, y + 4, x + 4,y + h - 6);
      }
  }  
}





