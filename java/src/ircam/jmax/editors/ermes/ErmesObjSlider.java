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
	
  ErmesObjThrottle itsThrottle;
  int itsInteger = 0;
  final static int BOTTOM_OFFSET = 10;
  final static int UP_OFFSET = 10;
  final static int PREFERRED_RANGE_MAX = 128;
  final static int PREFERRED_RANGE_MIN = 0;
  int itsRangeMax = PREFERRED_RANGE_MAX;
  int itsRangeMin = PREFERRED_RANGE_MIN;
  int itsRange = itsRangeMax-itsRangeMin;
  int itsPixelRange = itsRangeMax-itsRangeMin;
  static Dimension preferredSize = new Dimension(20,BOTTOM_OFFSET+PREFERRED_RANGE_MAX+UP_OFFSET);
  static Dimension currentMinimumSize = new Dimension(20,BOTTOM_OFFSET+PREFERRED_RANGE_MAX+UP_OFFSET);
  boolean itsMovingThrottle = false;
  int itsDelta = 0;
  float itsStep =  itsRange/itsPixelRange;

  static final int TRUST=50;		//how many transmitted values we trust?
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
    //currentRect = new Rectangle(x, y, getPreferredSize().width, getPreferredSize().height);
    itsThrottle = new ErmesObjThrottle(this);
    return true;
  }

		
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    super.Init(theSketchPad,  theFtsObject);
    itsThrottle = new ErmesObjThrottle(this);    
    {
      Integer aInteger = (Integer)theFtsObject.get("minValue");
      itsRangeMin = aInteger.intValue();
      aInteger = (Integer)theFtsObject.get("maxValue");
      itsRangeMax = aInteger.intValue();
      itsRange = itsRangeMax-itsRangeMin;
      itsStep = (float)itsRange/itsPixelRange;      
    }
    Resize(0,0);
    return true;
  }

  public void Resize(int theDeltaH, int theDeltaV){
    super.Resize(theDeltaH, theDeltaV);
    itsPixelRange = currentRect.height-(UP_OFFSET+BOTTOM_OFFSET);
    
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

  public void FromDialogValueChanged(Integer theCurrentInt, Integer theMaxInt, Integer theMinInt){
    itsRangeMax = theMaxInt.intValue();
    itsRangeMin = theMinInt.intValue();
    itsRange = itsRangeMax-itsRangeMin;
    itsStep = (float)itsRange/itsPixelRange;

    int temp = theCurrentInt.intValue();        
    int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
    itsInteger = clippedValue;
    itsFtsObject.put("value", new Integer(itsInteger));
  }

  
  //***************************
  // callback function from the associated FtsObject in FTS
  //***************************

  protected void FtsValueChanged(Object value) {
    int temp = ((Integer) value).intValue();
    int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
    clippedValue-=itsRangeMin;
    last_value = temp;
    if ((itsThrottle != null) && (!itsMovingThrottle)) {
      itsThrottle.Move(itsThrottle.itsX, (int)(itsY+currentRect.height-BOTTOM_OFFSET-clippedValue/itsStep));
      DoublePaint();
    }
  }
  
  private void SetSliderDialog(){
    Point aPoint = GetSketchWindow().getLocation();
    itsSketchPad.GetSliderDialog().setLocation(aPoint.x + itsX,aPoint.y + itsY - 25);
    itsSketchPad.GetSliderDialog().ReInit(String.valueOf(itsRangeMax), String.valueOf(itsRangeMin),
				   String.valueOf(itsInteger), this, itsSketchPad.GetSketchWindow());
    itsSketchPad.GetSliderDialog().setVisible(true);
  }
  
  public boolean MouseDown_specific(MouseEvent evt, int x, int y){
    if(evt.getClickCount()>1) {
      SetSliderDialog();
      return true;
    }
    if(itsSketchPad.itsRunMode){
      if(IsInThrottle(x,y)){
	itsMovingThrottle = true;
	return true;
      }
      else{
	//qui sposta il Throttle nel punto del click
	if(itsY+currentRect.height-BOTTOM_OFFSET>=y && itsY+UP_OFFSET<y) {
	  //compute the value and send to FTS
	  itsInteger = (int)(((itsY+currentRect.height)-y-BOTTOM_OFFSET)*itsStep);
	  itsFtsObject.put("value", new Integer(itsInteger+itsRangeMin));
	  itsThrottle.Move(itsThrottle.itsX, y);
	  itsMovingThrottle = true;
	  DoublePaint();
	}
	else if(itsY+currentRect.height-BOTTOM_OFFSET<y){
	  itsFtsObject.put("value", new Integer(itsRangeMin));
	  itsInteger = 0;
	  itsThrottle.Move(itsThrottle.itsX, itsY+currentRect.height-BOTTOM_OFFSET);
	  DoublePaint();
	}
	else if(itsY+UP_OFFSET>=y){
	  itsFtsObject.put("value", new Integer(itsRangeMax));
	  itsInteger = itsRangeMax;
	  itsThrottle.Move(itsThrottle.itsX, itsY+UP_OFFSET);
	  DoublePaint();
	}
	//else if(evt.getClickCount()>1){
	//SetSliderDialog();
	// return true;
	//}
	return true;
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return false;
  }
  
  public boolean MouseDrag(MouseEvent evt,int x, int y){
    if((itsSketchPad.itsRunMode)&&(itsMovingThrottle == true)){
      if(itsY+currentRect.height-BOTTOM_OFFSET>=y && itsY+UP_OFFSET<y) {
	//compute the value and send to FTS
	itsInteger = (int)(((itsY+currentRect.height)-y-BOTTOM_OFFSET)*itsStep);
	itsFtsObject.put("value", new Integer(itsInteger+itsRangeMin));
	itsThrottle.Move(itsThrottle.itsX, y);
	DoublePaint();
      }
      else if(itsY+currentRect.height-BOTTOM_OFFSET<y){//theValue is the minimum
	itsFtsObject.put("value", new Integer(itsRangeMin));
	itsInteger = 0;
	itsThrottle.Move(itsThrottle.itsX, itsY+currentRect.height-BOTTOM_OFFSET);
	DoublePaint();
      }
      else if(itsY+UP_OFFSET>=y){
	itsFtsObject.put("value", new Integer(itsRangeMax));
	itsInteger = itsRangeMax;
	itsThrottle.Move(itsThrottle.itsX, itsY+UP_OFFSET);
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
      transmission_buffer[transmission_index] = theInt;
      transmission_index = (transmission_index+1) % TRUST;
    }
  }
	  
  public boolean MouseUp(MouseEvent evt,int x, int y){
    if(itsSketchPad.itsRunMode){
      FtsServer.getServer().syncToFts();
      itsMovingThrottle = false;
      DoublePaint();
      return true;
    }
    else return super.MouseUp(evt, x, y);
  }

  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY) {
    //if((currentRect.width+theDeltaX < getPreferredSize().width)||(currentRect.height+theDeltaY<getPreferredSize().height))
    //  return false;
    //else return true;
    
    return true;
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    //int aWidth = currentRect.width+theDeltaX;
    //int aHeight = currentRect.height+theDeltaY;
    //if(aWidth<getPreferredSize().width) aWidth = getPreferredSize().width;
    //if(aHeight<getPreferredSize().height) aHeight = getPreferredSize().height;
    //Resize(aWidth-currentRect.width, aHeight-currentRect.height);
    
    Resize(theDeltaX, theDeltaY);
  }
  
  public boolean IsInThrottle(int theX, int theY){
    Rectangle aRect = itsThrottle.Bounds();
    return aRect.contains(theX,theY);
  }
  
  void MoveThrottleTo(int value) {//value between 0 and itsRange, to be scaled to 0ÖcurrentRect.height
    itsThrottle.Move(itsThrottle.itsX, itsThrottle.DragToAbsolute((int)(value*itsStep)));
    if (itsSketchPad.getGraphics() != null)
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
    g.fill3DRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2, true);
    g.setColor(Color.black);
    g.drawRect(itsX+0,itsY+ 0, currentRect.width-1, currentRect.height-1);
    g.drawLine(itsX+currentRect.width/2, itsY+UP_OFFSET, itsX+currentRect.width/2, itsY+currentRect.height-BOTTOM_OFFSET);
	  
    //paint dragBox
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    //paint throttle
    itsThrottle.Paint(g);
  }

  public void MoveBy(int theDeltaH, int theDeltaV) {
    super.MoveBy(theDeltaH, theDeltaV);
    itsThrottle.MoveBy(theDeltaH, theDeltaV);
  }

  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    currentMinimumSize.width =20;
    currentMinimumSize.height = BOTTOM_OFFSET+(itsRangeMax-itsRangeMin)+UP_OFFSET;
    return currentMinimumSize;
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}










