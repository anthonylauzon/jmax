package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

//import ircam.jmax.MaxApplication; //using the global probe...
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
//import com.sun.java.swing.Timer;

/**
 * The "bang" graphic object.
 */
class ErmesObjBang extends ErmesObject /*implements ActionListener */{

  boolean itsFlashing = false;
  static Dimension preferredSize = new Dimension(20,20);

  public ErmesObjBang(){
    super();
  }

  /*  public void actionPerformed(ActionEvent e) {
    itsFlashing = false;
    Paint_specific(itsSketchPad.getGraphics());
    itsTimer.stop();
  }*/

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad,FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    //itsFlashingThread = new FlashingThread("aFlash");
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
    if (itsSketchPad.itsRunMode) {
      itsFtsObject.sendMessage(0, "bang", null);
      itsFlashing = true;
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }

  /*  public boolean MouseUp_specific(MouseEvent e, int x, int y) {
    if (!userInitiatedFlash) return false; //fts is controlling this flash
    else {
      itsFlashing = false;
      Paint_specific(itsSketchPad.getGraphics());
      userInitiatedFlash = false;
      return true;
    }
  }*/

  protected void FtsValueChanged(Object value) {
    int on_off = ((Integer) value).intValue();

    if (on_off == 1) {
	itsFlashing = true;
    }
    else {
	itsFlashing = false;
    }
    Paint_specific(itsSketchPad.getGraphics());
  }
	
  public boolean NeedPropertyHandler(){
    return true;
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester)
  {
    // HERE the checking: is the type of connection requested allowed?
    if (!theRequester.IsInlet())	//if is an outlet...
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); // then, is it's an inlet
  }

  public boolean ConnectionAbort(ErmesObjInOutlet theRequester)
  {
    // HERE the checking: is the type of connection abort allowed?
    // (for now always allowed)
    theRequester.ChangeState(false, theRequester.connected);
    itsSketchPad.ResetConnect();
    return true;	//for now, everything is allowed
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
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<getMinimumSize().width) aWidth = getMinimumSize().width;
    if(aHeight<getMinimumSize().height) aHeight = getMinimumSize().height;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
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







