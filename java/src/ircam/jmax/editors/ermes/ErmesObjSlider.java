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
  int itsRangeMax = PREFERRED_RANGE_MAX;
  int itsRangeMin = PREFERRED_RANGE_MIN;
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
    makeCurrentRect(theFtsObject);
    itsThrottle = new ErmesObjThrottle(this, itsX, itsY);   
    super.Init(theSketchPad,  theFtsObject); 
    {
      Integer aInteger = (Integer)theFtsObject.get("minValue");
      itsRangeMin = aInteger.intValue();
      aInteger = (Integer)theFtsObject.get("maxValue");
      itsRangeMax = aInteger.intValue();
      itsRange = itsRangeMax-itsRangeMin;
      itsStep = (float)itsRange/itsPixelRange;
      aInteger = (Integer)theFtsObject.get("value");
      itsInteger = aInteger.intValue();
    }
    Resize(0,0);
    return true;
  }


  public void Resize(int theDeltaH, int theDeltaV){
    super.Resize(theDeltaH, theDeltaV);
    itsPixelRange = currentRect.height-(UP_OFFSET+BOTTOM_OFFSET);
    
    itsStep = (float)itsRange/itsPixelRange;
    itsThrottle.Resize(itsThrottle.getPreferredSize().width+theDeltaH, itsThrottle.getPreferredSize().height);
    itsThrottle.Move(itsThrottle.itsX, (int)(itsY+currentRect.height - BOTTOM_OFFSET-2 -itsInteger/itsStep));
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
    itsRangeMax = theMaxInt.intValue();
    itsRangeMin = theMinInt.intValue();
    itsRange = itsRangeMax-itsRangeMin;
    itsStep = (float)itsRange/itsPixelRange;

    int temp = theCurrentInt.intValue();        
    int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
    itsInteger = clippedValue;
    itsFtsObject.put("value", new Integer(itsInteger));
    //Double Paint??
  }



  //--------------------------------------------------------
  // FtsValueChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------
  protected void FtsValueChanged(Object value) {
    
    int temp = ((Integer) value).intValue();
    
    last_value = temp;
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
      // we're receiving other values
      transmission_index = 0;
      receiving_index = 0;
      if (itsInteger != temp) {
	itsInteger = temp;//era solo questo
	int clippedValue = (temp<itsRangeMin)?itsRangeMin:((temp>=itsRangeMax)?itsRangeMax:temp);
	clippedValue-=itsRangeMin;
	
	itsInteger = temp;
	if (itsThrottle != null) {
	  itsThrottle.Move(itsThrottle.itsX, (int)(itsY+currentRect.height-BOTTOM_OFFSET-2-clippedValue/itsStep));
	}
	Paint_specific(itsSketchPad.getGraphics());
      }
    }		
  }


  public void putOtherProperties(FtsObject theFObject) {
    theFObject.put("minValue", new Integer(itsRangeMin));
    theFObject.put("maxValue", new Integer(itsRangeMax));
  }


  private void SetSliderDialog(){
    Point aPoint = GetSketchWindow().getLocation();
    if (itsSliderDialog == null) itsSliderDialog = new ErmesObjSliderDialog(MaxWindowManager.getTopFrame());
    itsSliderDialog.setLocation(aPoint.x + itsX,aPoint.y + itsY - 25);
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
	if(itsY+currentRect.height-BOTTOM_OFFSET>=y && itsY+UP_OFFSET<y) {
	  //compute the value and send to FTS
	  itsInteger = (int)(((itsY+currentRect.height)-y-BOTTOM_OFFSET)*itsStep);
	  itsFtsObject.put("value", new Integer(itsInteger/*+itsRangeMin*/));
	  Trust(itsInteger);

	  itsThrottle.Move(itsThrottle.itsX, y-2);
	  itsMovingThrottle = true;

	  Paint_specific(itsSketchPad.getGraphics());
	}
	else if(itsY+currentRect.height-BOTTOM_OFFSET<y){
	  itsInteger = itsRangeMin/*0*/;
	  itsFtsObject.put("value", new Integer(itsRangeMin));
	  Trust(itsInteger);
	  itsThrottle.Move(itsThrottle.itsX, itsY+currentRect.height-BOTTOM_OFFSET-2);

	  Paint_specific(itsSketchPad.getGraphics());
	}
	else if(itsY+UP_OFFSET>=y){
	  itsFtsObject.put("value", new Integer(itsRangeMax));
	  itsInteger = itsRangeMax;
	  Trust(itsRangeMax);
	  itsThrottle.Move(itsThrottle.itsX, itsY+UP_OFFSET-2);

	  Paint_specific(itsSketchPad.getGraphics());
	}
	return true;
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return false;
  }


  public boolean MouseDrag_specific(MouseEvent evt,int x, int y){
    if((itsSketchPad.itsRunMode || evt.isControlDown())
       &&(itsMovingThrottle == true)){
      if(itsY+currentRect.height-BOTTOM_OFFSET>=y && itsY+UP_OFFSET<y) {
	//compute the value and send to FTS
	itsInteger = (int)(((itsY+currentRect.height)-y-BOTTOM_OFFSET)*itsStep);
	itsFtsObject.put("value", new Integer(itsInteger+itsRangeMin));
	Trust(itsInteger+itsRangeMin);
	itsThrottle.Move(itsThrottle.itsX, y-2);
	Paint_specific(itsSketchPad.getGraphics());
      }
      else if(itsY+currentRect.height-BOTTOM_OFFSET<y){
	itsFtsObject.put("value", new Integer(itsRangeMin));
	itsInteger = itsRangeMin;
	Trust(itsInteger);
	itsThrottle.Move(itsThrottle.itsX, itsY+currentRect.height-BOTTOM_OFFSET-2);

	Paint_specific(itsSketchPad.getGraphics());
      }
      else if(itsY+UP_OFFSET>=y){
	itsFtsObject.put("value", new Integer(itsRangeMax));
	itsInteger = itsRangeMax;
	Trust(itsRangeMax);
	itsThrottle.Move(itsThrottle.itsX, itsY+UP_OFFSET-2);

	Paint_specific(itsSketchPad.getGraphics());
      }
      return true;
    }
    return false;
  }



  public boolean MouseUp(MouseEvent evt,int x, int y){
    if(itsSketchPad.itsRunMode || evt.isControlDown()){

      itsMovingThrottle = false;
      Fts.getServer().syncToFts();
      //Paint_specific(itsSketchPad.getGraphics());
      return true;
    }

    else return super.MouseUp(evt, x, y);
  }

  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY) {
    return true;
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
  public void Paint_specific(Graphics g) {
    if (g == null) return; 
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
    currentMinimumSize.width =15;/*20;*/
    currentMinimumSize.height = 30;
    return currentMinimumSize;
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }

  void Trust (int theInt) {
    if (transmission_index == TRUST) {
      //we sent more then TRUST values without acknowledge...
      //write a message? FTS, are you there?
      itsInteger = last_value;
      DoublePaint();
    }
    else {
      transmission_buffer[transmission_index++] = theInt;
    }
  }
}
