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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
import javax.swing.JSeparator;
import javax.swing.SwingUtilities;

import java.util.*;

import java.awt.image.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;

/**
 * The "number box" graphic object base class
 */

abstract public class NumberBox extends Editable implements KeyEventClient {
  private int nDecimals = 0;
  private String filter;
  public static final int DEFAULT_WIDTH = 40;
  public static final int DEFAULT_HEIGHT = 15;
  private static final int DEFAULT_VISIBLE_DIGIT = 3;
  private static Color floatColor = new Color(239, 239, 239); 

  public final static int NOEDIT_MODE = 0;
  public final static int DRAG_MODE   = 1;
  public final static int TEXT_MODE   = 2;
  public final static int INCR_MODE   = 3;
  private int editMode = NOEDIT_MODE;

  private BufferedImage buff;
  private Graphics2D buffG;

  public NumberBox(FtsGraphicObject theFtsObject, String filter) 
  {
    super(theFtsObject);

    this.filter = filter;

    updateOffScreenBuffer();
  }

  void updateOffScreenBuffer()
  {
    int h = getHeight() - 2;
    if( h <= 0) h = DEFAULT_HEIGHT - 2;
    
    int w = getWidth() - h/2 - 3;
    if( w <= 0) w = DEFAULT_WIDTH - h/2 - 3;

    buff = new BufferedImage( w, h, BufferedImage.TYPE_INT_RGB);
    buffG = buff.createGraphics();
    buffG.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
  }

  public void setDefaults()
  {
    super.setWidth( getMinWidth());
    super.setHeight( getMinHeight());
  }

  public String getArgs()
  {
    return getValueAsText();
  }

  private int getMinWidth()
  {
    return getFontMetrics().stringWidth("0")*DEFAULT_VISIBLE_DIGIT + getFontMetrics().stringWidth("..") +17;
  }

  public int getDefaultWidth()
  {
    return getMinWidth();
  }

  private int getFullTextWidth(){
    return (getTextWidthOffset() + getFontMetrics().stringWidth(getValueAsText()) + 2);
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

    updateOffScreenBuffer();
  }

  // redefined from base class
  // This way, changing the height from outside is forbidden
  public void setHeight( int theHeight)
  {
    updateOffScreenBuffer();
  }

  // redefined from base class
  public void setFont( Font theFont)
  {
    super.setFont( theFont);

    int minWidth = getMinWidth();
    if (getWidth() < minWidth)
      super.setWidth( minWidth);

    super.setHeight(getMinHeight());

    updateOffScreenBuffer();
  }

  public void setCurrentFont( Font theFont)
  {
    super.setCurrentFont( theFont);
    updateOffScreenBuffer();
  }

  public void fitToText()
  {
    int full = getFullTextWidth();
    int min = getMinWidth();

    if(full < min) super.setWidth(min);
    else  super.setWidth(full);

    super.setHeight(getMinHeight());

    updateOffScreenBuffer();
  }

  public void redefine( String text) 
  {
    setValueAsText( text);
    setEditMode( NumberBox.NOEDIT_MODE);
  }

  public void redefined()
  {    
    SwingUtilities.invokeLater(new Runnable(){
	public void run()
	{
	  fitToText();
	  redraw();
	}
      });
  }
  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  
  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 3;

  public int getTextXOffset()
  {
    return getTextHeightOffset() + getTriangleWidth();
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return  getTextHeightOffset() + getTriangleWidth() +4;
  }

  public int getTextHeightOffset()
  {
    return 5;
  }
    
  public Color getTextForeground()
  {
    return Color.black;
  }

  public Color getTextBackground()
  {
    return Color.white;
  }

  public boolean isMultiline()
  {
    return false;
  }

  // ----------------------------------------
  // ValueAsText property
  // ----------------------------------------
  abstract public void setValueAsText( String value);
  abstract public String getValueAsText();

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
    String aString;

    // Background
    if ( !isSelected())
      g.setColor( Color.white);
    else
      g.setColor( Settings.sharedInstance().getUIColor().darker());

    g.fillRect( x+1, y+1, w-2 , h-2);

