package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The graphic "float box" object.
 */
class ErmesObjFloat extends ErmesObject {
	
  static ErmesObjFloatDialog itsFloatDialog = null;
  float itsFloat =  (float) 0.;
  final int WIDTH_DIFF = 14;
  final int HEIGHT_DIFF = 2;
  int DEFAULT_WIDTH = 50;
  int DEFAULT_HEIGHT = 20;
  float itsStartingValue;
  int itsFirstY;
  boolean firstClick = true;
  boolean fastMode = true;
  Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
	
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjFloat(){
    super();
  }
	
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    
    DEFAULT_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.sketchFont).getHeight();
    DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0.0");
    preferredSize.height = DEFAULT_HEIGHT+10;
    preferredSize.width = DEFAULT_WIDTH+DEFAULT_WIDTH;//l'errore e' qui, deve essere proporzionale alla grandezza
    if(itsFloatDialog == null) itsFloatDialog = new ErmesObjFloatDialog(theSketchPad.GetSketchWindow(), this);
    super.Init(theSketchPad, x, y, theString);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsGraphicDescription theFtsGraphic,FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsGraphic, theFtsObject);
    if(itsFloatDialog == null) itsFloatDialog = new ErmesObjFloatDialog(theSketchPad.GetSketchWindow(), this);
    return true;
  }
  
  //--------------------------------------------------------
  // GetArgs
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "floatbox", (Vector) null);    
  }

  public void redefineFtsObject()
  {
    // ErmesObjFloat do not redefine itself
  }
  
  //--------------------------------------------------------
  // FtsValueChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------
  protected void FtsValueChanged(Object value) {
    
    itsFloat = ((Float) value).floatValue();
    if (ChangeRectFloat()) itsSketchPad.repaint();
    else DoublePaint();
    
  }
	
  public void FromDialogValueChanged(Float theFloat){
    itsFloat = theFloat.floatValue();
    itsFtsObject.putProperty("value", theFloat);
    if (ChangeRectFloat()) itsSketchPad.repaint();
    else DoublePaint();
  }
	
  public String GetFloatString(){
    return String.valueOf(itsFloat);
  }
	
  public boolean ChangeRectFloat(){
    String aString; 
    boolean aChange = false;
    if (itsFloat != 0) aString = String.valueOf(itsFloat);
    else aString = "0.0";
    int lenght = itsFontMetrics.stringWidth(aString);
    itsSketchPad.RemoveElementRgn(this);
    if (lenght >= currentRect.width*2/3-2/*-16*/-5){
      while(lenght >= currentRect.width*2/3-2/*-16*/-5){
	Resize1(currentRect.width+15, currentRect.height);
	aChange = true;
      }
    }
    else {
      while(lenght < currentRect.width*2/3-2-/*5*/20){
	Resize1(currentRect.width-15, currentRect.height);
	aChange = true;
      }
    }
    itsSketchPad.SaveOneElementRgn(this);
    if(aChange) {
      aChange = false;
      return true;
    }
    else return false;
  }
	
  void ResizeToNewFont(Font theFont) {
    //correct width: stringWidth + trianglewidth + font-dependent offset;
    if(!itsResized){
      int tempWidth = (int) ((itsFontMetrics.stringWidth(String.valueOf(itsFloat)) +
			      15)*3);
      int tempHeight = itsFontMetrics.getHeight() + 10;
      Resize(tempWidth - currentRect.width, tempHeight - currentRect.height);
    }
    else ResizeToText(0,0);
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<(int)(itsFontMetrics.stringWidth(String.valueOf(itsFloat)) + 15 + itsFontMetrics.getMaxAdvance())*/*3*/2 ) 
      aWidth = (int)(itsFontMetrics.stringWidth(String.valueOf(itsFloat)) + 15 +itsFontMetrics.getMaxAdvance())*/*3*/2;
    if(aHeight<itsFontMetrics.getHeight() + 10) 
      aHeight = itsFontMetrics.getHeight() + 10;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < (int)(itsFontMetrics.stringWidth(String.valueOf(itsFloat)) + 15)*/*3*/2+itsFontMetrics.getMaxAdvance())||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 10))
      return false;
    else return true;
  }
  
	
  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    int tempWidth = (int) ((itsFontMetrics.stringWidth(String.valueOf(itsFloat)) +15)*3);
    int tempHeight = itsFontMetrics.getHeight() + 10;
    Resize(tempWidth - currentRect.width, tempHeight - currentRect.height);		
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }
  
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      itsFirstY = y;
      if (firstClick) {
	itsStartingValue = itsFloat;
	firstClick = false;
      }
      itsFtsObject.putProperty("value", new Float(itsFloat));
      DoublePaint();
      if(evt.getClickCount()>1) fastMode = true;
      else fastMode = false;	//ENZOOOOOO!
    }
    else if(evt.getClickCount()>1) {
      Point aPoint = GetSketchWindow().getLocation();
      itsFloatDialog.setLocation(aPoint.x + itsX,aPoint.y + itsY - 25);
      itsFloatDialog.ReInit(String.valueOf(itsFloat), this, GetSketchWindow());
      itsFloatDialog.setVisible(true);
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
	
  //--------------------------------------------------------
  //  mouseUp
  //--------------------------------------------------------
  public boolean MouseUp(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      itsStartingValue = itsFloat;
      firstClick = true;
      
      String aString; 
      if (itsFloat != 0) aString = String.valueOf(itsFloat);
      else aString = "0.0";
      int lenght = itsFontMetrics.stringWidth(aString);
      if(!itsResized){
	if(lenght<currentRect.width*2/3-2-20){
	  itsSketchPad.RemoveElementRgn(this);
	  while(lenght<currentRect.width*2/3-2-20){
	    Resize1(currentRect.width-10, currentRect.height);
	  }
	  itsSketchPad.SaveOneElementRgn(this);
	  itsSketchPad.repaint();//???????
	}
      }
      return true;
    }
    return super.MouseUp(evt,x,y);
  }
  
  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  public boolean MouseDrag(MouseEvent evt,int x, int y) {
    
    if(itsSketchPad.itsRunMode){
      if (fastMode)
	itsFloat = (float)(itsStartingValue+ itsFirstY-y);
      else itsFloat = (float)(itsStartingValue+ (float)(itsFirstY-y)/1000.0);
      itsFtsObject.putProperty("value", new Float(itsFloat));
      DoublePaint();
      //((FtsFloat) itsFtsActive).setValue(itsFloat);	//ENZOOOOO!
      return true;
    }
    else return false;
  }
	
  //--------------------------------------------------------
  // ConnectionRequested
  //--------------------------------------------------------
  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    if (!theRequester.IsInlet())
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); // then, is it's an inlet
  }
	
  //--------------------------------------------------------
  // ConnectionAbort
  //--------------------------------------------------------
  public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
    theRequester.ChangeState(false, theRequester.connected);
    itsSketchPad.ResetConnect();
    return true;	
  }
	
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    
    //draw the box
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    
    //draw the white area				
    g.setColor(Color.white);
    g.fillRect(itsX+4,itsY+ 4, currentRect.width-8, currentRect.height-8);
    
    //draw the outline
    g.setColor(Color.black);
    g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
    
    //draw the triangle
    g.drawLine(itsX+4,itsY+4,itsX+currentRect.width/4,itsY+currentRect.height/2);
    g.drawLine(itsX+currentRect.width/4, itsY+currentRect.height/2, itsX+4, itsY+currentRect.height-4);
    
    //draw the dot
    //g.fillRect(12,currentRect.height-8, 2, 2);
    
    //draw the value
    String aString;
    String aString2 = ".."; 
    if (itsFloat != 0) {
      aString = String.valueOf(itsFloat);
      int aTemp = aString.indexOf(".");
      if(aString.length() - aTemp>4) {
	aString = aString.substring(0,aTemp+4);
	aString = aString + aString2;
      }
    }
    else aString = "0.0";
    g.setFont(itsFont);
    g.drawString(aString, itsX+currentRect.width/3+20/DEFAULT_WIDTH,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2);
    
    //draw the drag box
    g.setColor(Color.black);
    g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize();
  }
  
  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }

}










