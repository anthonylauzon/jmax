 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.*;

import ircam.jmax.fts.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;


public class Input extends GraphicObject implements FtsIntValueListener
{
  private static transient final int FIXED_WIDTH = 60;
  private static transient final int FIXED_HEIGHT = 28;

  private transient boolean isOn = false;
  private int iconWidth;
  private static Color deselectedColor = new Color(204, 204, 204);
  private static Color selectedColor = deselectedColor.darker();

  public Input( FtsGraphicObject theFtsObject) 
  {
    super( theFtsObject);

    iconWidth = JMaxIcons.audioin_on.getIconWidth() + 10;
  }

  public void setDefaults()
  {
    super.setWidth( FIXED_WIDTH);
    super.setHeight( FIXED_HEIGHT);
  }

  public void setWidth(int w)
  {
    if(w < iconWidth)
      w = iconWidth;
    super.setWidth(w);
  }

  public void setHeight(int h)
  {
  }

  public void redefined()
  {
    setDefaults();
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      ((FtsInputObject)ftsObject).sendBang();
  }

  public void valueChanged(int value) 
  {
    isOn = (value == 1);
    redraw();
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( !isSelected())
      g.setColor( deselectedColor);
    else
      g.setColor( selectedColor);
    
    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    int ix, iy;
    if( ftsObject.isError())
      {
	ix = x + (w - JMaxIcons.no_audioin.getIconWidth())/2;
	iy = y + (h - JMaxIcons.no_audioin.getIconHeight())/2;
	g.drawImage(JMaxIcons.no_audioin.getImage(), ix, iy, JMaxIcons.no_audioin.getImageObserver());
      }    
    else 
      if(isOn)
	{
	  ix = x + (w - JMaxIcons.audioin_on.getIconWidth())/2;
	  iy = y + (h - JMaxIcons.audioin_on.getIconHeight())/2;
	  g.drawImage(JMaxIcons.audioin_on.getImage(), ix, iy, JMaxIcons.audioin_on.getImageObserver());
	}      
      else
	{
	  ix = x + (w - JMaxIcons.audioin_off.getIconWidth())/2;
	  iy = y + (h - JMaxIcons.audioin_off.getIconHeight())/2;
	  g.drawImage(JMaxIcons.audioin_off.getImage(), ix, iy, JMaxIcons.audioin_off.getImageObserver());
	}
    super.paint( g);
  }
}