    // Float area
    if(( editMode != TEXT_MODE) && !isSelected() && (getIntZoneWidth() > 0) )
      {
	g.setColor( floatColor);
	g.fillRect( x+getIntZoneWidth()+1, y+1, w-getIntZoneWidth()-2, h-2);
      }

    // Triangle
    g.setColor( Color.black);
    if( editMode == TEXT_MODE || editMode == INCR_MODE)
      {
	int xPoints[] = { xp1, xp1 + hd2, xp1};
	int yPoints[] = { y, y + hd2, y + hm1};
	g.fillPolygon( xPoints, yPoints, 3);
      }
    else
      {
	g.drawLine( xp1, y, xp1 + hd2, y + hd2);
	g.drawLine( xp1 + hd2, y + hd2, xp1, y + hm1);
      }
    
    aString = getVisibleString(getValueAsText());

    if( editMode == INCR_MODE)
      g.setColor( Color.gray);
    else
      g.setColor( Color.black);

    g.setFont( getFont());
    g.drawString( aString, 
		  x + hd2 + 5, 
		  y + getFontMetrics().getAscent() + (h - getFontMetrics().getHeight())/2 + 1);
    
    super.paint( g);
  }

  public void updatePaint(Graphics g) 
  {
    g.drawImage( buff, getX() + getHeight()/2 + 2, getY() + 1, itsSketchPad);  
  }

  public void drawContent( Graphics g, int x, int y, int w, int h) 
  {
    int hd2 = h / 2;
    String aString;
  
    if( editMode != TEXT_MODE)
      {
	// Fill the background
	g.setColor( Color.white);
      
	if(getIntZoneWidth() > 0)
	  {
	    int iw = getIntZoneWidth()-(hd2+2);
	    g.fillRect( 0, 0, iw, h);
	    g.setColor( floatColor);
	    g.fillRect( iw, 0, w - iw, h);
	  }
	else
	  g.fillRect( 0, 0, w, h);
      
	// Get the value
	aString = getVisibleString( getValueAsText());
      }
    else
      {
	g.setColor( Color.white);
	g.fillRect( 0, 0, w, h);
      
	aString = getVisibleString( getValueAsText());
      }
      
    // Draw the value
    if( editMode == INCR_MODE)
      g.setColor( Color.gray);
    else
      g.setColor( Color.black);
    g.setFont( getFont());
    g.drawString( aString, 3, getFontMetrics().getAscent() + (h + 2 - getFontMetrics().getHeight())/2);
  }
  
  public void updateRedraw()
  {
    int h = getHeight();
    drawContent( buffG, 0, 0, getWidth()- h/2 - 3, h - 2);
    super.updateRedraw();
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

  public int getTriangleWidth()
  {
    return getHeight()/2;
  }

  abstract public int getIntZoneWidth();

  public void keyPressed( KeyEvent e) 
  {
    switch (e.getKeyCode())
      {
      case KeyEvent.VK_UP:
	if( getEditMode() == INCR_MODE)
	  increment( true, e.isShiftDown());
	break;
      case KeyEvent.VK_DOWN:
	if( getEditMode() == INCR_MODE)
	  increment( false, e.isShiftDown());
	break;
      default:
	break;
      }
    e.consume();
    redraw();
  }

  abstract public void increment( boolean up, boolean shift);

  public void keyReleased( KeyEvent e){}
  public void keyTyped( KeyEvent e){}
  public void keyInputGained() 
  {
    redraw();
  }
  public void keyInputLost() 
  {
    setEditMode( NOEDIT_MODE);
    redraw();
  }

  public void setEditMode( int mode)
  {
    editMode = mode;
    redraw();
  }
  public int getEditMode()
  {
    return editMode;
  }
  
  public void setEditing(boolean v)
  {
    super.setEditing(v);
    if( v) setEditMode( TEXT_MODE);
    else setEditMode( NOEDIT_MODE);
  }

  public boolean isOnArrow( Point p)
  {
    int x = getX();
    int y = getY();
    int h = getHeight();
    int xPoints[] = { x, x + h/2, x};
    int yPoints[] = { y, y + h/2, y + h};
    return (new  Polygon( xPoints, yPoints, 3)).contains( p);
  }
}






