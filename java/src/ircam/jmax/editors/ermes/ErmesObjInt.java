package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

//
// The "integer box" graphic object.
//

class ErmesObjInt extends ErmesObjNumberBox {
  private int itsInteger = 0;

  private int itsStartingY, itsFirstY;

  public ErmesObjInt( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    Object value  = (Integer)itsFtsObject.get("value");

    if (value instanceof Integer)
      itsInteger = ((Integer)value).intValue();
  }

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    int temp = ((Integer) value).intValue();

    if (itsInteger != temp) 
      {
	itsInteger = temp;

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

    itsFtsObject.put("value", itsInteger);
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

	itsFtsObject.put( "value", new Integer(itsInteger));
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
	itsFtsObject.put( "value", new Integer(itsInteger));
	DoublePaint();
      } 
  }
}

