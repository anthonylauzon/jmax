//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal, ErmesObjPatcher).
//

abstract class ErmesObjEditableObject extends ErmesObject implements FtsInletsListener, FtsOutletsListener, ErmesObjEditable
{
//   boolean itsInEdit = true;

  protected MultiLineText itsText = new MultiLineText();

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

//     itsInEdit = false;

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

  // redefined from base class
  void setFont( Font theFont)
  {
    super.setFont( theFont);

    if ( itsText != null)
      {
	itsText.setFontMetrics( itsFontMetrics);

	super.setHeight( itsFontMetrics.getHeight()*itsText.getRows() + 2*getTextYOffset());
      }
  }


  // (em) set the text AND adjust the dimension of the object accordingly
  protected void setText( String text)
  {
    itsText.setText(text);
    updateDimensions();
  }


  void dispose()
  {
    itsText = null;
    super.dispose();
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

  public void inletsChanged(int n)
  {
    // (***fd) Should do something ??

    itsSketchPad.repaint();
  }

  public void outletsChanged(int n)
  {
    // (***fd) Should do something ??

    itsSketchPad.repaint();
  }
    
  void MouseDown_specific( MouseEvent evt,int x, int y) 
  {
    itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void startEditing()  // (fd) public, because public in ErmesEditable...
  {
    ErmesObjEditField editField = itsSketchPad.GetEditField();

    if (editField != null)
      editField.setEditable( true);

    editField.setFont( getFont());
    editField.setText( getArgs());
    editField.setOwner( this);

    editField.setBounds( getX()-3, 
			 getY()-3, 
			 getWidth() + 6,
			 itsFontMetrics.getHeight() * (itsText.getRows()+1) + 6);

    editField.setVisible( true);
    editField.requestFocus();
    editField.setCaretPosition( getArgs().length());
  }

//   void MouseUp( MouseEvent evt,int x,int y) 
//   {
//     if (itsInEdit)
//       return;

//     super.MouseUp(evt, x, y);
//   }

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
}

