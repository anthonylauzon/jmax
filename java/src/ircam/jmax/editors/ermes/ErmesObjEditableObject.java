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
  public MaxVector itsParsedTextVector = new MaxVector();
  public String itsMaxString = "";
  public boolean itsInEdit = true;

  static final int HEIGHT_DIFF = 3;
  static final int TEXT_INSET = 2;

  int TEXT_OFFSET;
  int itsMaxStringWidth = 0; // (fd)
  String itsArgs;
  String itsBackupText = new String();
  Dimension preferredSize = new Dimension(100, 25);//hu-hu
  Dimension currentMinimumSize = new Dimension();

  public ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);
    TEXT_OFFSET = getWhiteOffset()+TEXT_INSET;
  }

  void cleanAll()
  {
    itsParsedTextVector = null;
    super.cleanAll();
  }

  protected int getWhiteOffset() 
  {
    return 0;
  }

  //--------------------------------------------------------
  // Init from ftsObjects
  //--------------------------------------------------------

  public void Init() 
  {
    int fieldHeight;

    FontMetrics temporaryFM =  itsSketchPad.getFontMetrics(itsSketchPad.getFont());
    fieldHeight = temporaryFM.getHeight();

    if (itsArgs.equals("")) 
      {
	preferredSize = new Dimension(70, fieldHeight+2*HEIGHT_DIFF);
      } 
    else 
      {
	int length = temporaryFM.stringWidth(itsArgs); 
	preferredSize = new Dimension( length + 2*TEXT_OFFSET, fieldHeight+2*HEIGHT_DIFF); 
      }

    super.Init();

    itsInEdit = false;

    itsFtsObject.watch("ins", this);
    itsFtsObject.watch("outs", this);
  }

  /* Property handling */
  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    if ((name == "ins") || (name == "outs"))
      update(itsFtsObject);
  
    itsSketchPad.repaint();
  }
    
  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  void backupText() 
  {
    itsBackupText = itsArgs.toString();
  }

  void restoreText() 
  {
    itsArgs = itsBackupText.toString();
  }
    
  // actually starts the edit operation.
  // The inset parameter specify the relative position of the editfield
  private void doEdit(int editFieldInset) 
  {
    if (itsSketchPad.GetEditField() != null)
      itsSketchPad.GetEditField().setEditable(true);

    itsSketchPad.GetEditField().setFont(getFont());
    itsSketchPad.GetEditField().setText(itsArgs);
    itsSketchPad.GetEditField().itsOwner = this;

    if ( itsParsedTextVector.size() == 0)
      itsSketchPad.GetEditField().setBounds( getItsX() + editFieldInset/2,
					     getItsY()+1, 
					     getItsWidth() - editFieldInset,
					     itsFontMetrics.getHeight()+20);
    else
      itsSketchPad.GetEditField().setBounds( getItsX() + editFieldInset/2,
					     getItsY()+1, 
					     getItsWidth()-editFieldInset,
					     itsFontMetrics.getHeight() *(itsParsedTextVector.size()+1));

    itsMaxString = "";
    itsParsedTextVector.removeAllElements();

    itsSketchPad.GetEditField().setVisible(true);
    itsSketchPad.GetEditField().requestFocus();
    itsSketchPad.GetEditField().setCaretPosition(itsArgs.length());
  }

  public void restartEditing() 
  {
    doEdit( 0);
  }

  public void startEditing() 
  {
    doEdit( 6);
  }
    
  public boolean MouseUp(MouseEvent evt,int x,int y) 
  {
    if (itsInEdit)
      return true;
    else
      return super.MouseUp(evt, x, y);
  }

  public void RestoreDimensions(boolean paintNow) 
  {
    int aMaxWidth = MaxWidth( itsFontMetrics.stringWidth( itsMaxString) + TEXT_OFFSET +  getWhiteOffset(),
			      itsInletList.size()*12,
			      itsOutletList.size()*12);

    int aHeightDiff = itsFontMetrics.getHeight() * itsParsedTextVector.size() + 2 * HEIGHT_DIFF-getItsHeight();
    int aWidthDiff = aMaxWidth - getItsWidth();
    if (aHeightDiff == 0 && aWidthDiff == 0)
      return;

    resizeBy( aWidthDiff, aHeightDiff);
    if (paintNow) 
      {
	if (aHeightDiff < 0 || aWidthDiff <0)
	  itsSketchPad.repaint();
	else
	  DoublePaint();
      } else 
	{
	  if (aHeightDiff < 0 || aWidthDiff <0)
	    itsSketchPad.markSketchAsDirty();
	  else
	    itsSketchPad.addToDirtyObjects(this);
        }
  }

  public int MaxWidth( int uno, int due, int tre) 
  {
    int MaxInt = uno;

    if (due>MaxInt)
      MaxInt=due;
    if (tre>MaxInt)
      MaxInt=tre;
    return MaxInt;
  }


  protected void SetFieldText(String theString) 
  {
    FontMetrics fm = itsSketchPad.GetEditField().getFontMetrics(itsSketchPad.getFont());
    int lenght = fm.stringWidth(theString);
    Dimension d1 = preferredSize;

    while (lenght > d1.width-20)
      d1.width += 20;

    preferredSize = d1;
    resize(d1.width, d1.height);

    itsSketchPad.GetEditField().setText(theString);
    itsSketchPad.GetEditField().setSize(d1.width-TEXT_OFFSET, d1.height-HEIGHT_DIFF);

    itsSketchPad.GetEditField().repaint();
    itsSketchPad.repaint();
  }

  void ResizeToNewFont(Font theFont) 
  {
    ResizeToText(0,0);
  }

  public void ResizeToText(int theDeltaX, int theDeltaY) 
  {
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;

    if ( aWidth < itsFontMetrics.stringWidth(itsMaxString) + TEXT_OFFSET + getWhiteOffset())
      aWidth = itsFontMetrics.stringWidth(itsMaxString) + TEXT_OFFSET + getWhiteOffset();

    if ( aHeight < itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF)
      aHeight = itsFontMetrics.getHeight() * itsParsedTextVector.size() + HEIGHT_DIFF;

    resizeBy( aWidth - getItsWidth(), aHeight - getItsHeight());
  }

  public boolean canResizeBy(int theDeltaX, int theDeltaY) 
  {
    if ((getItsWidth() + theDeltaX < itsFontMetrics.stringWidth( itsMaxString) + TEXT_OFFSET + getWhiteOffset())
	|| (getItsHeight() + theDeltaY < itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF))
      return false;
    else
      return true;
  }

  public void ParseText(String theString) 
  {
    int aIndex = theString.indexOf( "\n");
    int aOldIndex = -1;
    int aLastIndex = theString.lastIndexOf( "\n");
    String aString;
    int width = 0;
    int i = 0;

    while ( aIndex != -1) 
      {

	aString = theString.substring( aOldIndex + 1, aIndex);
	width = itsFontMetrics.stringWidth( aString);

	if ( width > itsMaxStringWidth) 
	  {
	    itsMaxString = aString;
	    itsMaxStringWidth = width;
	  }

	itsParsedTextVector.addElement( aString);
	aOldIndex = aIndex;
	aIndex = theString.indexOf( "\n", aOldIndex + 1);
	i++;
      }

    aString = theString.substring( aOldIndex + 1);
    width = itsFontMetrics.stringWidth( aString);

    if ( width > itsMaxStringWidth) 
      {
	itsMaxString = aString;
	itsMaxStringWidth = width;
      }

    itsParsedTextVector.addElement( aString);
  }

  public void MoveBy(int theDeltaH, int theDeltaV) 
  {
    super.MoveBy( theDeltaH, theDeltaV);
  }

  public void MoveOutlets() 
  {
    ErmesObjOutlet aOutlet;

    for (Enumeration e=itsOutletList.elements(); e.hasMoreElements();) 
      {
	aOutlet = (ErmesObjOutlet) e.nextElement();
	aOutlet.MoveTo(aOutlet.itsX, getItsY()+getItsHeight());
      }
  }

  abstract protected void Paint_specific(Graphics g);

  protected void DrawParsedString(Graphics theGraphics) 
  {
    String aString;
    int i=0;
    int insetY = (getItsHeight() - itsFontMetrics.getHeight()*itsParsedTextVector.size()) / 2;

    for ( Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements(); ) 
      {
	aString = (String)e.nextElement();
	theGraphics.drawString( aString, 
				getItsX() + TEXT_OFFSET, 
				getItsY() + itsFontMetrics.getAscent() + insetY + itsFontMetrics.getHeight()*i);
	i++;
      }
  }

  public Dimension getMinimumSize() 
  {
    if (itsParsedTextVector.size()==0)
      return getPreferredSize();
    else 
      {
	currentMinimumSize.width = itsFontMetrics.stringWidth(itsMaxString) + 2*getWhiteOffset();
	currentMinimumSize.height = itsFontMetrics.getHeight()*itsParsedTextVector.size()+HEIGHT_DIFF;
	return currentMinimumSize;
      }
  }

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
  
