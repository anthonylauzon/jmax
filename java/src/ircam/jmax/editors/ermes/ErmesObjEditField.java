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
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
//
public class ErmesObjEditField extends TextArea implements KeyListener, FocusListener {

  private ErmesObjEditableObject itsOwner = null;
  private ErmesSketchPad itsSketchPad = null;
  private boolean focused = false;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjEditField( ErmesSketchPad theSketchPad) 
  {
    super(" ", 1, 20, TextArea.SCROLLBARS_NONE);

    setBackground(Color.white);

    setFont( new Font( ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE));

    setEditable(true);

    selectAll();

    addKeyListener(this);
    addFocusListener(this);

    itsSketchPad = theSketchPad;
  }

  public void setOwner( ErmesObjEditableObject owner)
  {
    itsOwner = owner;
  }

  protected void AbortEdit() 
  {
    setVisible( false);

    setLocation( -200, -200);

    focused = false;

    if (itsSketchPad.itsToolBar.staySelected())
      itsSketchPad.editStatus = ErmesSketchPad.START_ADD;
    else
      itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;

    itsOwner = null;
  }


  //--------------------------------------------------------
  // lostFocus
  //--------------------------------------------------------
  public void LostFocus() 
  {
    if (! focused)
      return ;

    focused = false;
    itsSketchPad.editStatus = ErmesSketchPad.DOING_NOTHING;
    itsSketchPad.itsSketchWindow.requestFocus();

    String aTextString = getText().trim();

    if ( !itsOwner.getArgs().equals( aTextString) )
      {
	itsOwner.redefine( aTextString);

	itsSketchPad.markSketchAsDirty();
	itsSketchPad.paintDirtyList();
      } 

    setRows( 2);
    setColumns( 20);

    AbortEdit();
  }

  public void focusGained( FocusEvent e) 
  {
    itsSketchPad.editStatus = ErmesSketchPad.EDITING_OBJECT;
    if (!focused)
      focused = true;
  }

  public void focusLost(FocusEvent e) 
  {
  }

  public boolean HasFocus() 
  {
    return focused;
  }

  public void keyTyped(KeyEvent e) 
  {
  }

  public void keyReleased(KeyEvent e) 
  {
  }

  public void keyPressed(KeyEvent e) 
  {
    int lenght;
    String s1, s2;
    String s = getText();
    FontMetrics fm = getFontMetrics(getFont());
    int aWidth = 0;
    int aCurrentLineWidth = 0;
    int aCurrentLineChars = 0;

    if (isEditable()) 
      {
	if (e.getKeyCode()==ircam.jmax.utils.Platform.ENTER_KEY||
	    e.getKeyCode()==ircam.jmax.utils.Platform.RETURN_KEY)
	  {//return
	    setRows(getRows()+1);
	    Dimension d2 = itsOwner.Size();
	    itsOwner.resize( d2.width, d2.height + fm.getHeight());
	    itsSketchPad.repaint();

	    setSize( getSize().width, getSize().height + fm.getHeight());
	    requestFocus();
	    return;
	  } 
	else 
	  {
	    aCurrentLineChars = GetCurrentLineChars();
	    if (aCurrentLineChars+10 > getColumns())
	      setColumns(getColumns()+20);

	    char k = e.getKeyChar();

	    if ( k == Platform.DELETE_KEY 
		 ||  k == Platform.BACKSPACE_KEY 
		 || ErmesSketchWindow.isAnArrow(e.getKeyCode()))
	      return;

	    aCurrentLineWidth = GetCurrentLineWidth( fm);
	    aWidth = itsOwner.itsFontMetrics.getMaxAdvance();

	    if (aCurrentLineWidth >= getSize().width-20) 
	      {
		int step;
		if ( aWidth > 20)
		  step = aWidth;
		else
		  step = 30;

// 		itsOwner.resize( itsOwner.Size().width + step, itsOwner.Size().height);
// 		itsSketchPad.repaint();
		setSize( getSize().width + step, getSize().height);
		requestFocus();
	      }
	  }
      }
  }

  public int GetCurrentLineWidth(FontMetrics fm) 
  {
    String theString = getText();
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;

    while ((aIndex!=-1)&&(aIndex<aPos)) 
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf("\n", aOldIndex+1);
      }

    if (aIndex==-1) 
      {
	if (aOldIndex==-1)
	  return fm.stringWidth(theString);
	else
	  return fm.stringWidth(theString.substring(aOldIndex));
      } 
    else 
      {
	if (aOldIndex==-1)
	  return fm.stringWidth(theString.substring(0,aIndex));
	else
	  return fm.stringWidth( theString.substring(aOldIndex, aIndex));
      }
  }

  public int GetCurrentLineChars()
  {
    String theString = getText();
    int aPos = getCaretPosition();
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;

    while ((aIndex!=-1)&&(aIndex<aPos)) 
      {
	aOldIndex = aIndex;
	aIndex = theString.indexOf("\n", aOldIndex+1);
      }

    if (aIndex==-1) 
      {
	if (aOldIndex==-1)
	  return theString.length();
	else
	  return theString.length()-aOldIndex;
      } 
    else 
      {
	if (aOldIndex==-1)
	  return aIndex;
	else
	  return aIndex-aOldIndex;
      }
  }


  private Dimension minimumSize = new Dimension();

  public Dimension getMinimumSize() 
  {
    minimumSize.setSize( itsOwner.getWidth() - itsOwner.getWhiteXOffset(), itsOwner.getHeight() - itsOwner.getWhiteYOffset());
    return minimumSize;
  }

  public Dimension getPreferredSize() 
  {
    return getMinimumSize();
  }

  // Support for cut editing operations

  void deleteSelectedText()
  {
    String s = getText();

    setText(s.substring(0, getSelectionStart()) +  s.substring(getSelectionEnd(), s.length()));
  }
}
