package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

//
// The graphic "float box" object.
//
class ErmesObjFloat extends ErmesObjNumberBox implements FtsFloatValueListener
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
    super( theSketchPad, theFtsObject);

    itsFloat = ((FtsFloatValueObject)itsFtsObject).getValue();
  }

  public void valueChanged(float value) 
  {
    itsFloat = value;

    Graphics g = itsSketchPad.getGraphics();
    Paint_specific(g);
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

    DoublePaint();
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
  void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;
    previousY = y;

    if ( itsSketchPad.isLocked() || evt.isControlDown()) 
      {
	if (!evt.isControlDown())
	  {
	    state = 1;
	    itsSketchPad.itsSketchWindow.setKeyEventClient( this);
	  }

	itsStartingValue = itsFloat;

	((FtsFloatValueObject)itsFtsObject).setValue(itsFloat);

	DoublePaint();
      } 
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  void MouseUp( MouseEvent evt,int x, int y) 
  {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;

    if ( itsSketchPad.isLocked())
      {
	((FtsFloatValueObject)itsFtsObject).updateValue();
	Fts.sync();
	DoublePaint();
	return;
      } 

    super.MouseUp(evt, x, y);
  }

  void MouseDrag_specific(MouseEvent evt,int x, int y) 
  {
    previousVelocity = velocity;
    velocity = (previousY-y);
    acceleration = Math.abs(velocity-previousVelocity);
    previousY=y;

    if ( itsSketchPad.isLocked() || evt.isControlDown()) 
      {
	if (!evt.isControlDown())
	  state = 2;

	float increment;
	if (velocity*previousVelocity > 0)
	  increment = (velocity/1000) + ((velocity>0)?acceleration:-acceleration)/10;
	else
	  increment = velocity/1000;

	if (evt.isShiftDown())
	  increment*=10;

	itsFloat += increment;

	((FtsFloatValueObject)itsFtsObject).setValue(itsFloat);
	DoublePaint();
      } 
  }
}










