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

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;


public class Monitor extends GraphicObject implements FtsIntValueListener
{
  private static transient final int FIXED_WIDTH = 60;
  private static transient final int FIXED_HEIGHT = 24;

  private transient static final Color itsCrossColor = new Color(0, 0, 128);

  private transient boolean isOn = false;
  private int iconWidth;
  private static Color deselectedColor = new Color(204, 204, 204);
  private static Color selectedColor = deselectedColor.darker();

  public Monitor(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);

    iconWidth = JMaxIcons.dspOn.getIconWidth() + 10;
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

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      ((FtsMonitorObject)ftsObject).sendBang();
  }

  public void valueChanged(int value) 
  {
    isOn = (value == 1);

    updateRedraw();
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

    int ix = x + (w - JMaxIcons.dspOn.getIconWidth())/2;
    int iy = y + (h - JMaxIcons.dspOn.getIconHeight())/2;

    if( ftsObject.isError())
      g.drawImage(JMaxIcons.dspUndef.getImage(), ix, iy, JMaxIcons.dspUndef.getImageObserver());
    else 
      if(isOn)
	g.drawImage(JMaxIcons.dspOn.getImage(), ix, iy, JMaxIcons.dspOn.getImageObserver());
      else
	g.drawImage(JMaxIcons.dspOff.getImage(), ix, iy, JMaxIcons.dspOff.getImageObserver());

    super.paint( g);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( !isSelected())
      g.setColor( deselectedColor);
    else
      g.setColor( selectedColor);

    g.fillRect( x + 3, y + 3, w - 6, h - 6);

    int ix = x + (w - JMaxIcons.dspOn.getIconWidth())/2;
    int iy = y + (h - JMaxIcons.dspOn.getIconHeight())/2;

    if( ftsObject.isError())
      g.drawImage(JMaxIcons.dspUndef.getImage(), ix, iy, JMaxIcons.dspUndef.getImageObserver());
    else 
      if(isOn)
	g.drawImage(JMaxIcons.dspOn.getImage(), ix, iy, JMaxIcons.dspOn.getImageObserver());
      else
	g.drawImage(JMaxIcons.dspOff.getImage(), ix, iy, JMaxIcons.dspOff.getImageObserver());
  }  
}
