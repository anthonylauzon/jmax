//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

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

  private int itsStartingValue, itsFirstY;

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
	itsStartingValue = itsInteger;
	dragged = false;
      }
    else if (Squeack.isDrag(squeack))
      {
	((FtsIntValueObject)ftsObject).setValue(itsStartingValue + (itsFirstY - mouse.y));
	dragged = true;
      }
    else if (Squeack.isUp(squeack))
      {
	if (! dragged)
	  {
	    itsSketchPad.setKeyEventClient( this);
	    valueValid = false;
	    return;
	  }
      }
  }
}
