package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The graphic "float box" object.
//

public class FloatBox extends NumberBox implements FtsFloatValueListener
{
  private float value = (float) 0.0;

  private float itsStartingValue;

  // values relative to mouse dragging motion
  private float acceleration;
  private float velocity;
  private float previousVelocity;
  private int previousY;

  FloatBox( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject, "-0123456789.");

    value = ((FtsFloatValueObject)ftsObject).getValue();
  }

  public void valueChanged(float v) 
  {
    this.value = v;
    updateRedraw();
  }

  // ValueAsText property

  static private DecimalFormat formatter;

  static
  {
    // Number format for controlling the float printed by the float box.
    // essentially used to avoid scientific notation in float.
    
    formatter  = new DecimalFormat("0.######;-0.######");
    formatter.setGroupingUsed(false);
    formatter.setDecimalSeparatorAlwaysShown(true);
  }

  void setValueAsText(String v)
  {
    try
      {
	value = ((Double) formatter.parse(v)).floatValue();
      }
    catch (java.text.ParseException  e1)
      {
	return;
      }

    ((FtsFloatValueObject)ftsObject).setValue(value);
  }


  String getValueAsText()
  {
    return formatter.format( value);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  boolean dragged = false;

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      {
	dragged = false;
	velocity = 0;
	previousVelocity = 0;
	acceleration = 0;
	previousY = mouse.y;
	itsStartingValue = value;
      }
    else if (Squeack.isDrag(squeack))
      {
	dragged = true;
	previousVelocity = velocity;
	velocity = (previousY- mouse.y);
	acceleration = Math.abs(velocity-previousVelocity);
	previousY= mouse.y;

	float increment;

	if (velocity*previousVelocity > 0)
	  increment = (velocity/1000) + ((velocity>0)?acceleration:-acceleration)/10;
	else
	  increment = velocity/1000;

	if (Squeack.isShift(squeack))
	  increment*=10;

	value += increment;

	((FtsFloatValueObject)ftsObject).setValue(value);
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










