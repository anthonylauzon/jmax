package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

/**
 * The "integer box" graphic object.
 */
class ErmesObjInt extends ErmesObject implements FtsPropertyHandler{
  
  ErmesObjInlet itsInlet;
  ErmesObjOutlet itsOutlet;
  int itsInteger = 0;
  
  static ErmesObjIntegerDialog itsIntegerDialog = null;
  static final int TRUST=10;		//how many transmitted values we trust?
  int transmission_buffer[];
  int transmission_index = 0;
  int receiving_index = 0;
  int last_value = 0;
  
  int DEFAULT_WIDTH = 40;
  int DEFAULT_HEIGHT = 15;
  int DEFAULT_VISIBLE_DIGIT = 3;
  int itsStartingY, itsFirstY;
  boolean firstClick = true;
  Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  Dimension minimumSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjInt(){
    super();
    transmission_buffer = new int[TRUST];
  }
	
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    
    DEFAULT_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.sketchFont).getHeight();
    DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0")*DEFAULT_VISIBLE_DIGIT+theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("..");
    preferredSize.height = DEFAULT_HEIGHT+4;
    preferredSize.width = DEFAULT_WIDTH+17;
    super.Init(theSketchPad, x, y, theString);
    itsFtsObject.watch("value", this);
    return true;
  }

   public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
     super.Init(theSketchPad,  theFtsObject);
     itsFtsObject.watch("value", this);

     itsInteger = ((Integer)theFtsObject.get("value")).intValue();
     DEFAULT_HEIGHT = itsFontMetrics.getHeight();
     DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");
     if(getItsHeight()<DEFAULT_HEIGHT+4) {
       preferredSize.height = DEFAULT_HEIGHT+4;
       resizeBy(0, getPreferredSize().height - getItsHeight());
     }
     if(getItsWidth()<DEFAULT_WIDTH+17){
       preferredSize.width = DEFAULT_WIDTH+17;
       setItsWidth(preferredSize.width);
     }
     return true;
   }
	
  //--------------------------------------------------------
  // makeFtsObject, redefineFtsObject
  //--------------------------------------------------------
  
  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "intbox");
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }

  }

  public void redefineFtsObject()
  {
    // gint do not redefine themselves
  }
  
  //--------------------------------------------------------
  // propertyChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------
  public void propertyChanged(FtsObject obj, String name, Object value) {
    
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
	Paint_specific(itsSketchPad.getGraphics());
      }
    }		
  }

  public void FromDialogValueChanged(int theInt){
    itsInteger = theInt;

    itsFtsObject.put("value", theInt);
    DoublePaint();
  }
  
  void ResizeToNewFont(Font theFont){
      resizeBy(17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..")-getItsWidth(),itsFontMetrics.getHeight()+4-getItsHeight());
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    
    if((aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))&&(aHeight<itsFontMetrics.getHeight()+4)) {
      aWidth = getMinimumSize().width;
      aHeight = getMinimumSize().height;
    }else{
      if(aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))
	aWidth = aHeight/2+17+itsFontMetrics.stringWidth("0");
      
      if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    }
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }

  public boolean canResizeBy(int theDeltaX, int theDeltaY){
    if((getItsWidth()+theDeltaX < getItsHeight()/2 +17+itsFontMetrics.stringWidth("0"))||(getItsHeight()+theDeltaY<itsFontMetrics.getHeight() + 4))
      return false;
    else return true;
  }
  
  public void RestoreDimensions(){
    int aHeight, aWidth;
    aHeight = itsFontMetrics.getHeight()+4;
    aWidth = 17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");

    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
    itsSketchPad.repaint();
  }
	
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    
    /*if(evt.getClickCount()>1) {
      inspect();
      return true;
    }*/
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      itsFirstY = y;
      if (firstClick) {
	itsStartingY = itsInteger;
	firstClick = false;
      }
      else itsStartingY = itsInteger;
      itsFtsObject.put("value", new Integer(itsInteger));
      Trust(itsInteger);
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
  
  public boolean inspectorAlreadyOpen() {
    return (itsIntegerDialog != null && itsIntegerDialog.isVisible());
  }

  public void openInspector() {
    Point aPoint = GetSketchWindow().getLocation();
    if (itsIntegerDialog == null) itsIntegerDialog = new ErmesObjIntegerDialog(MaxWindowManager.getTopFrame());
    itsIntegerDialog.setLocation(aPoint.x + getItsX(),aPoint.y + getItsY());
    itsIntegerDialog.ReInit(String.valueOf(itsInteger), this, itsSketchPad.GetSketchWindow());
    //itsIntegerDialog.setVisible(true);
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
	
  //--------------------------------------------------------
  //  mouseUp
  //--------------------------------------------------------
  public boolean MouseUp(MouseEvent evt,int x, int y) {
    if(itsSketchPad.itsRunMode || evt.isControlDown()){
      Fts.getServer().syncToFts();

      DoublePaint();
      return true;
    }
    else return super.MouseUp(evt, x, y);
  }
	
  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  public boolean MouseDrag_specific(MouseEvent evt,int x, int y) {

    if(itsSketchPad.itsRunMode || evt.isControlDown()){
      itsInteger = itsStartingY+(itsFirstY-y);
      itsFtsObject.put("value", new Integer(itsInteger));
      DoublePaint();
      Trust(itsInteger);
      return true;
    }
    else return false;
  }

  public boolean NeedPropertyHandler(){
    return true;
  }
  
  public boolean isUIController() {
    return true;
  }

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {

    if (g == null) return;
    //draw the white area
    int xWhitePoints[] = {getItsX()+3, getItsX()+getItsWidth()-3, getItsX()+getItsWidth()-3, getItsX()+3, getItsX()+getItsHeight()/2+3};
    int yWhitePoints[] = {getItsY()+1, getItsY()+1, getItsY()+getItsHeight()-1,getItsY()+getItsHeight()-1, getItsY()+getItsHeight()/2};
    if(!itsSelected) g.setColor(Color.white);
    else g.setColor(itsUINormalColor);
    g.fillPolygon(xWhitePoints, yWhitePoints, 5);

    //fill the triangle and blue rectangle 
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    
    g.fill3DRect(getItsX()+getItsWidth()-4,getItsY()+1, 3, getItsHeight()-2, true);
    
    int xPoints[]={getItsX()+1, getItsX()+getItsHeight()/2+1, getItsX()+1};
    int yPoints[]={getItsY(), getItsY()+getItsHeight()/2, getItsY()+getItsHeight()-1};
    g.fillPolygon(xPoints, yPoints, 3);

    //draw the outline
    g.setColor(Color.black);
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    
    //draw the triangle
    if(!itsSelected) g.setColor(itsUISelectedColor);
    else g.setColor(Color.black);
    g.drawLine(getItsX()+1,getItsY(),getItsX()+getItsHeight()/2+1,getItsY()+getItsHeight()/2);
    g.drawLine(getItsX()+getItsHeight()/2+1,getItsY()+getItsHeight()/2,getItsX()+1, getItsY()+getItsHeight()-1);
    
    //draw the value
    g.setColor(Color.black);
    String aString = GetVisibleString(String.valueOf(itsInteger));
    g.setFont(getFont());
    g.drawString(aString, getItsX()+getItsHeight()/2+5,getItsY()+itsFontMetrics.getAscent()+(getItsHeight()-itsFontMetrics.getHeight())/2 +1);
    
    //draw the dragbox
    if(!itsSketchPad.itsRunMode)
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  String GetVisibleString(String theString){
    String aString = theString;
    String aString2 = "..";
    int aStringLength = theString.length();
    int aCurrentSpace = getItsWidth()-(getItsHeight()/2+5)-5;
    int aStringWidth = itsFontMetrics.stringWidth(aString);
    if(aStringWidth<aCurrentSpace) return aString;
    while((aCurrentSpace<=aStringWidth)&&(aString.length()>0)){
      aString = aString.substring(0,aString.length()-1);
      aStringWidth = itsFontMetrics.stringWidth(aString);
    }
    if((aStringWidth+itsFontMetrics.stringWidth("..") >= aCurrentSpace)&&(aString.length()>0))
      aString = aString.substring(0,aString.length()-1);
    aString =  aString + aString2;
    return aString;
  }

  //--------------------------------------------------------
  // minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    if(getItsHeight()==0 || getItsWidth() == 0) return minimumSize;
    else return new Dimension(getItsHeight()/2+13+itsFontMetrics.stringWidth("0"),itsFontMetrics.getHeight()+4); 
  }

  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}




