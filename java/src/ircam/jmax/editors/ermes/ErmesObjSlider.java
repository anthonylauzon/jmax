package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;
import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "slider" graphic object
 */
class ErmesObjSlider extends ErmesObject {
	
  static ErmesObjSliderDialog itsSliderDialog = null;
  ErmesObjThrottle itsThrottle;
  int itsInteger = 0;
  final static int BOTTOM_OFFSET = 10;
  final static int UP_OFFSET = 10;
  final static int PREFERRED_RANGE = 128;
  int itsRange = PREFERRED_RANGE;
  int itsPixelRange = itsRange;
  static Dimension preferredSize = new Dimension(20,BOTTOM_OFFSET+PREFERRED_RANGE+UP_OFFSET);
  boolean itsMovingThrottle = false;
  int itsDelta = 0;
  float itsStep =  itsRange/itsPixelRange;

  static final int TRUST=100;		//how many transmitted values we trust?
  int transmission_buffer[];
  int transmission_index = 0;
  int receiving_index = 0;
  int last_value = 0;
  boolean noSync = false;
  
  public ErmesObjSlider(){
    super();
    transmission_buffer = new int[TRUST];
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    Dimension d = getPreferredSize();
    currentRect = new Rectangle(x, y, d.width, d.height);
    itsThrottle = new ErmesObjThrottle(this);
    if(itsSliderDialog == null) itsSliderDialog = new ErmesObjSliderDialog(theSketchPad.GetSketchWindow(), this);
    return true;
  }

		
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    super.Init(theSketchPad,  theFtsObject);
    itsThrottle = new ErmesObjThrottle(this);
    if(itsSliderDialog == null) itsSliderDialog = new ErmesObjSliderDialog(theSketchPad.GetSketchWindow(), this);
    return true;
  }

  public void Resize(int theDeltaH, int theDeltaV){
    super.Resize(theDeltaH, theDeltaV);
    // Change itsStep
    itsPixelRange += theDeltaV;

    itsStep = (float)itsRange/itsPixelRange;
    itsThrottle.Resize(itsThrottle.getPreferredSize().width+theDeltaH, itsThrottle.getPreferredSize().height);
    itsThrottle.Move(itsThrottle.itsX, (int)(itsY+currentRect.height - BOTTOM_OFFSET -itsInteger/itsStep));
  }

  // starting of the graphic/FTS mix
  
  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "slider");
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }
  }

  public void redefineFtsObject()
  {
    // sliders do not redefine themselves
  }

  public void FromDialogValueChanged(Integer theInt){
    itsRange = theInt.intValue();
    Resize(0,0);
    DoublePaint();
  }

  
  //***************************
  // callback function from the associated FtsObject in FTS
  //***************************

  protected void FtsValueChanged(Object value) {
    int temp = ((Integer) value).intValue();        
    int clippedValue = (temp<0)?0:((temp>=itsRange)?itsRange:temp);

    last_value = temp;
    if ((itsThrottle != null) && (!itsMovingThrottle)) {
      itsThrottle.Move(itsThrottle.itsX, (int)(itsY+currentRect.height - BOTTOM_OFFSET -clippedValue/itsStep));
      DoublePaint();
    }
  }
  

  
  public boolean MouseDown_specific(MouseEvent evt, int x, int y){
    if(itsSketchPad.itsRunMode){
      if(IsInThrottle(x,y)){
	itsMovingThrottle = true;
	//itsDelta = y-itsThrottle.itsY;// for now
	return true;
      }
    }
    else if(evt.getClickCount()>1) {
      Point aPoint = GetSketchWindow().getLocation();
      itsSliderDialog.setLocation(aPoint.x + itsX,aPoint.y + itsY - 25);
      itsSliderDialog.ReInit(String.valueOf(itsRange), this, GetSketchWindow());
      itsSliderDialog.setVisible(true);
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return false;
  }
  
  public boolean MouseDrag(MouseEvent evt,int x, int y){
    if((itsSketchPad.itsRunMode)&&(itsMovingThrottle == true)){
      if (itsY+currentRect.height-y> BOTTOM_OFFSET && itsY+UP_OFFSET<y) {
	//compute the value and send to FTS
	itsInteger = (int)(((itsY+currentRect.height)-y-BOTTOM_OFFSET)*itsStep);
	itsFtsObject.put("value", new Integer(itsInteger));
	itsThrottle.Move(itsThrottle.itsX, y);
	DoublePaint();
      }
      return true;
    }
    return false;
  }

  //e.m. this function would be dropped, no one is calling it?
  void Trust (int theInt) {
    int temp = (receiving_index!=0)?receiving_index-1:TRUST-1;
    if (transmission_index == receiving_index-1 || (transmission_index == TRUST && receiving_index == 0)) { //(the buffer is circular)
      //we sent more then TRUST values without acknowledge...
      //write a message? FTS, are you there?
      itsInteger = last_value;
      //DoublePaint();
      //e.m.System.out.println("-->\t\tmore then "+TRUST+"values without ack");
    }
    else {
      //e.m.System.out.println("+ "+theInt);
      transmission_buffer[transmission_index] = theInt;
      transmission_index = (transmission_index+1) % TRUST;
    }
  }
	  
  public boolean MouseUp(MouseEvent evt,int x, int y){
    if(itsSketchPad.itsRunMode){
      MaxApplication.getFtsServer().syncToFts();
      itsMovingThrottle = false;
      DoublePaint();
      return true;
    }
    else return super.MouseUp(evt, x, y);
  }
  
  public boolean IsInThrottle(int theX, int theY){
    Rectangle aRect = itsThrottle.Bounds();
    return aRect.contains(theX,theY);
  }
  
  void MoveThrottleTo(int value) {//value between 0 and itsRange, to be scaled to 0ÖcurrentRect.height
    itsThrottle.Move(itsThrottle.itsX, itsThrottle.DragToAbsolute((int)(value*itsStep)));
    /*if(value>itsRange) value = itsRange;
      else if(value<0) value = 0;
      itsThrottle.Move(itsThrottle.itsX, itsY + UP_OFFSET + itsPixelRange - (value * itsStep));*/
    if (itsSketchPad.getGraphics() != null)
      // Paint_specific(itsSketchPad.getGraphics());
      DoublePaint();//?
  }
	
  public boolean NeedPropertyHandler(){
    return true;
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    if (!theRequester.IsInlet())	
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester));
  }

  public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
    theRequester.ChangeState(false, theRequester.connected);
    itsSketchPad.ResetConnect();
    return true;
  }
  
  public void Paint_specific(Graphics g) {
    
    if (g==null) {
      System.out.println("Error in graphic update: null graphic contest");
      return;
    }
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    //g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2, true);
    g.setColor(Color.black);
    g.drawRect(itsX+0,itsY+ 0, currentRect.width-1, currentRect.height-1);
    g.drawLine(itsX+currentRect.width/2, itsY+UP_OFFSET, itsX+currentRect.width/2, itsY+currentRect.height-BOTTOM_OFFSET);
	  
    //paint dragBox
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    //paint throttle
    // g.translate(itsX+itsThrottle.LATERAL_OFFSET, itsY+itsThrottle.AbsoluteToSlider(itsThrottle.itsY));
    itsThrottle.Paint(g);
    //g.translate(-itsX-itsThrottle.LATERAL_OFFSET, -itsY-itsThrottle.AbsoluteToSlider(itsThrottle.itsY));
  }

  public void MoveBy(int theDeltaH, int theDeltaV) {
    super.MoveBy(theDeltaH, theDeltaV);
    itsThrottle.MoveBy(theDeltaH, theDeltaV);
  }

  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize();
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}
