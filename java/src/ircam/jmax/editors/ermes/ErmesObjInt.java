package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

//
// The "integer box" graphic object.
//

class ErmesObjInt extends ErmesObjNumberBox implements FtsIntValueListener
{
  private int itsInteger = 0;

  private int itsStartingY, itsFirstY;

  public ErmesObjInt( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    itsInteger = ((FtsIntValueObject)itsFtsObject).getValue();
  }

  public void valueChanged(int value) 
  {
    if (itsInteger != value) 
      {
	itsInteger = value;

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific(g);
	g.dispose();
      }
  }

  // ----------------------------------------
  // ValueAsText property
  // ----------------------------------------
  void setValueAsText( String value)
  {
    try
      {
	itsInteger = Integer.parseInt( value);
      }
    catch ( NumberFormatException e)
      {
	return;
      }

    ((FtsIntValueObject)itsFtsObject).setValue(itsInteger);
    DoublePaint();
  }

  String getValueAsText()
  {
    return String.valueOf(itsInteger);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------
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

	itsStartingY = itsInteger;

	((FtsIntValueObject)itsFtsObject).setValue(itsInteger);
      } 
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  void MouseUp(MouseEvent evt,int x, int y) 
  {
    if (itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	Fts.sync();
	DoublePaint();
      }
    else
      super.MouseUp(evt, x, y);
  }

  void MouseDrag_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown() ) 
      {
	if (!evt.isControlDown())
	  state = 2;

	itsInteger = itsStartingY + (itsFirstY - y);
	((FtsIntValueObject)itsFtsObject).setValue(itsInteger);
	DoublePaint();
      } 
  }
}

