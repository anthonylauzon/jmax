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
    DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0")*6+theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("...");
    preferredSize.height = DEFAULT_HEIGHT+4;
    preferredSize.width = DEFAULT_WIDTH/*+DEFAULT_HEIGHT/2*/+20;
    if(itsFloatDialog == null) itsFloatDialog = new ErmesObjFloatDialog(theSketchPad.GetSketchWindow(), this);
    super.Init(theSketchPad, x, y, theString);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad,FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    if(itsFloatDialog == null) itsFloatDialog = new ErmesObjFloatDialog(theSketchPad.GetSketchWindow(), this);
    //ca parce-que dans le chargement d'un patch .pat, les Int sont trop petits et
    //le valeur affiche risque de sortir de la boite
    DEFAULT_HEIGHT = itsFontMetrics.getHeight();
    DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("...");
    if(currentRect.height<DEFAULT_HEIGHT+4) {
      preferredSize.height = DEFAULT_HEIGHT+4;
      currentRect.height = preferredSize.height;
    }
    if(currentRect.width<DEFAULT_WIDTH/*+DEFAULT_HEIGHT/2*/+20){
      preferredSize.width = DEFAULT_WIDTH/*+DEFAULT_HEIGHT/2*/+20;
      currentRect.width = preferredSize.width;
    }
    return true;
  }
  
  //--------------------------------------------------------
  // GetArgs
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "floatbox");
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }

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
    DoublePaint();
  }
	
  public void FromDialogValueChanged(Float theFloat){
    itsFloat = theFloat.floatValue();

    itsFtsObject.put("value", theFloat);
    DoublePaint();
  }
	
  public String GetFloatString(){
    return String.valueOf(itsFloat);
  }
	
  void ResizeToNewFont(Font theFont) {
    if(!itsResized){
      int tempWidth = itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("...")+20;
      int tempHeight = itsFontMetrics.getHeight()+4;
      Resize(tempWidth - currentRect.width, tempHeight - currentRect.height);
    }
    else ResizeToText(0,0);
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    
    if(aWidth<currentRect.height/2 +20+itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("..."))
      aWidth=currentRect.height/2 +20+itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("...");
    if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
	
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < currentRect.height/2+20+itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("..."))||(currentRect.height+theDeltaY<itsFontMetrics.getHeight()+4))
      return false;
    else return true;
  }
  
	
  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    int tempWidth = 20+itsFontMetrics.stringWidth("0")*6+itsFontMetrics.stringWidth("...");
    int tempHeight = itsFontMetrics.getHeight()+4;
    Resize(tempWidth - currentRect.width, tempHeight - currentRect.height);		
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }
  
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if(evt.getClickCount()>1) {
      Point aPoint = GetSketchWindow().getLocation();
      itsFloatDialog.setLocation(aPoint.x + itsX,aPoint.y + itsY - 25);
      itsFloatDialog.ReInit(String.valueOf(itsFloat), this, GetSketchWindow());
      itsFloatDialog.setVisible(true);
      return true;
    }
    if (itsSketchPad.itsRunMode) {
      itsFirstY = y;
      if (firstClick) {
	itsStartingValue = itsFloat;
	firstClick = false;
      }
      else itsStartingValue = itsFloat;
      itsFtsObject.put("value", itsFloat);

      DoublePaint();
      if(x<itsX+currentRect.width/2) fastMode = true;
      else fastMode = false;
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
	
  //--------------------------------------------------------
  //  mouseUp
  //--------------------------------------------------------
  public boolean MouseUp(MouseEvent evt,int x, int y) {
    /*if (itsSketchPad.itsRunMode) {
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
      }*/
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
      itsFtsObject.put("value", new Float(itsFloat));
      DoublePaint();
      //((FtsFloat) itsFtsActive).setValue(itsFloat);	//ENZOOOOO!
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
    //draw the white area	
    int xWhitePoints[] = {itsX+3, itsX+currentRect.width-3, itsX+currentRect.width-3, itsX+3, itsX+currentRect.height/2+3};
    int yWhitePoints[] = {itsY+1, itsY+1, itsY+currentRect.height-1,itsY+currentRect.height-1, itsY+currentRect.height/2};
    g.setColor(Color.white);
    g.fillPolygon(xWhitePoints, yWhitePoints, 5);

    //fill the triangle
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
    g.drawLine(itsX+currentRect.height/2+1, itsY+currentRect.height/2, itsX+1, itsY+currentRect.height-1);
    
    //draw the value
    String aString;
    String aString2 = ".."; 
    if (itsFloat != 0) {
      aString = String.valueOf(itsFloat);
      if(aString.length()>6){
	aString = aString.substring(0,6);
	aString = aString + aString2;
      }
    }
    else aString = "0.0";
    g.setFont(itsFont);
    g.setColor(Color.black);
    g.drawString(aString, itsX+currentRect.height/2+8,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2+1);
    
    //draw the drag box
    if(!itsSketchPad.itsRunMode)
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










