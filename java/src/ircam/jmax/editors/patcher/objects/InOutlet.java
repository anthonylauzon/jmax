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

//
// The abstract base class graphic in/outlet contained in subpatchers
//

abstract public class InOutlet extends Editable implements FtsObjectErrorListener{

  private static final int DEFAULT_WIDTH = 64;
  private static final int MINIMUM_WIDTH = 30;

  InOutlet( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);

    int width = getWidth();

    if (width == -1)
	setWidth( DEFAULT_WIDTH);
    else if (width <= MINIMUM_WIDTH)
	setWidth( width);

    updateDimensions();
  }

  public String getArgs()
  {
     return ftsObject.getDescription();
  }

  // redefined from base class
  public void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    
    fitToText();
  }

  public void setFont( Font theFont)
  {
    super.setFont( theFont);
    setWidth(getWidth());
    fitToText();    
  }

  public void redefine( String text) 
  {
      try 
	  {
	      ftsObject = ftsObject.getFts().redefineFtsObject( ftsObject, ftsObject.getClassName() + " " + text);
	      
	      if (ftsObject.isError())
		  itsSketchPad.showMessage(ftsObject.getErrorDescription());
	  } 
      catch (FtsException e) 
	  {
	      System.err.println("Error in redefining object, action cancelled");
	  }

      redraw();
      setWidth(getWidth());
      super.redefine(text);      
  }

  public void errorChanged(boolean value) 
  {
    redraw();
  } 

  public Dimension getMinimumSize() 
  {
    return null;
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 2;

  public int getTextXOffset()
  {
    return getHeight()+TEXT_X_OFFSET;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return getHeight()+5;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    if(ftsObject.isError())
      {
	if(isSelected())
	  return Color.gray.darker();
	else
	  return Color.gray;
      }
    else
      return Color.black;
  }

  public Color getTextBackground()
  {
      if(isSelected())
	  return Settings.sharedInstance().getObjColor().darker();
      else
	  return Settings.sharedInstance().getObjColor();
  }

 public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int hLine = ((TextRenderer)renderer).getRHeight()+getTextHeightOffset();

    g.setColor(getTextBackground());

    g.fillRect( x + 1, y + 1, w - 2,  h - 2);
    g.fill3DRect( x + 1, y + 1, hLine - 2,  h-2, true);

    drawTriangle(g, x, y, w, hLine);
    drawContent( g);

    super.paint( g);
  }

  abstract void drawTriangle(Graphics g, int x, int y, int w, int h);
}
