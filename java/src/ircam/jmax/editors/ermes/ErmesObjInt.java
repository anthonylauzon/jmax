package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "integer box" graphic object.
 */
class ErmesObjInt extends ErmesObject {

  ErmesObjInlet itsInlet;
  ErmesObjOutlet itsOutlet;
  int itsInteger = 0;
  static final int TRUST=10;		//how many transmitted values we trust?
  int transmission_buffer[];
  int transmission_index = 0;
  int receiving_index = 0;
  int last_value = 0;
  
  int DEFAULT_WIDTH = 40;
  int DEFAULT_HEIGHT = 20;
  int itsStartingY, itsFirstY;
  boolean firstClick = true;
  Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjInt(){
    super();
    transmission_buffer = new int[TRUST];
    //setLayout(null);
  }
	
  // WARNING!! this Init must set the integer value coming in theString?
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    
    DEFAULT_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.sketchFont).getHeight();
    DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0")*6;
    preferredSize.height = DEFAULT_HEIGHT+4;
    preferredSize.width = DEFAULT_WIDTH+DEFAULT_HEIGHT/2+20;
    super.Init(theSketchPad, x, y, theString);
    return true;
  }

   public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
     super.Init(theSketchPad,  theFtsObject);
    //ca parce-que dans le chargement d'un patch .pat, les Int sont trop petits et
    //le valeur affiche risque de sortir de la boite
     DEFAULT_HEIGHT = itsFontMetrics.getHeight();
     DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*6;
     if(currentRect.height<DEFAULT_HEIGHT+4) {
       preferredSize.height = DEFAULT_HEIGHT+4;
       currentRect.height = preferredSize.height;
     }
     if(currentRect.width<DEFAULT_WIDTH+DEFAULT_HEIGHT/2+20){
       preferredSize.width = DEFAULT_WIDTH+DEFAULT_HEIGHT/2+20;
       currentRect.width = preferredSize.width;
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
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "intbox");
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
      itsInteger = temp;
      DoublePaint();
    }		
  }
  
  /*public boolean ChangeRectInt(){
    String aString; 
    boolean aChange = false;
    aString = String.valueOf(itsInteger);
    int lenght = itsFontMetrics.stringWidth(aString);
    itsSketchPad.RemoveElementRgn(this);
    //if (lenght >= currentRect.width*2/3-2-5){
    if (lenght+currentRect.height/2+20+itsFontMetrics.stringWidth("0")-8 > currentRect.width){
    //while(lenght >= currentRect.width*2/3-2-5){
    while(lenght+currentRect.height/2+20+itsFontMetrics.stringWidth("0")-8 > currentRect.width){
    Resize1(currentRect.width+15, currentRect.height);
    aChange = true;
    }
    }
    else {
    //while(lenght < currentRect.width*2/3-2-20){
    while(lenght+currentRect.height/2+20+itsFontMetrics.stringWidth("0")< currentRect.width-30){
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
    }*/
  
  void ResizeToNewFont(Font theFont){
    if(!itsResized){
      Resize(20+itsFontMetrics.stringWidth("0")*6-currentRect.width,itsFontMetrics.getHeight()+4-currentRect.height);
    }
    else ResizeToText(0,0);
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    
    if(aWidth<currentRect.height/2+20+itsFontMetrics.stringWidth("0")*6)
      aWidth = currentRect.height/2+20+itsFontMetrics.stringWidth("0")*6;

    if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < currentRect.height/2 +20+itsFontMetrics.stringWidth("0")*6)||(currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 4))
      return false;
    else return true;
  }
  
  public void RestoreDimensions(){
    int aHeight, aWidth;
    aHeight = itsFontMetrics.getHeight()+4;
    aWidth = 20+itsFontMetrics.stringWidth("0")*6;
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
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
    /*if (itsSketchPad.itsRunMode) {
      itsStartingY = itsInteger;
      firstClick = true;
      
      String aString = String.valueOf(itsInteger);
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
      else*/ return super.MouseUp(evt,x,y);
  }
	
  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  public boolean MouseDrag(MouseEvent evt,int x, int y) {

    if(itsSketchPad.itsRunMode){
      itsInteger = itsStartingY+(itsFirstY-y);
      itsFtsObject.put("value", new Integer(itsInteger));
      //((FtsInteger) itsFtsActive).setValue(itsInteger);	ENZOOO
      DoublePaint();
      Trust(itsInteger);
      return true;
    }
    else return false;
  }

  public boolean NeedPropertyHandler(){
    return true;
  }
  

  //--------------------------------------------------------
  // ConnectionRequested
  //--------------------------------------------------------
  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    if (!theRequester.IsInlet())
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); 
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
    //draw the white area
    int xWhitePoints[] = {itsX+3, itsX+currentRect.width-3, itsX+currentRect.width-3, itsX+3, itsX+currentRect.height/2+3};
    int yWhitePoints[] = {itsY+1, itsY+1, itsY+currentRect.height-1,itsY+currentRect.height-1, itsY+currentRect.height/2};
    g.setColor(Color.white);
    g.fillPolygon(xWhitePoints, yWhitePoints, 5);

    //fill the triangle and blue rectangle 
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    
    g.fill3DRect(itsX+currentRect.width-4,itsY+1, 3, currentRect.height-2, true);
    
    int xPoints[]={itsX+1, itsX+currentRect.height/2+1, itsX+1};
    int yPoints[]={itsY, itsY+currentRect.height/2, itsY+currentRect.height-1};
    g.fillPolygon(xPoints, yPoints, 3);

    //draw the outline
    g.setColor(Color.black);
    g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
    
    //draw the triangle
    if(!itsSelected) g.setColor(itsUISelectedColor);
    else g.setColor(Color.black);
    g.drawLine(itsX+1,itsY,itsX+currentRect.height/2+1,itsY+currentRect.height/2);
    g.drawLine(itsX+currentRect.height/2+1,itsY+currentRect.height/2,itsX+1, itsY+currentRect.height-1);
    
    //draw the value
    g.setColor(Color.black);
    String aString = String.valueOf(itsInteger);
    String aString2 = "..";
    if(aString.length()>4){
      aString = aString.substring(0,4);
      aString = aString + aString2;
    }
    g.setFont(itsFont);
    g.drawString(aString, itsX+currentRect.height/2+8,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2 +1);
    
    //draw the dragbox
    if(!itsSketchPad.itsRunMode)
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  //--------------------------------------------------------
  // minimumSize
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



