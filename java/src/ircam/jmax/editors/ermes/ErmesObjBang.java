package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "bang" graphic object.
 */
class ErmesObjBang extends ErmesObject {

  class FlashingThread extends Thread {

    public FlashingThread(String str) {
      super(str);
    }
    
    public void run() {
      while (true) {//make a first flash, then suspend itself, waiting for successive resume()
	Graphics aGraphics = ErmesObjBang.this.GetSketchPad().getGraphics();
	if (aGraphics != null) {
	  ErmesObjBang.this.itsFlashing = true;
	  ErmesObjBang.this.Paint(aGraphics);
	  try {
	    sleep(100);
	  } catch (InterruptedException e) {}
	  ErmesObjBang.this.itsFlashing = false;
	  ErmesObjBang.this.Paint(aGraphics);
	}	
	suspend();
      }
    }
  }

  boolean itsFlashing = false;
  FlashingThread	itsFlashingThread;
  static Dimension preferredSize = new Dimension(20,20);
  
  // there was a try to make an offscreen drawing for every object:
  // it worked, but we didn't get too much performance gain. The doubt is that the Image class
  // in AWT is just a "recorder" of graphics operations, and not a simple bitmap. 
  //Graphics offScreenGraphics = null;
  //Dimension offScreenDimension;
  //Image offScreenImage;
  //boolean firstPaint = true;
  
  public ErmesObjBang(){
    super();
    //setLayout(null);
  }


  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    itsFlashingThread = new FlashingThread("aFlash");
    return true;
  }

  //The "fts_related" Init is reimplemented here because we need to create the flashing thread
  //also when we are loading from a file
  public boolean Init(ErmesSketchPad theSketchPad,FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    itsFlashingThread = new FlashingThread("aFlash");
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
    // Bang don't redefine itself
  }
    
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      //itsFtsObject.put("value", new Integer(1));//?????
      itsFtsObject.sendMessage(0, "bang", null);
      DoublePaint();
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }

  protected void FtsValueChanged(Object value) {
    //1. new EObjFlashingThread(this, "aFlash").start();
    if (itsFlashingThread.isAlive()) itsFlashingThread.resume();
    else itsFlashingThread.start();
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
    
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    if(itsFlashing) {	//only a simulation for now
      g.setColor(Color.yellow);
      itsFlashing = false;
    }
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    g.setColor(Color.black);
    g.drawRect(itsX, itsY, currentRect.width-1, currentRect.height-1);
    g.setColor(Color.darkGray);
    g.drawOval(itsX+5, itsY+5, currentRect.width-10, currentRect.height-10);
    //if (itsFlashing) {	//only a simulation for now
    //g.setColor(Color.yellow);
    //g.fillOval(itsX+5,itsY+ 5, currentRect.width-10, currentRect.height-10);
    //itsFlashing = false;
    //}
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < getPreferredSize().width)||
       (currentRect.height+theDeltaY < getPreferredSize().height))
      return false;
    else return true;
  }
  //resize to preferredSize()
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<getPreferredSize().width) aWidth = getPreferredSize().width;
    if(aHeight<getPreferredSize().height) aHeight = getPreferredSize().height;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize(); //(depending on the layout manager).
  }
  
  //If we don't specify this, the canvas might not show up at all
  //(depending on the layout manager).
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
}







