package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "message box" graphic object.
 */
class ErmesObjMessage extends ErmesObjEditableObject implements FtsPropertyHandler{
  boolean itsFlashing = false;
  public static final int WHITE_OFFSET = 4;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public ErmesObjMessage(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }

  public void Init()
  {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = (String) itsFtsObject.get("value");
    super.Init();
    itsFtsObject.watch("value", this);
    ParseText(itsArgs);
    if(!canResizeBy(0,0)) RestoreDimensions(false);
  }


  protected int getWhiteOffset() {
    return WHITE_OFFSET;
  }

  //--------------------------------------------------------
  //  redefineFtsObject
  //--------------------------------------------------------

  public void redefineFtsObject()
  {
    ((FtsMessageObject)itsFtsObject).setMessage(itsArgs);
  }

  // Set the text; it is a try; the message box object in the
  // application layer take care of converting the message text to the
  // "value" property; it is an hack, should be done more cleanly.

  public void propertyChanged(FtsObject obj, String name, Object value) {
    if (name == "value")
      {
	itsArgs = (String) value;
	ParseText(itsArgs);
	if (!canResizeBy(0, 0)) {
	  ResizeToText(0,0);
	  itsSketchPad.repaint();
	}
	else
	  {
	    Graphics g = itsSketchPad.getGraphics();
	    Paint(g);
	    g.dispose();
	  }
      }
    else
      super.propertyChanged(obj, name, value);
  }

  public boolean isUIController() {
    return true;
  }

  public boolean MouseUp_specific(MouseEvent e, int x, int y){
    if (!itsSketchPad.itsRunMode && !e.isControlDown()) return false;
    else {
      if (itsFlashing) {
	itsFlashing = false;

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific(g);
	g.dispose();
      }
    }
    return true;
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public void MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      if (itsFtsObject != null){
	itsFtsObject.sendMessage(0, "bang", null);
	itsFlashing = true;

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific(g);
	g.dispose();
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    
    if (g == null) return;
    if (!itsSketchPad.itsRunMode) 
      {
	if(itsSelected || itsFlashing) g.setColor(itsLangSelectedColor);
	else g.setColor(itsLangNormalColor);
      }
    else g.setColor(itsUINormalColor);
 
    g.fillRect(getItsX()+1,getItsY()+1,getItsWidth()-2, getItsHeight()-2);
    
    
    if (!itsSketchPad.itsRunMode) 
      {
	if(itsFlashing) g.setColor(itsLangSelectedColor);
	else
	  {
	    if(!itsSelected) g.setColor(Color.white);
	    else g.setColor(itsLangNormalColor);
	  }
      }
    else 
      {
	if(itsFlashing || itsSelected) g.setColor(itsUISelectedColor);
	else g.setColor(Color.white);
      }
  
    g.fillRect(getItsX()+getWhiteOffset(), getItsY()+1, getItsWidth()-(getWhiteOffset()*2), getItsHeight()-HEIGHT_DIFF);
    
    g.setColor(Color.black);
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont(getFont());
    DrawParsedString(g);
  }
}









