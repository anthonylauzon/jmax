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

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The display graphic object.
//

public class Display extends GraphicObject implements FtsMessageListener
{
  String display = null; // the display content
  int minWidth = ObjectGeometry.INOUTLET_PAD + ObjectGeometry.HIGHLIGHTED_INOUTLET_WIDTH;
  int underWidth = 0;
  int underWidthMax = 0;

  public Display(ErmesSketchPad theSketchPad, FtsGraphicObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    display = "###";

    int w = getFontMetrics().stringWidth(display) + 4;
    int h = getFontMetrics().getHeight() + 4;

    if(w < minWidth)
      ftsObject.setCurrentWidth(ScaleTransform.getInstance().invScaleX(minWidth));
    else
      ftsObject.setCurrentWidth(ScaleTransform.getInstance().invScaleX(w));

    if(w < underWidthMax)
      underWidth = w;
    else
      underWidth = underWidthMax;

    setFont(getFont());
  }

  // ---------------------------------------- 
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    return display;
  }

  public void redefined()
  {
      messageChanged(display);
  } 

  // Set the text when FTS change the display content
  public void messageChanged(String text)
  {
    int w = getFontMetrics().stringWidth(text) + 4;
    int h = getFontMetrics().getHeight() + 4;

    redraw();

    display = text;

    if(w < minWidth)
      ftsObject.setCurrentWidth(ScaleTransform.getInstance().invScaleX(minWidth));
    else
      ftsObject.setCurrentWidth(ScaleTransform.getInstance().invScaleX(w));

    if(w < underWidthMax)
      underWidth = w;
    else
      underWidth = underWidthMax;

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

    if(w < minWidth)
      w = minWidth;
    
    super.setWidth(w);
    super.setHeight(h);

    underWidthMax = getFontMetrics().stringWidth("xx");

    if(w < underWidthMax)
      underWidth = w;
    else
      underWidth = underWidthMax;

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

    g.setColor(getTextBackground());
    
    if(itsSketchPad.isLocked())
      g.fillRect( x + 1, y + 1, w - 2, h - 2);
    else
      g.fill3DRect( x, y + 1, w, h - 1, true);     
    
    g.setColor( Color.black);
    g.drawLine( x, y, x + underWidth - 1, y);
    g.drawLine( x, y, x, y + h/4);
    
    paintInlets(g);
    
    g.setFont(getFont());      
    g.drawString(display, x + 2, y + h - getFontMetrics().getDescent() - 2);
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
    ObjectPopUp.addSeparation();
    TextPopUpMenu.update(this);
    ObjectPopUp.addMenu(TextPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(TextPopUpMenu.getInstance());
    ObjectPopUp.removeSeparation();
  }
}
