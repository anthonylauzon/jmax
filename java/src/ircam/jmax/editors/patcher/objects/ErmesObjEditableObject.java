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

abstract public class ErmesObjEditableObject extends ErmesObject implements FtsInletsListener, FtsOutletsListener, ErmesObjEditable
{
//   boolean itsInEdit = true;

  protected MultiLineText itsText = new MultiLineText();

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

//     itsInEdit = false;

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
  protected void setWidth( int theWidth) 
  {
    int minWidth = getFontMetrics().stringWidth( "m ") + 2*getTextXOffset();

    if (theWidth < minWidth)
      theWidth = minWidth;

    super.setWidth( theWidth);

    updateDimensions();
  }

  // redefined from base class
  // This way, changing the height from outside is forbidden
  protected void setHeight( int theHeight)
  {
  }

  // redefined from base class

  public void resizeBy( int theDeltaW, int theDeltaH) 
  {
    setWidth( getWidth() + theDeltaW);
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
  // Property handling
  // ----------------------------------------

  public void inletsChanged(int n)
  {
    // (***fd) Should do something ??

    redraw();
    redrawConnections();
  }

  public void outletsChanged(int n)
  {
    // (***fd) Should do something ??

    redraw();
    redrawConnections();
  }
    
  // actually starts the edit operation.
  // The inset parameter specify the relative position of the editfield
  private void doEdit( int editFieldInset) 
  {
    if (itsSketchPad.getEditField() != null)
      itsSketchPad.getEditField().setEditable( true);

    itsSketchPad.getEditField().setFont( getFont());
    itsSketchPad.getEditField().setText( getArgs());
    itsSketchPad.getEditField().setOwner(this);

    if ( itsText.getRows() == 0)
      itsSketchPad.getEditField().setBounds( getX() + editFieldInset/2,
					     getY() + 1,
					     getWidth() - editFieldInset,
					     getFontMetrics().getHeight() + 20);
    else
      itsSketchPad.getEditField().setBounds( getX() + editFieldInset/2,
					     getY() + 1,
					     getWidth() - editFieldInset,
					     getFontMetrics().getHeight() * (itsText.getRows()+1));

    itsSketchPad.getEditField().setVisible( true);
    itsSketchPad.getEditField().requestFocus();
    itsSketchPad.getEditField().setCaretPosition( getArgs().length());
  }

  public void restartEditing()  // (fd) public, because public in ErmesEditable...
  {
    doEdit( 0); 
  }

  public void startEditing()  // (fd) public, because public in ErmesEditable...
  {
    doEdit( 6);
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

