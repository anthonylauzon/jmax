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
class ErmesObjBang extends ErmesObject /*implements ActionListener */{

  boolean itsFlashing = false;
  static Dimension preferredSize = new Dimension(20,20);
  public MetaList itsDebugVariable;

  public ErmesObjBang(){
    super();
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad,FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    return true;
  }

  // starting of the graphic/FTS mix
  
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


  protected void FtsValueChanged(Object value) {
    int on_off = ((Integer) value).intValue();
    
    if (on_off == 1) {
      itsFlashing = true;
      //      if (whichToProbe == this) 
      //itsProbe.mark("bang");
    }
    else {
       itsFlashing = false;
    } 
    Paint_specific(itsSketchPad.getGraphics());
  }
	
  public boolean NeedPropertyHandler(){
    return true;
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
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((getItsWidth()+theDeltaX < getMinimumSize().width)||
       (getItsHeight()+theDeltaY < getMinimumSize().height))
      return false;
    else return true;
  }
  //resize to preferredSize()
  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(15,15); //(depending on the layout manager).
  }
  
  //If we don't specify this, the canvas might not show up at all
  //(depending on the layout manager).
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
}







