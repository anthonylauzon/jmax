 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;


import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "integer box" graphic object.
//

public class IntBox extends NumberBox implements FtsIntValueListener
{
  private int itsInteger = 0;

  private int itsStartingValue, itsFirstY;

  public IntBox(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject, "-0123456789");

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

  public void setValueAsText( String value)
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

  public String getValueAsText()
  {
    return String.valueOf(itsInteger);
  }

  public int getIntZoneWidth()
  {
    return 0;
  }

  public void increment( boolean up, boolean shift)
  {
    int incr = shift ? 10 : 1;
    itsInteger = up ? itsInteger + incr : itsInteger - incr;
    ((FtsIntValueObject)ftsObject).setValue( itsInteger);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  boolean dragged = false;

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if ( Squeack.isDown(squeack))
      {
	itsFirstY = mouse.y;
	itsStartingValue = itsInteger;
	dragged = false;
      }
    else if ( Squeack.isDrag(squeack))
      {
	((FtsIntValueObject)ftsObject).setValue(itsStartingValue + (itsFirstY - mouse.y));
	dragged = true;
	setEditMode( NumberBox.DRAG_MODE);
      }
    else if ( Squeack.isUp( squeack))
      {
	if (! dragged)
	  {
	    if( isOnArrow( mouse))
	      {
		setEditMode( NumberBox.INCR_MODE);
		itsSketchPad.setKeyEventClient( this);		
	      }	    
	    else
	      edit( mouse);

	    return;
	  }
      }
  }
}

