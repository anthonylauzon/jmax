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
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "button");
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

  public void Paint_specific(Graphics g) {
    if(!itsFlashing){
      if(!itsSelected) g.setColor(itsUINormalColor);
      else g.setColor(itsUISelectedColor);
    }
    else{
      g.setColor(Color.yellow);
      itsFlashing = false;
    }
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    g.setColor(Color.black);
    g.drawRect(itsX, itsY, currentRect.width-1, currentRect.height-1);
    g.drawOval(itsX+5, itsY+5, currentRect.width-10, currentRect.height-10);
 
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < getMinimumSize().width)||
       (currentRect.height+theDeltaY < getMinimumSize().height))
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







