package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.utils.*;

/**
 * The "bang" graphic object.
 */
class ErmesObjBang extends ErmesObject implements FtsPropertyHandler {

  boolean itsFlashing = false;
  static Dimension preferredSize = new Dimension(20,20);
  static Dimension minimumSize = new Dimension(15, 15);

  public ErmesObjBang(){
    super();
  }

  
  
  public void makeFtsObject() {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "button");
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }
  }
  
  public void redefineFtsObject() {
  }
    
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      
      itsFtsObject.sendMessage(0, "bang", null);
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }

  
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);
    itsFtsObject.watch("value", this);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    itsFtsObject.watch("value", this);
    return true;
  }

  public void propertyChanged(FtsObject obj, String name, Object value) {
    int on_off = ((Integer) value).intValue();
    
    if (on_off == 1) {
      itsFlashing = true;
    }
    else {
       itsFlashing = false;
    } 
    Paint_specific(itsSketchPad.getGraphics());
  }
	

  public boolean isUIController() {
    return true;
  }

  public void Paint_specific(Graphics g) {
    
    if (g == null) return;
    if(!itsFlashing){
      if(!itsSelected) g.setColor(itsUINormalColor);
      else g.setColor(itsUISelectedColor);
    }
    else{
      g.setColor(Color.yellow);
      itsFlashing = false;
    }
    g.fillRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2);
    g.fill3DRect(getItsX()+2,getItsY()+2, getItsWidth()-4,  getItsHeight()-4, true);
    g.setColor(Color.black);
    g.drawRect(getItsX(), getItsY(), getItsWidth()-1, getItsHeight()-1);
    g.drawOval(getItsX()+5, getItsY()+5, getItsWidth()-10, getItsHeight()-10);
 
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  
  //--------------------------------------------------------
  // minimum and preferred sizes
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return minimumSize; 
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
}
















