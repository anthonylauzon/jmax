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

public class ErmesObjFloat extends ErmesObjNumberBox implements FtsFloatValueListener
{
  private float itsFloat = (float) 0.0;

  private float itsStartingValue;

  // values relative to mouse dragging motion
  private float acceleration;
  private float velocity;
  private float previousVelocity;
  private int previousY;

  ErmesObjFloat( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject, "-0123456789.");

    itsFloat = ((FtsFloatValueObject)itsFtsObject).getValue();
  }

  public void valueChanged(float value) 
  {
    itsFloat = value;

    Graphics g = itsSketchPad.getGraphics();
    Paint(g);
    g.dispose();
  }

  // ValueAsText property

  void setValueAsText( String value)
  {
    try
      {
	itsFloat = Float.valueOf( value).floatValue();
      }
    catch (NumberFormatException e1)
      {
	return;
      }

    ((FtsFloatValueObject)itsFtsObject).setValue(itsFloat);
  }

  static private NumberFormat numberFormat;

  static
  {
    // Number format for controlling the float printed by the float box.
    // essentially used to avoid scientific notation in float.
    
    numberFormat = NumberFormat.getInstance(Locale.US);
    numberFormat.setMaximumFractionDigits(6);
    numberFormat.setMinimumFractionDigits(0);

    if (numberFormat instanceof DecimalFormat)
      ((DecimalFormat) numberFormat).setDecimalSeparatorAlwaysShown(true);

    numberFormat.setGroupingUsed(false);
  }


  String getValueAsText()
  {
    return numberFormat.format( itsFloat);
  }

  //--------------------------------------------------------
  // mouse handlers
  //--------------------------------------------------------

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      {
	velocity = 0;
	previousVelocity = 0;
	acceleration = 0;
	previousY = mouse.y;
	state = 1;
	itsSketchPad.setKeyEventClient(this);
	itsStartingValue = itsFloat;
	((FtsFloatValueObject)itsFtsObject).setValue(itsFloat);
	redraw();

      }
    else if (Squeack.isUp(squeack))
      {
	velocity = 0;
	previousVelocity = 0;
	acceleration = 0;
	
	((FtsFloatValueObject)itsFtsObject).updateValue();
	Fts.sync();
	redraw();
      }
    else if (Squeack.isDrag(squeack))
      {
	previousVelocity = velocity;
	velocity = (previousY- mouse.y);
	acceleration = Math.abs(velocity-previousVelocity);
	previousY= mouse.y;

	state = 2;

	float increment;

	if (velocity*previousVelocity > 0)
	  increment = (velocity/1000) + ((velocity>0)?acceleration:-acceleration)/10;
	else
	  increment = velocity/1000;

	if (Squeack.isShift(squeack))
	  increment*=10;

	itsFloat += increment;

	((FtsFloatValueObject)itsFtsObject).setValue(itsFloat);
	redraw();
      }
  }
}










