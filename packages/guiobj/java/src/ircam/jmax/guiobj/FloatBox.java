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

  public FloatBox( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject, "-0123456789.");

    value = (double)((FtsFloatValueObject)ftsObject).getValue();
  }

  public void valueChanged(float v) 
  {
    this.value = (double)v;
    updateRedraw();
  }

  // ValueAsText property

  static private DecimalFormat formatter;

  static
  {
    // Number format for controlling the float printed by the float box.
    // essentially used to avoid scientific notation in float.
    
    formatter = new DecimalFormat("0.####;-0.####");
    formatter.setGroupingUsed(false);
    formatter.setDecimalSeparatorAlwaysShown(true);
  }

  public void setValueAsText(String v)
  {
    try
      {
	  Number n = formatter.parse(v); 

	  value = n.floatValue();
      }
    catch (java.text.ParseException  e1)
      {
	return;
      }

    ((FtsFloatValueObject)ftsObject).setValue((float)value);
  }


  public String getValueAsText()
  {
    return formatter.format(value);
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
	    double valueAbs = Math.abs(value);

	    if(valueAbs > 750.)
	      {
		increment = 1.;
	      }
	    else if(valueAbs > 0.1)
	      {
		double decade = Math.floor(Math.log(valueAbs) / Math.log(10.));
		double decadeFactor = Math.exp(decade * Math.log(10.));
		double factor = valueAbs / decadeFactor;

		if(factor > 7.5)
		  factor = 10.;
		else if(factor > 3.5)
		  factor = 5.;
		else if(factor > 1.5)
		  factor = 2.;
		else
		  factor = 1.;
		
		increment = 0.001 * factor * decadeFactor;
	      }
	    else
	      increment = 0.0001;
	    
	    if(mouse.y > itsLastY)
	      valInc = Math.floor(value / increment); // move down
	    else
	      valInc = Math.ceil(value / increment); // move up

	    value = valInc * increment;
	    ((FtsFloatValueObject)ftsObject).setValue((float)value);

	    itsLastY = mouse.y;
	    dragged = true;
	  }
       else
	 {
	   if(Squeack.isShift(squeack))
	     valInc += (double)(itsLastY - mouse.y) * 10.;
	   else
	     valInc += (double)(itsLastY - mouse.y);
	   
	    value = valInc * increment;
	   ((FtsFloatValueObject)ftsObject).setValue((float)value);

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
}










