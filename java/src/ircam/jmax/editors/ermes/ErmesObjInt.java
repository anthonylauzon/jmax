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
  
  static Frame itsFalseFrame = new Frame();
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
    //setLayout(null);
  }
	
  // WARNING!! this Init must set the integer value coming in theString?
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    
    DEFAULT_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.sketchFont).getHeight();
    DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0")*DEFAULT_VISIBLE_DIGIT+theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("..");
    preferredSize.height = DEFAULT_HEIGHT+4;
    preferredSize.width = DEFAULT_WIDTH+17;
    super.Init(theSketchPad, x, y, theString);
    return true;
  }

   public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
     super.Init(theSketchPad,  theFtsObject);
     //ca parce-que dans le chargement d'un patch .pat, les Int sont trop petits et
    //le valeur affiche risque de sortir de la boite
     DEFAULT_HEIGHT = itsFontMetrics.getHeight();
     DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");
     if(currentRect.height<DEFAULT_HEIGHT+4) {
       preferredSize.height = DEFAULT_HEIGHT+4;
       Resize(0, getPreferredSize().height - currentRect.height);
     }
     if(currentRect.width<DEFAULT_WIDTH+17){
       preferredSize.width = DEFAULT_WIDTH+17;
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

  public void FromDialogValueChanged(Integer theInt){
    itsInteger = theInt.intValue();

    itsFtsObject.put("value", theInt);
    DoublePaint();
  }
  
  void ResizeToNewFont(Font theFont){
    //#@!if(!itsResized){
      Resize(17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..")-currentRect.width,itsFontMetrics.getHeight()+4-currentRect.height);
      //#@!}
      //#@!else ResizeToText(0,0);
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    
    if((aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))&&(aHeight<itsFontMetrics.getHeight()+4)) {
      aWidth = getMinimumSize().width;
      aHeight = getMinimumSize().height;
    }else{
      if(aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))
	aWidth = aHeight/2+17+itsFontMetrics.stringWidth("0");
      
      if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    }
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }

  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < currentRect.height/2 +17+itsFontMetrics.stringWidth("0"))||(currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 4))
      return false;
    else return true;
  }
  
  public void RestoreDimensions(){
    int aHeight, aWidth;
    aHeight = itsFontMetrics.getHeight()+4;
    aWidth = 17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");

    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
    itsSketchPad.repaint();
  }
	
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    
    if(evt.getClickCount()>1) {
      Point aPoint = GetSketchWindow().getLocation();
    if (itsIntegerDialog == null) itsIntegerDialog = new ErmesObjIntegerDialog(itsFalseFrame);
      itsIntegerDialog.setLocation(aPoint.x + itsX,aPoint.y + itsY);
      itsIntegerDialog.ReInit(String.valueOf(itsInteger), this, itsSketchPad.GetSketchWindow());
      itsIntegerDialog.setVisible(true);
      return true;
    }
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
    if(itsSketchPad.itsRunMode){
      FtsServer.getServer().syncToFts();
      //itsMovingThrottle = false;
      DoublePaint();
      return true;
    }
    else return super.MouseUp(evt, x, y);
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
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    //draw the white area
    int xWhitePoints[] = {itsX+3, itsX+currentRect.width-3, itsX+currentRect.width-3, itsX+3, itsX+currentRect.height/2+3};
    int yWhitePoints[] = {itsY+1, itsY+1, itsY+currentRect.height-1,itsY+currentRect.height-1, itsY+currentRect.height/2};
    if(!itsSelected) g.setColor(Color.white);
    else g.setColor(itsUINormalColor);
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
    String aString = GetVisibleString(String.valueOf(itsInteger));
    g.setFont(itsFont);
    g.drawString(aString, itsX+currentRect.height/2+5,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2 +1);
    
    //draw the dragbox
    if(!itsSketchPad.itsRunMode)
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  String GetVisibleString(String theString){
    String aString = theString;
    String aString2 = "..";
    int aStringLength = theString.length();
    int aCurrentSpace = currentRect.width-(currentRect.height/2+5)-5;
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
    if(currentRect==null) return minimumSize;
    else return new Dimension(currentRect.height/2+13+itsFontMetrics.stringWidth("0"),itsFontMetrics.getHeight()+4); 
  }

  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}




