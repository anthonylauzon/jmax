package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

/**
 * The "integer box" graphic object.
 */

class ErmesObjInt extends ErmesObject implements FtsPropertyHandler, KeyEventClient {
  int state;
  StringBuffer currentText;
  int itsInteger = 0;

  int DEFAULT_WIDTH = 40;
  int DEFAULT_HEIGHT = 15;
  int DEFAULT_VISIBLE_DIGIT = 3;
  int itsStartingY, itsFirstY;
  boolean firstClick = true;

  public ErmesObjInt( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    state = 0;
    currentText = new StringBuffer();

    itsFtsObject.watch("value", this);

    itsInteger = ((Integer)itsFtsObject.get("value")).intValue();

    int w = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..") +17;
    if (getWidth() < w) 
      setWidth( w);

    int h = itsFontMetrics.getHeight() + 4;
    if ( getHeight() < h)
      setHeight( h);

    recomputeInOutletsPositions();
  }

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    int temp = ((Integer) value).intValue();

    if (itsInteger != temp) 
      {
	itsInteger = temp;

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific(g);
	g.dispose();
      }
  }

  public void FromDialogValueChanged( int theInt) 
  {
    itsInteger = theInt;

    itsFtsObject.put("value", theInt);
    Repaint();
  }

  void ResizeToNewFont(Font theFont) 
  {
    resizeBy( 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..") - getWidth(),
	      itsFontMetrics.getHeight() + 4 -getHeight());
  }

  public void ResizeToText(int theDeltaX, int theDeltaY) 
  {
    int aWidth = getWidth()+theDeltaX;
    int aHeight = getHeight()+theDeltaY;

    if ( (aWidth < aHeight/2 + 17 + itsFontMetrics.stringWidth("0"))
	 && (aHeight < itsFontMetrics.getHeight() + 4)) 
      {
	aWidth = getMinimumSize().width;
	aHeight = getMinimumSize().height;
      } else 
	{
	  if ( aWidth < aHeight/2 + 17 + itsFontMetrics.stringWidth("0"))
	    aWidth = aHeight/2 + 17 + itsFontMetrics.stringWidth("0");

	  if (aHeight < itsFontMetrics.getHeight() + 4)
	    aHeight = itsFontMetrics.getHeight() + 4;
        }
    resizeBy( aWidth - getWidth(), aHeight - getHeight());
  }

  public boolean canResizeBy( int theDeltaX, int theDeltaY) 
  {
    if ( (getWidth() + theDeltaX < getHeight()/2 + 17 + itsFontMetrics.stringWidth("0"))
	 || ( getHeight() + theDeltaY < itsFontMetrics.getHeight() + 4))
      return false;
    else
      return true;
  }

  public void RestoreDimensions() 
  {
    int aHeight, aWidth;

    aHeight = itsFontMetrics.getHeight() + 4;
    aWidth = 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..");

    resizeBy( aWidth - getWidth(), aHeight - getHeight());
  }

  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	if (!evt.isControlDown())
	  {
	    state = 1;
	    itsSketchPad.itsSketchWindow.setKeyEventClient( this);
	  }

	itsFirstY = y;

	if (firstClick) 
	  {
	    itsStartingY = itsInteger;
	    firstClick = false;
	  } 
	else
	  itsStartingY = itsInteger;

	itsFtsObject.put( "value", new Integer(itsInteger));

      } 
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  public void inspect() 
  {
    new ErmesObjIntegerDialog(itsSketchPad.GetSketchWindow(), String.valueOf(itsInteger), this);
  }

  void MouseUp(MouseEvent evt,int x, int y) 
  {
    if (itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	Fts.sync();
	Repaint();
      }
    else
      super.MouseUp(evt, x, y);
  }

  public boolean MouseDrag_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown() ) 
      {
	if (!evt.isControlDown())
	  state = 2;

	itsInteger = itsStartingY + (itsFirstY - y);
	itsFtsObject.put( "value", new Integer(itsInteger));
	Repaint();

	return true;
      } 
    else
      return false;
  }

  public boolean NeedPropertyHandler() 
  {
    return true;
  }

  public boolean isUIController() 
  {
    return true;
  }

  public void Paint_specific( Graphics g) 
  {
    int x = getX();
    int xp1 = x + 1;
    int y = getY();
    int h = getHeight();
    int hd2 = h / 2;
    int hm1 = h - 1;
    int w = getWidth();
    int wm1 = w - 1;

    if (g == null)
      return;

    // (1) Fill the background
    if ( !itsSelected)
      g.setColor( Color.white);
    else
      g.setColor( itsUISelectedColor);
    g.fillRect( x, y, wm1 , hm1);

    // (2) Draw the outline
    g.setColor( Color.black);
    g.drawRect( x, y, wm1, hm1);

    // (3) Draw or fill the triangle
    if ( state != 0 ) {
      int xPoints[] = {
	xp1, xp1 + hd2, xp1};
      int yPoints[] = {
	y, y + hd2, y + hm1};
      g.fillPolygon( xPoints, yPoints, 3);
    } else {
      g.drawLine( xp1, y, xp1 + hd2, y + hd2);
      g.drawLine( xp1 + hd2, y + hd2, xp1, y + hm1);
    }

    // (4) Draw the value
    String aString;

    if (state != 3)
      aString = GetVisibleString(String.valueOf(itsInteger));

    else {
      aString = currentText.toString();
      g.setColor( Color.red);
    }

    g.setFont( getFont());
    g.drawString( aString, x + hd2 + 5,
		  y + itsFontMetrics.getAscent()
		  + (h - itsFontMetrics.getHeight())/2 + 1);

    // (5) Draw the dragbox (?)
    if ( !itsSketchPad.itsRunMode)
      g.fillRect( x + w - DRAG_DIMENSION, y + h - DRAG_DIMENSION,
		  DRAG_DIMENSION, DRAG_DIMENSION);
  }

  String GetVisibleString(String theString) 
  {
    String aString = theString;
    String aString2 = "..";

    int aStringLength = theString.length();

    int aCurrentSpace = getWidth() - (getHeight()/2+5) - 5;
    int aStringWidth = itsFontMetrics.stringWidth( aString);

    if ( aStringWidth < aCurrentSpace)
      return aString;

    while ( ( aCurrentSpace <= aStringWidth) && (aString.length() > 0) )
      {
	aString = aString.substring( 0, aString.length()-1);
	aStringWidth = itsFontMetrics.stringWidth( aString);
      }

    if ( ( aStringWidth + itsFontMetrics.stringWidth("..") >= aCurrentSpace)
	 && ( aString.length() > 0) )
      aString = aString.substring( 0, aString.length() - 1);

    aString = aString + aString2;
    return aString;
  }

  Dimension minimumSize = new Dimension( DEFAULT_WIDTH, DEFAULT_HEIGHT);

  public Dimension getMinimumSize() 
  {
    if (getHeight()==0 || getWidth() == 0)
      return minimumSize;
    else
      return new Dimension( getHeight()/2 + 13 + itsFontMetrics.stringWidth("0"), itsFontMetrics.getHeight()+4);
  }

  public void keyPressed( KeyEvent e) 
  {

    if (e.getKeyCode() == 17)
      return; //avoid CTRL as valid (independent) key

    state = 3;

    if ( !e.isControlDown() && !e.isMetaDown() && !e.isShiftDown()) 
      {
	int c = e.getKeyChar();

	if ( c >= '0' && c <= '9')
	  currentText.append( (char)c);
	else if ( c == ircam.jmax.utils.Platform.ENTER_KEY||
		  c == ircam.jmax.utils.Platform.RETURN_KEY ) 
	  {
	    try 
	      {
		int value = Integer.parseInt( currentText.toString());

		itsInteger = value;
		itsFtsObject.put( "value", new Integer( value));
	      } 
	    catch ( NumberFormatException exception) 
	      {
	      }
	    currentText.setLength(0);
	    state = 0;
	  } 
	else if ( ( c == ircam.jmax.utils.Platform.DELETE_KEY)||
		  ( c == ircam.jmax.utils.Platform.BACKSPACE_KEY) ) 
	  {
	    int l = currentText.length();

	    l = ( l > 0 ) ? l - 1 : 0;

	    currentText.setLength( l);
	  }

	Repaint();
      }
  }

  public void keyReleased( KeyEvent e) 
  {
  }

  public void keyTyped( KeyEvent e) 
  {
  }

  public void keyInputGained() 
  {
    Repaint();
  }

  public void keyInputLost() 
  {
    state = 0;
    currentText.setLength(0);
    Repaint();
  }
}
