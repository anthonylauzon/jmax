package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;


import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "integer box" graphic object.
//

public class IntBox extends NumberBox implements FtsIntValueListener
{
  private int itsInteger = 0;

  private int itsStartingY, itsFirstY;

  public IntBox( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject, "-0123456789");

    itsInteger = ((FtsIntValueObject)ftsObject).getValue();
  }

  public void valueChanged(int value) 
  {
    if (itsInteger != value) 
      {
	itsInteger = value;
	updateRedraw();
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

    ((FtsIntValueObject)ftsObject).setValue(itsInteger);
  }

  String getValueAsText()
  {
    return String.valueOf(itsInteger);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  boolean dragged = false;

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      {
	itsFirstY = mouse.y;
	itsStartingY = itsInteger;
	((FtsIntValueObject)ftsObject).setValue(itsInteger);
	dragged = false;
	redraw();
      }
    else if (Squeack.isDrag(squeack))
      {
	itsInteger = itsStartingY + (itsFirstY - mouse.y);
	((FtsIntValueObject)ftsObject).setValue(itsInteger);
	redraw();
	dragged = true;
      }
    else if (Squeack.isUp(squeack))
      {
	Fts.sync();

	if (! dragged)
	  {
	    itsSketchPad.setKeyEventClient( this);
	    valueValid = false;
	  }

	itsFirstY = mouse.y;
	redraw();
      }
  }
}

