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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.editors.patcher.menus.*;

//
// The "bang" graphic object.
//

public class Bang extends GraphicObject implements FtsIntValueListener
{
  private Color itsFlashColor = Settings.sharedInstance().getUIColor();
  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 15;
  private static final int CIRCLE_ORIGIN = 3;
  private static final int DEFAULT_FLASH_DURATION = 125;

  public Bang( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    int width = getWidth();
    if (width == -1)
      setWidth( DEFAULT_WIDTH);
    else if (width <= MINIMUM_WIDTH)
      setWidth( MINIMUM_WIDTH);
  }

  // redefined from base class
  public void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class
  public void setHeight( int theHeight)
  {
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      ftsObject.sendMessage( -1, "bang", null);
  }

  public void valueChanged(int value) 
  {
    int flash = value;

    if (flash <= 0)
      itsFlashColor = Settings.sharedInstance().getUIColor();
    else
      itsFlashColor = ColorPopUpMenu.getColorByIndex(flash);

    updateRedraw();
  }

  public void paint( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( isSelected())
	g.setColor( Settings.sharedInstance().getUIColor().darker());
    else
	g.setColor( Settings.sharedInstance().getUIColor());

    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    g.setColor( itsFlashColor);

    int delta = w/5;
    g.fillRect( x + delta + 1,
		y + delta + 1,
		w - 2*(delta+1),
		h - 2*(delta+1));

    g.setColor(Color.black);
    g.drawRect( x + delta, 
		y + delta, 
		w - 2*delta - 1,
		h - 2*delta - 1);
      
    super.paint( g);
  }
  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    
    g.setColor( itsFlashColor);

    /*g.fillOval( x + CIRCLE_ORIGIN + 1,
      y + CIRCLE_ORIGIN + 1,
      w - 2*(CIRCLE_ORIGIN+1) - 1,
      h - 2*(CIRCLE_ORIGIN+1) - 1);*/    
    int delta = w/5;
    g.fillRect( x + delta + 1,
		y + delta + 1,
		w - 2*(delta+1),
		h - 2*(delta+1));
  }

  //popup interaction 
  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    BangPopUpMenu.update(this);
    ObjectPopUp.addMenu(BangPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(BangPopUpMenu.getInstance());
  }
}


