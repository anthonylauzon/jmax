package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;


//
// The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal, ErmesObjPatcher).
//

abstract public class ErmesObjEditableObject extends ErmesObject implements FtsInletsListener, FtsOutletsListener
{
  protected MultiLineText itsText = new MultiLineText();

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    itsText.setFontMetrics( getFontMetrics());
    itsText.setText( getArgs());

    if (getWidth() == -1)
      setWidth( getFontMetrics().stringWidth( "pack 1 2 3") + 2*getTextXOffset());
    else
      updateDimensions();
  }

  private void updateDimensions()
  {
    itsText.setWidth( getWidth() - 2*getTextXOffset());
    super.setHeight( getFontMetrics().getHeight()*itsText.getRows() + 2*getTextYOffset());
  }

  // redefined from base class
  public  void setWidth( int theWidth) 
  {
    int minWidth = getFontMetrics().stringWidth( "m ") + 2*getTextXOffset();

    if (theWidth < minWidth)
      theWidth = minWidth;

    super.setWidth( theWidth);

    updateDimensions();
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

    if ( itsText != null)
      {
	itsText.setFontMetrics( getFontMetrics());

	super.setHeight( getFontMetrics().getHeight()*itsText.getRows() + 2*getTextYOffset());
      }
  }


  // (em) set the text AND adjust the dimension of the object accordingly
  protected void setText( String text)
  {
    itsText.setText(text);
    updateDimensions();
  }

  // get the number of rows in the multiline text;
  // needed in editing

  public int getRows()
  {
    return itsText.getRows();
  }

  public void dispose()
  {
    itsText = null;
    super.dispose();
  }

  // ----------------------------------------
  // White and text area offset
  // for now, this base class provides default value.
  // These methods should be abstracts.
  // ----------------------------------------

  public int getWhiteXOffset()
  {
    return 0;
  }

  public int getWhiteYOffset()
  {
    return 0;
  }

  protected int getTextXOffset()
  {
    return 0;
  }

  protected int getTextYOffset()
  {
    return 0;
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  abstract public String getArgs();

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  public Color getTextBackground()
  {
    return Color.white;
  }

  // ----------------------------------------
  // Property handling
  // ----------------------------------------

  public void inletsChanged(int n)
  {
    redraw();
    redrawConnections();
  }

  public void outletsChanged(int n)
  {
    redraw();
    redrawConnections();
  }
    
  protected void DrawParsedString( Graphics theGraphics) 
  {
    int x = getX() + getTextXOffset();
    int y = getY() + getTextYOffset() + getFontMetrics().getAscent();
    int height = getFontMetrics().getHeight();

    for ( Enumeration e = itsText.elements(); e.hasMoreElements(); ) 
      {
	theGraphics.drawString( (String)e.nextElement(), x, y);
	y += height;
      }
  }
}

