package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;


import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;

//
// The "integer box" graphic object.
//

public class ErmesObjInt extends ErmesObjNumberBox implements FtsIntValueListener
{
  private int itsInteger = 0;

  private int itsStartingY, itsFirstY;

  public ErmesObjInt( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject, "-0123456789");

    itsInteger = ((FtsIntValueObject)itsFtsObject).getValue();
  }

  public void valueChanged(int value) 
  {
    if (itsInteger != value) 
      {
	itsInteger = value;

	Graphics g = itsSketchPad.getGraphics();
	Paint(g);
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
  }

  String getValueAsText()
  {
    return String.valueOf(itsInteger);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  public void mouseDown(MouseEvent evt,int x, int y) 
  {
    if (!evt.isControlDown())
      {
	state = 1;
	itsSketchPad.setKeyEventClient( this);
      }

    itsFirstY = y;

    itsStartingY = itsInteger;

    ((FtsIntValueObject)itsFtsObject).setValue(itsInteger);
  }

  public void mouseUp(MouseEvent evt,int x, int y) 
  {
    Fts.sync();
    redraw();
  }

  public void mouseDrag(MouseEvent evt,int x, int y) 
  {
    if (!evt.isControlDown())
      state = 2;

    itsInteger = itsStartingY + (itsFirstY - y);
    ((FtsIntValueObject)itsFtsObject).setValue(itsInteger);
    redraw();
  }
}

