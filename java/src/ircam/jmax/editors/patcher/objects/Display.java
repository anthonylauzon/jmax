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

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The display graphic object.
//

class Display extends GraphicObject implements FtsMessageListener
{
  String display = null; // the display content
  int minWidth = 0; //

  public Display(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    redefine("     ");

    setFont(getFont());
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    return display;
  }

  public void redefine( String text) 
  {
    int w = getFontMetrics().stringWidth(text) + 4;
    int h = getFontMetrics().getHeight() + 4;

    display = text;

    if(w < minWidth)
      super.setWidth(minWidth);
    else
      super.setWidth( w);

    super.setHeight( h);
  }

  // Set the text when FTS change the display content
  public void messageChanged(String text)
  {
    redraw();
    redefine( text);
    redraw();
  }

  public void setWidth( int theWidth) 
  {
  }

  public void setHeight( int theHeight)
  {
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {          
  }

  // redefined from base class
  public void setFont( Font theFont)
  {
    super.setFont( theFont);

    int w = getFontMetrics().stringWidth(display) + 4;
    int h = getFontMetrics().getHeight() + 4;

    minWidth = h;

    if(w < minWidth)
      super.setWidth(minWidth);
    else
      super.setWidth( w);

    super.setHeight( h);

    redraw();
  }

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  public Color getTextForeground()
  {
    return Color.black;
  }

  public Color getTextBackground()
  {
    if (itsSketchPad.isLocked())
      return Color.white;
    else
      {
	if (isSelected())
	  return Color.gray;
	else
	  return itsSketchPad.getBackground();
      }
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if(itsSketchPad.isLocked())
      {
	g.setColor( Color.white);
	g.fillRect( x + 1, y + 1, w - 2, h - 2);

	g.setColor( Color.black);
	//g.drawLine( x, y, x + w - 1, y);
	g.drawLine( x, y, x + minWidth, y);
	g.drawLine( x, y, x, y + h/4);
	
	paintInlets(g);
	
	g.setColor( Color.gray);
	g.setFont(getFont());
	g.drawString(display, x + 2, y + h - getFontMetrics().getDescent() - 2);
      }
    else
      {
	if (isSelected())
	  g.setColor(Color.gray);
	else
	  g.setColor(itsSketchPad.getBackground());

	g.fill3DRect( x, y + 1, w, h - 1, true); 

	g.setColor( Color.black);
	//g.drawLine( x, y, x + w - 1, y);
	g.drawLine( x, y, x + minWidth, y);
	g.drawLine( x, y, x, y + h/4);

	paintInlets(g);
	
	g.setFont(getFont());
	g.drawString(display, x + 2, y + h - getFontMetrics().getDescent() - 2);
      }
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.setFont(getFont());
    g.drawString(display, x + 2, y + h - getFontMetrics().getDescent() - 2);
  }

  public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    TextPopUpMenu.update(this);
    ObjectPopUp.addMenu(TextPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(TextPopUpMenu.getInstance());
  }
}
