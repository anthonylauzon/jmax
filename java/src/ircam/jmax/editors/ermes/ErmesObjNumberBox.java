package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

/**
 * The "number box" graphic object base class
 */

abstract class ErmesObjNumberBox extends ErmesObject implements FtsPropertyHandler, KeyEventClient {

  protected int state;
  private StringBuffer currentText;

  private static final int DEFAULT_WIDTH = 40;
  private static final int DEFAULT_HEIGHT = 15;
  private static final int DEFAULT_VISIBLE_DIGIT = 3;

  public ErmesObjNumberBox( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    state = 0;
    currentText = new StringBuffer();

    itsFtsObject.watch( "value", this);

    int minWidth = getMinWidth();
    if (getWidth() < minWidth)
      super.setWidth( minWidth);

    int minHeight = getMinHeight();
    if ( getHeight() < minHeight)
      super.setHeight( minHeight);
  }

  private int getMinWidth()
  {
    return itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..") +17;
  }

  private int getMinHeight()
  {
    return itsFontMetrics.getHeight() + 4;
  }

  // redefined from base class
  protected void setWidth( int theWidth) 
  {
    int minWidth = getMinWidth();
    if (theWidth < minWidth)
      theWidth = minWidth;

    super.setWidth( theWidth);
  }

  // redefined from base class
  // This way, changing the height from outside is forbidden
  protected void setHeight( int theHeight)
  {
  }

  // redefined from base class
  void resizeBy( int theDeltaW, int theDeltaH) 
  {
    setWidth( getWidth() + theDeltaW);
  }

  // redefined from base class
  void setFont( Font theFont)
  {
    super.setFont( theFont);

    super.setWidth( getMinWidth());
    super.setHeight( getMinHeight());
  }

  // ----------------------------------------
  // ValueAsText property
  // ----------------------------------------
  abstract void setValueAsText( String value);
  abstract String getValueAsText();

  public void inspect() 
  {
    new ErmesNumberDialog( itsSketchPad.GetSketchWindow(), this);
  }

  boolean isUIController() 
  {
    return true;
  }

  public void Paint_specific( Graphics g) 
  {
    if (g == null)
      return;

    int x = getX();
    int xp1 = x + 1;
    int y = getY();
    int w = getWidth();
    int wm1 = w - 1;
    int h = getHeight();
    int hd2 = h / 2;
    int hm1 = h - 1;

    // Fill the background
    if ( !itsSelected)
      //g.setColor( Settings.sharedInstance().getUIColor());
      g.setColor( Color.white);
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    //g.fill3DRect( x+1, y+1, w-2 , h-2, true);
    g.fillRect( x+1, y+1, w-2 , h-2);

    g.setColor( Color.black);
    // Draw or fill the triangle
    if ( state != 0 ) 
      {
	int xPoints[] = { xp1, xp1 + hd2, xp1};
	int yPoints[] = { y, y + hd2, y + hm1};
	g.fillPolygon( xPoints, yPoints, 3);
      } 
    else 
      {
	//g.setColor( Settings.sharedInstance().getUIColor().brighter());
	g.drawLine( xp1, y, xp1 + hd2, y + hd2);
	g.drawLine( xp1 + hd2, y + hd2, xp1, y + hm1);
      }

    // Draw the value
    String aString;

    if (state != 3)
      aString = GetVisibleString( getValueAsText());
    else 
      {
	aString = currentText.toString();
	g.setColor( Color.red);
      }

    g.setColor( Color.black);
    g.setFont( getFont());
    g.drawString( aString, 
		  x + hd2 + 5, 
		  y + itsFontMetrics.getAscent() + (h - itsFontMetrics.getHeight())/2 + 1);

    super.Paint_specific( g);
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

  public void keyPressed( KeyEvent e) 
  {

    if (e.getKeyCode() == 17)
      return; //avoid CTRL as valid (independent) key

    state = 3;

    if ( !e.isControlDown() && !e.isMetaDown() && !e.isShiftDown()) 
      {
	int c = e.getKeyChar();

	if ( c == ircam.jmax.utils.Platform.ENTER_KEY||
		  c == ircam.jmax.utils.Platform.RETURN_KEY ) 
	  {
	    setValueAsText( currentText.toString());

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
	else
	  currentText.append( (char)c);

	DoublePaint();
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
    DoublePaint();
  }

  public void keyInputLost() 
  {
    state = 0;
    currentText.setLength(0);
    DoublePaint();
  }
}
