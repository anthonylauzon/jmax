package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The graphic "float box" object.
 */
class ErmesObjFloat extends ErmesObject implements FtsPropertyHandler{
	
  float itsFloat =  (float) 0.;
  static ErmesObjFloatDialog itsFloatDialog = null;
  final int WIDTH_DIFF = 14;
  final int HEIGHT_DIFF = 2;
  int DEFAULT_WIDTH = 50;
  int DEFAULT_HEIGHT = 15;
  int DEFAULT_VISIBLE_DIGIT = 3;
  float itsStartingValue;

  /* values relative to mouse dragging motion */
  float acceleration;
  float velocity;
  float previousVelocity;
  int previousY;

  int itsFirstY;
  boolean firstClick = true;
  Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  Dimension minimumSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public ErmesObjFloat(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }
	
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------
  
  public void Init()
  {
    super.Init();
    itsFtsObject.watch("value", this);

    itsFloat = ((Float)itsFtsObject.get("value")).floatValue();

    DEFAULT_HEIGHT = itsFontMetrics.getHeight();
    DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("...");

    if(getItsHeight()<DEFAULT_HEIGHT+4) {
      preferredSize.height = DEFAULT_HEIGHT+4;
      resizeBy(0, getPreferredSize().height - getItsHeight());
    }

    if(getItsWidth()<DEFAULT_WIDTH+17){
      preferredSize.width = DEFAULT_WIDTH+17;
      setItsWidth(preferredSize.width);
    }
  }
  
  //--------------------------------------------------------
  // propertyChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------

  public void propertyChanged(FtsObject obj, String name, Object value)
  {
    itsFloat = ((Float) value).floatValue();

    Graphics g = itsSketchPad.getGraphics();
    Paint_specific(g);
    g.dispose();
  }
	
  public void FromDialogValueChanged(float theFloat){
    itsFloat = theFloat;

    itsFtsObject.put("value", theFloat);
    DoublePaint();
  }
	
  public String GetFloatString(){
    return String.valueOf(itsFloat);
  }
	
  void ResizeToNewFont(Font theFont) {

    ResizeToText(0,0);
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    
    if((aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..."))&&(aHeight<itsFontMetrics.getHeight()+4)){
      aWidth = getMinimumSize().width;
      aHeight = getMinimumSize().height;
    }else{
      if(aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..."))
	aWidth=aHeight/2+17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("...");
      if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    }
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
	
  public boolean canResizeBy(int theDeltaX, int theDeltaY){
    if((getItsWidth()+theDeltaX < getItsHeight()/2+17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..."))||(getItsHeight()+theDeltaY<itsFontMetrics.getHeight()+4))
      return false;
    else return true;
  }
  
	
  public void RestoreDimensions(){

    int tempWidth = 17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("...");
    int tempHeight = itsFontMetrics.getHeight()+4;
    resizeBy(tempWidth - getItsWidth(), tempHeight - getItsHeight());		
    // itsSketchPad.repaint(); // @@@ BARBOGIO
  }
  
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public void MouseDown_specific(MouseEvent evt,int x, int y) {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;
    itsFirstY = y;
    previousY = y;

    /*if(evt.getClickCount()>1) {
      inspect();
      return true;
    }*/
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      if (firstClick) {
	itsStartingValue = itsFloat;
	firstClick = false;
      }
      else itsStartingValue = itsFloat;
      itsFtsObject.put("value", itsFloat);

      DoublePaint();
    }
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }
	
  public boolean inspectorAlreadyOpen() {
    return (itsFloatDialog != null && itsFloatDialog.isVisible());
  }
  
  public void openInspector() {
    Point aPoint = GetSketchWindow().getLocation();
    if (itsFloatDialog == null) itsFloatDialog = new ErmesObjFloatDialog(MaxWindowManager.getTopFrame());
    itsFloatDialog.setLocation(aPoint.x + getItsX(),aPoint.y + getItsY());
    itsFloatDialog.ReInit(String.valueOf(itsFloat), this, itsSketchPad.GetSketchWindow());
    //itsFloatDialog.setVisible(true);
  }

  //--------------------------------------------------------
  //  mouseUp
  //--------------------------------------------------------
  public boolean MouseUp(MouseEvent evt,int x, int y) {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;
    if(itsSketchPad.itsRunMode){
      itsFtsObject.ask("value");
      Fts.sync();
      DoublePaint();
      return true;
    }
    else return super.MouseUp(evt, x, y);
  }
  
  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  public boolean MouseDrag_specific(MouseEvent evt,int x, int y) {
    previousVelocity = velocity;
    velocity = (previousY-y);
    acceleration = Math.abs(velocity-previousVelocity);
    previousY=y;

    if(itsSketchPad.itsRunMode || evt.isControlDown()){
      
      float increment;
      if (velocity*previousVelocity > 0)
	increment = (velocity/1000)+((velocity>0)?acceleration:-acceleration)/10;
      else increment = velocity/1000;
     
      if (evt.isShiftDown()) increment*=10;
      
      itsFloat+=increment;
      
      itsFtsObject.put("value", new Float(itsFloat));
      DoublePaint();

      return true;
    }
    else return false;
  }

  public boolean isUIController() {
    return true;
  }

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    //draw the white area
    if (g == null) return;
    int xWhitePoints[] = {getItsX()+3, getItsX()+getItsWidth()-3, getItsX()+getItsWidth()-3, getItsX()+3, getItsX()+getItsHeight()/2+3};
    int yWhitePoints[] = {getItsY()+1, getItsY()+1, getItsY()+getItsHeight()-1,getItsY()+getItsHeight()-1, getItsY()+getItsHeight()/2};
    if(!itsSelected) g.setColor(Color.white);
    else g.setColor(itsUINormalColor);
    g.fillPolygon(xWhitePoints, yWhitePoints, 5);

    //fill the triangle
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor/*Color.black*/);
    
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
    g.drawLine(getItsX()+getItsHeight()/2+1, getItsY()+getItsHeight()/2, getItsX()+1, getItsY()+getItsHeight()-1);
    
    //draw the value
    String aString;
    //String aString2 = ".."; 
    if (itsFloat != 0) aString = GetVisibleString(String.valueOf(itsFloat));
    else aString = "0.0";
    g.setFont(getFont());
    g.setColor(Color.black);
    
    g.drawString(aString, getItsX()+getItsHeight()/2+5,getItsY()+itsFontMetrics.getAscent()+(getItsHeight()-itsFontMetrics.getHeight())/2+1);

    if(!itsSketchPad.itsRunMode)
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  String GetVisibleString(String theString){
    String aString = theString;
    String aString2 = "..";
    int aStringLength = theString.length();
    int aCurrentSpace = getItsWidth()-(getItsHeight()/2+8)-3;
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
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    if(getItsHeight() == 0 || getItsWidth() == 0)return minimumSize;
    else return new Dimension(itsFontMetrics.stringWidth("0.0")+itsFontMetrics.getHeight()/2+15,itsFontMetrics.getHeight()+4);
  }
  
  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }

}










