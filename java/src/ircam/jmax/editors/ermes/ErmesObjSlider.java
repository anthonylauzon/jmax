package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

/**
 * The "slider" graphic object
 */
class ErmesObjSlider extends ErmesObject {
  ErmesObjThrottle itsThrottle;
  int itsInteger = 0;

  static final int TRUST=10;	     //how many transmitted values we trust?
  int transmission_buffer[];
  int transmission_index = 0;
  int receiving_index = 0;
  int last_value = 0;


  static ErmesObjSliderDialog itsSliderDialog = null;
  final static int BOTTOM_OFFSET = /*10*/5;
  final static int UP_OFFSET = /*10*/5;
  final static int PREFERRED_RANGE_MAX = 127;
  final static int PREFERRED_RANGE_MIN = 0;
  private int itsRangeMax = PREFERRED_RANGE_MAX;
  private int itsRangeMin = PREFERRED_RANGE_MIN;
  int itsRange = itsRangeMax-itsRangeMin;
  int itsPixelRange = itsRangeMax-itsRangeMin;
  static Dimension preferredSize = new Dimension(20,BOTTOM_OFFSET+PREFERRED_RANGE_MAX+UP_OFFSET);
  static Dimension currentMinimumSize = new Dimension(20,BOTTOM_OFFSET+PREFERRED_RANGE_MAX+UP_OFFSET);
  boolean itsMovingThrottle = false;
  int itsDelta = 0;
  float itsStep =  itsRange/itsPixelRange;

