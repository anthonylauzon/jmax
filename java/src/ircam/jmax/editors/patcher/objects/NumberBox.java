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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;

/**
 * The "number box" graphic object base class
 */

abstract public class NumberBox extends GraphicObject implements KeyEventClient {
  boolean valueValid = true;
  private StringBuffer currentText;
  private int nDecimals = 0;
  private String filter;
  public static final int DEFAULT_WIDTH = 40;
  public static final int DEFAULT_HEIGHT = 15;
  private static final int DEFAULT_VISIBLE_DIGIT = 3;

  public NumberBox(FtsGraphicObject theFtsObject, String filter) 
  {
    super(theFtsObject);

    currentText = new StringBuffer();

    this.filter = filter;
  }

  public void setDefaults()
  {
    int minWidth = getMinWidth();
    if (getWidth() < minWidth)
      super.setWidth( minWidth);

    int minHeight = getMinHeight();
    if ( getHeight() < minHeight)
      super.setHeight( minHeight);
  }

  private int getMinWidth()
  {
    return getFontMetrics().stringWidth("0")*DEFAULT_VISIBLE_DIGIT + getFontMetrics().stringWidth("..") +17;
  }

  private int getFullTextWidth(){
    return (getFontMetrics().stringWidth("..")+ getFontMetrics().stringWidth(getValueAsText())+17);
  }

  private int getMinHeight()
  {
    return getFontMetrics().getHeight() + 4;
  }

  // redefined from base class
  public void setWidth( int theWidth) 
  {
    int minWidth = getMinWidth();
    if (theWidth < minWidth)
      theWidth = minWidth;

    super.setWidth( theWidth);
  }

  // redefined from base class
  // This way, changing the height from outside is forbidden
  public void setHeight( int theHeight)
  {
  }

  // redefined from base class
  public void setFont( Font theFont)
  {
    super.setFont( theFont);

    int minWidth = getMinWidth();
    if (getWidth() < minWidth)
      super.setWidth( minWidth);

    super.setHeight(getMinHeight());
  }

  public void fitToText()
  {
    int full = getFullTextWidth();
    int min = getMinWidth();

    if(full<min) super.setWidth(min);
    else  super.setWidth(full);

    super.setHeight(getMinHeight());
  }

  public void redefined()
  {
      fitToText();
  }

  // ----------------------------------------
  // ValueAsText property
  // ----------------------------------------
  abstract public void setValueAsText( String value);
  abstract public String getValueAsText();

  public boolean isValueValid()
  {
      return valueValid;
  }
  public void setValueValid(boolean valid)
  {
      valueValid = valid;
  }

  public void inspect() 
  {
  }


  public void paint( Graphics g) 
  {
    int x = getX();
    int xp1 = x + 1;
    int y = getY();
    int w = getWidth();
    int wm1 = w - 1;
    int h = getHeight();
    int hd2 = h / 2;
    int hm1 = h - 1;

    // Fill the background
    if ( !isSelected())
      g.setColor( Color.white);
    else
      g.setColor( Settings.sharedInstance().getUIColor().darker());

    g.fillRect( x+1, y+1, w-2 , h-2);

    g.setColor( Color.black);

    // Draw or fill the triangle

    if ( valueValid) 
      {
	g.drawLine( xp1, y, xp1 + hd2, y + hd2);
	g.drawLine( xp1 + hd2, y + hd2, xp1, y + hm1);
      }
    else 
      {
	int xPoints[] = { xp1, xp1 + hd2, xp1};
	int yPoints[] = { y, y + hd2, y + hm1};
	g.fillPolygon( xPoints, yPoints, 3);
      } 

    // Draw the value
    String aString;

    if(valueValid)
      aString = getVisibleString(getValueAsText());
    else 
      aString = getVisibleString(currentText.toString());
    
    g.setColor( Color.black);

    g.setFont( getFont());
    g.drawString( aString, 
		  x + hd2 + 5, 
		  y + getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2 + 1);

    super.paint( g);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int hd2 = h / 2;

    // Fill the background
    g.setColor( Color.white);
    g.fillRect( x+hd2+2, y+1, w-(hd2+2)-2, h-2);

    // Draw the value
    String aString;

    if(valueValid)
      aString = getVisibleString(getValueAsText());
    else 
      aString = getVisibleString(currentText.toString());
    
    g.setColor( Color.black);
    g.setFont( getFont());
    g.drawString( aString, 
		  x + hd2 + 5, 
		  y + getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2 + 1);
  }


