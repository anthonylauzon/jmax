package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The "comment" graphic object
//
class ErmesObjComment extends ErmesObject implements ErmesObjEditable {
  public String itsMaxString = "";

  Dimension preferredSize;
  String itsArgs;
  MaxVector itsParsedTextVector = new MaxVector();
  int itsTextRowNumber = 0;

  final static int TEXT_INSET = 10;

  public ErmesObjComment( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);
  }


  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public void Init() 
  {
    int FIELD_HEIGHT;

    FontMetrics temporaryFM = itsSketchPad.getFontMetrics(itsSketchPad.getFont());
    FIELD_HEIGHT = temporaryFM.getHeight();

    itsArgs = itsFtsObject.getDescription();

    if (itsArgs.equals(""))
      // preferredSize = new Dimension(70,FIELD_HEIGHT*5);
      // Since the comments are currently single lines, we initialize it to a single line.

      preferredSize = new Dimension(70,FIELD_HEIGHT + 3);
    else
      preferredSize = new Dimension( temporaryFM.stringWidth(itsArgs),FIELD_HEIGHT*5);

    super.Init();

    itsSketchPad.GetTextArea().setBackground( Color.white);

    ParseText(itsArgs);
    RestoreDimensions();
  }


  public void redefineFtsObject() 
  {
    ((FtsCommentObject)itsFtsObject).setComment(itsArgs);
  }

  public void RestoreDimensions() 
  {
    if (! itsMaxString.equals(""))
      resizeBy( itsFontMetrics.stringWidth(itsMaxString) + TEXT_INSET - getItsWidth(), 
		itsFontMetrics.getHeight() * itsParsedTextVector.size() - getItsHeight());
    // itsSketchPad.repaint(); // BARBOGIO
  }


  public void MouseDown_specific(MouseEvent evt, int x, int y) 
  {
    if (itsSketchPad.itsRunMode)
      return;
    itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  public void startEditing() 
  {
    restartEditing();
  }

  public void restartEditing() 
  {
    if (itsSketchPad.GetTextArea() != null)
      itsSketchPad.GetTextArea().setEditable(true);

    itsSketchPad.GetTextArea().setFont(getFont());

    if (itsArgs == null)
      itsSketchPad.GetTextArea().setText("");
    else
      itsSketchPad.GetTextArea().setText(itsArgs);

    itsSketchPad.GetTextArea().itsOwner = this;

    if ( itsParsedTextVector.size() == 0)
      itsSketchPad.GetTextArea().setBounds( getItsX(), getItsY(), 
					    getItsWidth()+20, itsFontMetrics.getHeight()*5);
    else
      itsSketchPad.GetTextArea().setBounds( getItsX(), getItsY(), 
					    getItsWidth()+10, 
					    itsFontMetrics.getHeight()*(itsParsedTextVector.size()+1));

    itsMaxString = "";
    itsParsedTextVector.removeAllElements();

    itsSketchPad.GetTextArea().setVisible( true);
    itsSketchPad.GetTextArea().requestFocus();
    itsSketchPad.GetTextArea().setCaretPosition( itsArgs.length());
  }

  public void setSize(int theH, int theV) 
  {
    resize(theH, theV);
    itsSketchPad.repaint();
  }

  public void setSize(Dimension d) 
  {
    setSize(d.width, d.height);
  }


  public void ResizeToText(int theDeltaX, int theDeltaY) 
  {
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;

    if ( aWidth < itsFontMetrics.stringWidth( itsMaxString) + TEXT_INSET)
      aWidth = itsFontMetrics.stringWidth( itsMaxString) + TEXT_INSET;

    if (aHeight < itsFontMetrics.getHeight() * itsParsedTextVector.size())
      aHeight = itsFontMetrics.getHeight() * itsParsedTextVector.size();

    resizeBy( aWidth - getItsWidth(), aHeight - getItsHeight());
  }

  public boolean canResizeBy(int theDeltaX, int theDeltaY) 
  {
    String temp = itsArgs;
    if ( ( getItsWidth() + theDeltaX < itsFontMetrics.stringWidth( itsMaxString) + TEXT_INSET)
	 || ( getItsHeight() + theDeltaY < itsFontMetrics.getHeight()*itsParsedTextVector.size()) )
      return false;
    else
      return true;
  }

  public void ParseText(String theString) 
  {
    // (fd) {
    // See ErmesObjEditableObject for comments on fonts
    // PLUS: a nice example of copy/paste programming..

    // int aIndex = theString.indexOf( "\n");
    // int aOldIndex = -1;
    // int aLastIndex = theString.lastIndexOf( "\n");
    // String aString;
    // int length = 0;
    // int i = 0;

    // while ( aIndex != -1)
    // {
    // aString = theString.substring( aOldIndex + 1, aIndex);
    // length = itsFontMetrics.stringWidth( aString);

    // if ( length > itsFontMetrics.stringWidth(itsMaxString))
    // itsMaxString = aString;

    // itsParsedTextVector.addElement( aString);
    // aOldIndex = aIndex;
    // aIndex = theString.indexOf( "\n", aOldIndex+1);
    // i++;
    // }

    // aString = theString.substring( aOldIndex+1);
    // length = itsFontMetrics.stringWidth( aString);

    // if ( length > itsFontMetrics.stringWidth( itsMaxString))
    // itsMaxString = aString;

    // itsParsedTextVector.addElement( aString);

    itsParsedTextVector.removeAllElements();
    itsParsedTextVector.addElement( theString);
    itsMaxString = theString;
    // } (fd)
  }

  public void Paint_specific(Graphics g) 
  {
    if (!itsSketchPad.itsRunMode) 
      {
	if (itsSelected)
	  g.setColor(Color.gray);
	else
	  g.setColor(itsSketchPad.getBackground());

	g.fill3DRect( getItsX(), getItsY(), getItsWidth(), getItsHeight(), true);

	//drag box
	if (itsSelected) 
	  {
	    g.setColor(Color.gray.darker());
	    g.fillRect( getItsX() + getItsWidth() - DRAG_DIMENSION, getItsY() + getItsHeight() - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
	  }
      }

    //text
    if (itsArgs != null) 
      {
	g.setColor( Color.black);
	g.setFont( getFont());
	DrawParsedString( g);
      }
  }

  private void DrawParsedString(Graphics theGraphics) 
  {
    String aString;
    int i=0;
    int insetY = (getItsHeight() - itsFontMetrics.getHeight() * itsParsedTextVector.size()) / 2;

    for ( Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements(); )
      {
	aString = (String)e.nextElement();
	theGraphics.drawString( aString, 
				getItsX()+2, 
				getItsY() + itsFontMetrics.getAscent() + insetY + itsFontMetrics.getHeight()*i);
	i++;
      }
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() 
  {
    if ( itsParsedTextVector.size() == 0)
      return getPreferredSize();
    else
      return new Dimension( itsFontMetrics.stringWidth( itsMaxString) + TEXT_INSET, 
			    itsFontMetrics.getHeight() * itsParsedTextVector.size());
  }

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