  public ErmesObjSlider(){
    super();
    transmission_buffer = new int[TRUST];
  }


  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    makeCurrentRect(x, y);
    itsThrottle = new ErmesObjThrottle(this, x, y);
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    return true;
  }


  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    super.Init(theSketchPad,  theFtsObject); //new
    makeCurrentRect(theFtsObject);
    itsThrottle = new ErmesObjThrottle(this, getItsX(), getItsY());   
    //oldsuper.Init(theSketchPad,  theFtsObject); 
    {
      Integer aInteger = (Integer)theFtsObject.get("minValue");
      setMinValue(aInteger.intValue());
      aInteger = (Integer)theFtsObject.get("maxValue");
      setMaxValue(aInteger.intValue());
      itsRange = itsRangeMax-itsRangeMin;
      itsStep = (float)itsRange/itsPixelRange;
      aInteger = (Integer)theFtsObject.get("value");
      itsInteger = aInteger.intValue();
    }
    Resize(0,0);
    return true;
  }

  public void setMinValue(int theValue) {
    itsRangeMin = theValue;
    itsFtsObject.put("minValue", itsRangeMin);
  }

  public int getMinValue() {
    return itsRangeMin;
  }

  public void setMaxValue(int theValue) {
    itsRangeMax = theValue;
    itsFtsObject.put("maxValue", itsRangeMax);
  }

  public int getMaxValue() {
    return itsRangeMax;
  }

  public void Resize(int theDeltaH, int theDeltaV){
    super.Resize(theDeltaH, theDeltaV);
    itsPixelRange = getItsHeight()-(UP_OFFSET+BOTTOM_OFFSET);
    
    itsStep = (float)itsRange/itsPixelRange;
    itsThrottle.Resize(itsThrottle.getPreferredSize().width+theDeltaH, itsThrottle.getPreferredSize().height);
    itsThrottle.MoveAbsolute(itsThrottle.itsX, (int)(getItsY()+getItsHeight() - BOTTOM_OFFSET-2 -itsInteger/itsStep));
  }


  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "slider");
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
    setMaxValue(theMaxInt.intValue());
    setMinValue(theMinInt.intValue());
    itsRange = itsRangeMax-itsRangeMin;
    itsStep = (float)itsRange/itsPixelRange;

    int temp = theCurrentInt.intValue();        
    int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
    itsInteger = clippedValue;
    sendValue(new Integer(itsInteger));
    //Double Paint??
  }



  //--------------------------------------------------------
  // FtsValueChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------
  protected void FtsValueChanged(Object value) {
    
    if (itsMovingThrottle) return;
    int temp = ((Integer) value).intValue();
    
    /*last_value = temp;
    if (receiving_index < transmission_index && temp == transmission_buffer[receiving_index]) {
      //Ok, the buffer is working, the value is the one we sent...
      receiving_index += 1;
      if (receiving_index == transmission_index) {
	
	receiving_index = 0;
	transmission_index = 0;
      }
      return;
    }
    else {
      // we're receiving other values, discard the buffer
      
      transmission_index = 0;
      receiving_index = 0;*/
      if (itsInteger != temp) {
	itsInteger = temp;
	int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
	clippedValue-=itsRangeMin;
	
	if (itsThrottle != null) {
	  itsThrottle.Move(itsThrottle.itsX, (int)(getItsY()+getItsHeight()-BOTTOM_OFFSET-2-clippedValue/itsStep));
	}
	if (itsSketchPad.itsRunMode) Paint_movedThrottle(itsSketchPad.getGraphics());
	else Paint_specific(itsSketchPad.getGraphics());
      }
      //}		
  }


  private void SetSliderDialog(){
    Point aPoint = GetSketchWindow().getLocation();
    if (itsSliderDialog == null) itsSliderDialog = new ErmesObjSliderDialog(MaxWindowManager.getTopFrame());
    itsSliderDialog.setLocation(aPoint.x + getItsX(),aPoint.y + getItsY() - 25);
    itsSliderDialog.ReInit(String.valueOf(itsRangeMax), String.valueOf(itsRangeMin), String.valueOf(itsInteger), this, itsSketchPad.GetSketchWindow());
    itsSliderDialog.setVisible(true);
  }



  public boolean MouseDown_specific(MouseEvent evt, int x, int y){
    if(evt.getClickCount()>1) {
      SetSliderDialog();
      return true;
    }
    if(itsSketchPad.itsRunMode || evt.isControlDown()){
      
      if(IsInThrottle(x,y)){
	itsMovingThrottle = true;
	return true;
      }
      else{
	if(getItsY()+getItsHeight()-BOTTOM_OFFSET>=y && getItsY()+UP_OFFSET<y) {
	  //compute the value and send to FTS
	  itsInteger = (int)(((getItsY()+getItsHeight())-y-BOTTOM_OFFSET)*itsStep);
	  //Trust(itsInteger);
	  sendValue(new Integer(itsInteger));

	  itsThrottle.Move(itsThrottle.itsX, y-2);
	  itsMovingThrottle = true;

	  Paint_specific(itsSketchPad.getGraphics());
	}
	else if(getItsY()+getItsHeight()-BOTTOM_OFFSET<y){
	  itsInteger = itsRangeMin/*0*/;
	  //Trust(itsRangeMin);
	  sendValue(new Integer(itsRangeMin));
	  itsThrottle.Move(itsThrottle.itsX, getItsY()+getItsHeight()-BOTTOM_OFFSET-2);

	  Paint_specific(itsSketchPad.getGraphics());
	}
	else if(getItsY()+UP_OFFSET>=y){
	  //Trust(itsRangeMax);
	  sendValue(new Integer(itsRangeMax));
	  itsInteger = itsRangeMax;
	  itsThrottle.Move(itsThrottle.itsX, getItsY()+UP_OFFSET-2);

	  Paint_specific(itsSketchPad.getGraphics());
	}
	return true;
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return false;
  }

  private void sendValue(Integer theValue) {
    
    itsFtsObject.put("value", theValue, this);//don't call me back
  }

  public boolean MouseDrag_specific(MouseEvent evt,int x, int y){
    if((itsSketchPad.itsRunMode || evt.isControlDown())
       &&(itsMovingThrottle == true)){
      if(getItsY()+getItsHeight()-BOTTOM_OFFSET>=y && getItsY()+UP_OFFSET<=y) {
	//compute the value and send to FTS
	if (itsInteger == (int)(((getItsY()+getItsHeight())-y-BOTTOM_OFFSET)*itsStep)) 
	    return false;
	
	itsInteger = (int)(((getItsY()+getItsHeight())-y-BOTTOM_OFFSET)*itsStep);
	//Trust(itsInteger+itsRangeMin);
	sendValue(new Integer(itsInteger+itsRangeMin));
	itsThrottle.Move(itsThrottle.itsX, y-2);
	Paint_specific(itsSketchPad.getGraphics());
      }
      else if(getItsY()+getItsHeight()-BOTTOM_OFFSET<y){
	
	//Trust(itsRangeMin);
	sendValue(new Integer(itsRangeMin));
	itsInteger = itsRangeMin;
	itsThrottle.Move(itsThrottle.itsX, getItsY()+getItsHeight()-BOTTOM_OFFSET-2);

	Paint_specific(itsSketchPad.getGraphics());
      }
      else if(getItsY()+UP_OFFSET>y){
	
	//Trust(itsRangeMax);
	sendValue(new Integer(itsRangeMax));
	itsInteger = itsRangeMax;
	itsThrottle.Move(itsThrottle.itsX, getItsY()+UP_OFFSET-2);

	Paint_specific(itsSketchPad.getGraphics());
      }
      return true;
    }
    return false;
  }



  public boolean MouseUp(MouseEvent evt,int x, int y){
    if(itsSketchPad.itsRunMode || evt.isControlDown() || itsMovingThrottle){

      itsMovingThrottle = false;
      itsFtsObject.put("value", itsInteger);
      Fts.getServer().syncToFts();

      return true;
    }

    else return super.MouseUp(evt, x, y);
  }

  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY) {
    if((getItsWidth()+theDeltaX < getMinimumSize().width)||
       (getItsHeight()+theDeltaY < getMinimumSize().height))
      return false;
    else return true;
  }
  
  public boolean IsInThrottle(int theX, int theY){
    Rectangle aRect = itsThrottle.Bounds();
    return aRect.contains(theX,theY);
  }
  
  public boolean NeedPropertyHandler(){
    return true;
  }

  public boolean isUIController() {
    return true;
  }

  int count = 0;
  public void Paint_movedThrottle(Graphics g) {
    itsThrottle.eraseAndPaint(g);
  }

  public void Paint_specific(Graphics g) {
    if (g == null) return; 
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    g.fill3DRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2, true);
    g.setColor(Color.black);
    g.drawRect(getItsX()+0,getItsY()+ 0, getItsWidth()-1, getItsHeight()-1);
    
    //paint dragBox
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    //paint throttle
    itsThrottle.paintNoErase(g);
  }

  public void MoveBy(int theDeltaH, int theDeltaV) {
    super.MoveBy(theDeltaH, theDeltaV);
    itsThrottle.MoveByAbsolute(theDeltaH, theDeltaV);
  }

  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    currentMinimumSize.width =15;/*20;*/
    currentMinimumSize.height = 30;
    return currentMinimumSize;
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }

  /*  void Trust (int theInt) {
    if (transmission_index == TRUST) {
      //we sent more then TRUST values without acknowledge...
      //write a message? FTS, are you there?
      
      itsInteger = last_value;
      DoublePaint();
    }
    else {
      transmission_buffer[transmission_index++] = theInt;
    }
  }*/
}
