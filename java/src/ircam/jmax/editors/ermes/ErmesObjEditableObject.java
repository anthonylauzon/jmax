package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal, ErmesObjPatcher).
//
abstract class ErmesObjEditableObject extends ErmesObject implements FtsPropertyHandler, ErmesObjEditable {

//   boolean itsInEdit = true;

  protected MultiLineText itsText = new MultiLineText();

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

//     itsInEdit = false;

    itsFtsObject.watch( "ins", this);
    itsFtsObject.watch( "outs", this);

    itsText.setFontMetrics( itsFontMetrics);
    itsText.setText( getArgs());

    if (getWidth() == -1)
      setWidth( itsFontMetrics.stringWidth( "pack 1 2 3") + 2*getTextXOffset());
    else
      updateDimensions();
  }

  private void updateDimensions()
  {
    itsText.setWidth( getWidth() - 2*getTextXOffset());
    super.setHeight( itsFontMetrics.getHeight()*itsText.getRows() + 2*getTextYOffset());

    recomputeInOutletsPositions();
  }

  void cleanAll()
  {
    itsText = null;
    super.cleanAll();
  }

  // redefined from base class
  protected void setWidth( int theWidth) 
  {
    int minWidth = itsFontMetrics.stringWidth( "m ") + 2*getTextXOffset();

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
  void resizeBy( int theDeltaW, int theDeltaH) 
  {
    setWidth( getWidth() + theDeltaW);
  }


  // ----------------------------------------
  // White and text area offset
  // for now, this base class provides default value.
  // These methods should be abstracts.
  // ----------------------------------------

  protected int getWhiteXOffset()
  {
    return 0;
  }

  protected int getWhiteYOffset()
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
  abstract String getArgs();


  // ----------------------------------------
  // Property handling
  // ----------------------------------------
  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    if ((name == "ins") || (name == "outs"))
      updateInOutlets();
  
    itsSketchPad.repaint();
  }
    
  void MouseDown_specific( MouseEvent evt,int x, int y) 
  {
    itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  // actually starts the edit operation.
  // The inset parameter specify the relative position of the editfield
  private void doEdit( int editFieldInset) 
  {
    if (itsSketchPad.GetEditField() != null)
      itsSketchPad.GetEditField().setEditable( true);

    itsSketchPad.GetEditField().setFont( getFont());
    itsSketchPad.GetEditField().setText( getArgs());
    itsSketchPad.GetEditField().itsOwner = this;

    if ( itsText.getRows() == 0)
      itsSketchPad.GetEditField().setBounds( getX() + editFieldInset/2,
					     getY() + 1,
					     getWidth() - editFieldInset,
					     itsFontMetrics.getHeight() + 20);
    else
      itsSketchPad.GetEditField().setBounds( getX() + editFieldInset/2,
					     getY() + 1,
					     getWidth() - editFieldInset,
					     itsFontMetrics.getHeight() * (itsText.getRows()+1));

    itsSketchPad.GetEditField().setVisible( true);
    itsSketchPad.GetEditField().requestFocus();
    itsSketchPad.GetEditField().setCaretPosition( getArgs().length());
  }

  public void restartEditing()  // (fd) public, because public in ErmesEditable...
  {
    doEdit( 0);
  }

  public void startEditing()  // (fd) public, because public in ErmesEditable...
  {
    doEdit( 6);
  }
    
//   void MouseUp( MouseEvent evt,int x,int y) 
//   {
//     if (itsInEdit)
//       return;

//     super.MouseUp(evt, x, y);
//   }

  void RestoreDimensions( boolean paintNow) 
  {
    int aMaxWidth = MaxWidth( itsFontMetrics.stringWidth( "ZOB") + 2*getTextXOffset(),
			      itsInletList.size()*12,
			      itsOutletList.size()*12);

    int aHeightDiff = itsFontMetrics.getHeight() * itsText.getRows() + 2*getTextYOffset() - getHeight();
    int aWidthDiff = aMaxWidth - getWidth();

    if (aHeightDiff == 0 && aWidthDiff == 0)
      return;

    resizeBy( aWidthDiff, aHeightDiff);

    if (paintNow) 
      {
	if (aHeightDiff < 0 || aWidthDiff < 0)
	  itsSketchPad.repaint();
	else
	  DoublePaint();
      }
    else 
	{
	  if (aHeightDiff < 0 || aWidthDiff <0)
	    itsSketchPad.markSketchAsDirty();
	  else
	    itsSketchPad.addToDirtyObjects( this);
        }
  }

  int MaxWidth( int uno, int due, int tre) 
  {
    int MaxInt = uno;

    if (due > MaxInt)
      MaxInt = due;

    if (tre > MaxInt)
      MaxInt = tre;

    return MaxInt;
  }

  void ResizeToNewFont(Font theFont) 
  {
    ResizeToText( 0,0);
  }

  void ResizeToText( int theDeltaX, int theDeltaY) 
  {
    int aWidth = getWidth() + theDeltaX;
    int aHeight = getHeight() + theDeltaY;

    int minWidth = itsFontMetrics.stringWidth( "m ") + 2*getTextXOffset();
    if ( aWidth < minWidth)
      aWidth = minWidth;

    int minHeight = itsFontMetrics.getHeight() * itsText.getRows() + 2*getWhiteYOffset();
    if ( aHeight < minHeight)
      aHeight = minHeight;

    resizeBy( aWidth - getWidth(), aHeight - getHeight());
  }

  boolean canResizeBy( int theDeltaX, int theDeltaY) 
  {
    return getWidth() + theDeltaX >= itsFontMetrics.stringWidth( "m ");
  }

  void MoveOutlets() 
  {
    for ( Enumeration e = itsOutletList.elements(); e.hasMoreElements(); )
      {
	ErmesObjOutlet aOutlet = (ErmesObjOutlet) e.nextElement();
	aOutlet.MoveTo( aOutlet.itsX, getY() + getHeight());
      }
  }

  abstract protected void Paint_specific( Graphics g);

  protected void DrawParsedString( Graphics theGraphics) 
  {
    int x = getX() + getTextXOffset();
    int y = getY() + getTextYOffset() + itsFontMetrics.getAscent();
    int height = itsFontMetrics.getHeight();

    for ( Enumeration e = itsText.elements(); e.hasMoreElements(); ) 
      {
	theGraphics.drawString( (String)e.nextElement(), x, y);
	y += height;
      }
  }

  // ----------------------------------------
  // old stuff
  // ----------------------------------------
  Dimension getMinimumSize()
  {
    new Throwable( this.getClass().getName()).printStackTrace();
    return new Dimension( 100, 17);
  }
}

