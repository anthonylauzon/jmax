 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The graphic "float box" object.
//

public class FloatBox extends NumberBox implements FtsFloatValueListener
{
  private double value = 0.0;

  // values relative to mouse dragging motion
  private double increment;
  private double valInc;  
  private int itsLastY;
  private int intZoneWidth = 0;
  
  public FloatBox(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject, "-0123456789.");

    value = ((FtsFloatValueObject)ftsObject).getValue();    
  }

  public void setDefaults()
  {
    super.setDefaults();
    updateIntegerZone();
  }

  public void valueChanged( double v) 
  {
    this.value = v;
    updateIntegerZone();
    updateRedraw();
  }

  void updateIntegerZone()
  {
    String val = getValueAsText();
    val = val.substring(0, val.indexOf('.'));
    intZoneWidth = getHeight()/2 + 5 + getFontMetrics().stringWidth( val);
  }
  // ValueAsText property

  static private DecimalFormat formatter;

  static
  {
    // Number format for controlling the float printed by the float box.
    // essentially used to avoid scientific notation in float.
    
    formatter = new DecimalFormat("0.######;-0.######");
    formatter.setGroupingUsed(false);
    formatter.setDecimalSeparatorAlwaysShown(true);
  }

  public void setValueAsText(String v)
  {
    try
      {
	Number n = formatter.parse(v); 

	value = n.doubleValue();
      }
    catch (java.text.ParseException  e1)
      {
	return;
      }

    ((FtsFloatValueObject)ftsObject).setValue( value);
  }


  public String getValueAsText()
  {
    return formatter.format( value);
  }

  public void setFont( Font theFont)
  {
    super.setFont( theFont);
    updateIntegerZone();
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  boolean dragged = false;

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if(Squeack.isDown(squeack))
      {
	dragged = false;
	itsLastY = mouse.y;
      }
    else if(Squeack.isDrag(squeack))
      {
	if(!dragged)
	  {
	    if( isInIntegerZone( mouse.x))
	      increment = 1.0;
	    else
	      increment = 0.001;
	    
	    ((FtsFloatValueObject)ftsObject).setValue( value);
	    
	    itsLastY = mouse.y;
	    dragged = true;
	  }
	else
	  {
	    if(Squeack.isShift(squeack))
	      value = value + (double)(itsLastY - mouse.y) * increment * 10.;
	    else
	      value = value + (itsLastY - mouse.y) * increment;

	    ((FtsFloatValueObject)ftsObject).setValue( value);
	    
	    itsLastY = mouse.y;
	  }
      }
    else if(Squeack.isUp(squeack))
      {
	if(!dragged)
	  {
	    itsSketchPad.setKeyEventClient(this);
	    setValueValid(false);
	    return;
	  }
      }    
  }

  private boolean isInIntegerZone( int x)
  {
    return ( (x > getX()) && (x < getX()+intZoneWidth)); 
  }

  public int getIntZoneWidth()
  {
    return intZoneWidth;
  }
}