  String getVisibleString(String theString) 
  {
    String aString = theString;
    String aString2 = "..";

    int aStringLength = theString.length();

    int aCurrentSpace = getWidth() - (getHeight() / 2 + 5) - 5;
    int aStringWidth = getFontMetrics().stringWidth(aString);

    if (aStringWidth < aCurrentSpace)
      return aString;

    while(( aCurrentSpace <= aStringWidth) && (aString.length() > 0) )
      {
	aString = aString.substring( 0, aString.length()-1);
	aStringWidth = getFontMetrics().stringWidth( aString);
      }

    if ( ( aStringWidth + getFontMetrics().stringWidth("..") >= aCurrentSpace)
	 && ( aString.length() > 0) )
      aString = aString.substring( 0, aString.length() - 1);

    aString = aString + aString2;
    return aString;
  }

  public void keyPressed( KeyEvent e) 
  {
    if ( !e.isControlDown() && !e.isMetaDown() && !e.isShiftDown()) 
      {
	if(e.getKeyCode()==109) return;//??????
	// This stuff should be thrown away, and we should use
	// a real text area for the number boxes !!!

	switch (e.getKeyCode())
	  {
	  case KeyEvent.VK_ENTER:
	    setValueAsText( currentText.toString());
	    currentText.setLength(0);
	    valueValid = true;
	    itsSketchPad.setKeyEventClient(null);
	    break;
	  case KeyEvent.VK_DELETE:
	  case KeyEvent.VK_BACK_SPACE:
	    {
	      int l = currentText.length();
	      l = ( l > 0 ) ? l - 1 : 0;
	      currentText.setLength( l);
	    }
	  break;
	  case KeyEvent.VK_NUMPAD0:
	    currentText.append("0");
	    break;
	  case KeyEvent.VK_NUMPAD1:
	    currentText.append("1");
	    break;
	  case KeyEvent.VK_NUMPAD2:
	    currentText.append("2");
	    break;
	  case KeyEvent.VK_NUMPAD3:
	    currentText.append("3");
	    break;
	  case KeyEvent.VK_NUMPAD4:
	    currentText.append("4");
	    break;
	  case KeyEvent.VK_NUMPAD5:
	    currentText.append("5");
	    break;
	  case KeyEvent.VK_NUMPAD6:
	    currentText.append("6");
	    break;
	  case KeyEvent.VK_NUMPAD7:
	    currentText.append("7");
	    break;
	  case KeyEvent.VK_NUMPAD8:
	    currentText.append("8");
	    break;
	  case KeyEvent.VK_NUMPAD9:
	    currentText.append("9");
	    break;
	  case KeyEvent.VK_DECIMAL:
	    currentText.append(".");
	    break;
	  default:
	    currentText.append((char) e.getKeyCode());
	    break;
	  }

	e.consume();
	redraw();
      }
  }

  public void keyReleased( KeyEvent e) 
  {
     if ( !e.isControlDown() && !e.isMetaDown() && !e.isShiftDown()) 
      {
	if(e.getKeyChar()== '-') 
	  currentText.append('-');

	e.consume();
	redraw();
      }
  }

  public void keyTyped( KeyEvent e){}

  public void keyInputGained() 
  {
    redraw();
  }

  public void keyInputLost() 
  {
    valueValid = true;
    currentText.setLength(0);
    redraw();
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






