package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

/**
 * The "patcher" graphic object. It knows the subpatchers it contains.
 */
public class ErmesObjPatcher extends ErmesObject {

  String itsNameString;
  public ErmesSketchWindow itsSubWindow = null;
  Dimension preferredSize = new Dimension(80,24);
  String pathForLoading;
	
  //--------------------------------------------------------
  // Constructor
  //--------------------------------------------------------
  public ErmesObjPatcher(){
    super();
  }
	
  /*	//--------------------------------------------------------
	// Init
	//--------------------------------------------------------
	public boolean Init(ErmesSketchPad theSketchPad, int x, int y) {
	super.Init(theSketchPad, x, y);
	itsNameString = "patcher...";
	preferredSize = new Dimension(SetDimension(itsNameString));
	currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
	return true;
	}*/
  
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    if (theString.length() != 0) itsNameString = theString;
    else itsNameString = "patcher...";
    preferredSize = new Dimension(80, 20);	//vergogna
    super.Init(theSketchPad, x, y, theString);
    preferredSize = new Dimension(SetDimension(itsNameString));
    currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
    return true;
  }
  
  // This Init(...FtsClass) is called when a FtsClass is available for the Object.
  // this happens, now, just when we are loading from a file
  public boolean Init(ErmesSketchPad theSketchPad, FtsGraphicDescription theFtsGraphic, FtsObject theFtsObject) {
    String theString;

    // modified by MDC to get the name from the object.
    // It get the arguments, excluding the "patcher" word

    theString = theFtsObject.getArgumentsDescription();

    if (theString.length() != 0)
      itsNameString = theString;
    else
      itsNameString = "patcher...";

    // preferredSize = new Dimension(80, 20);	//vergogna
    super.Init(theSketchPad, theFtsGraphic, theFtsObject);
    if((theFtsGraphic.width<10)||(theFtsGraphic.height<10)){
      preferredSize = new Dimension(SetDimension(itsNameString));
      //currentRect = new Rectangle(theFtsGraphic.x,theFtsGraphic.y, preferredSize.width, preferredSize.height);
    }
    else  preferredSize = new Dimension(80, 20);	//vergogna
    super.Init(theSketchPad, theFtsGraphic, theFtsObject);
    return true;
  }

  // starting of the graphic/FTS mix
  // temporary, should probabily change

  public void makeFtsObject()
  {
    itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "patcher", itsNameString);    
  }

  public void redefineFtsObject()
  {
    itsFtsObject.setArgumentsDescription(itsNameString);
  }
  
	
  //--------------------------------------------------------
  // GetName
  //--------------------------------------------------------
  public String GetName(){
    return itsNameString;
  }
	
  //--------------------------------------------------------
  // GetPath
  //--------------------------------------------------------
  public String GetPath(){
    return pathForLoading;
  }
	
  //--------------------------------------------------------
  // SetDimension
  //--------------------------------------------------------
  private Dimension SetDimension(String theString){
    //Font f = itsSketchPad.getFont();
    //FontMetrics fm = itsSketchPad.getFontMetrics(f);
    int lenght = itsFontMetrics.stringWidth(theString);
    Dimension d1 = minimumSize();
    d1.width = MaxWidth(itsFontMetrics.stringWidth(theString)+32,
			    (itsInletList.size())*12, (itsOutletList.size())*12, d1.width);
    int height = itsFontMetrics.getHeight();
    if(d1.height< height+10) d1.height = height+10;
    
    Resize1(d1.width, d1.height);
    itsSketchPad.validate();
    return d1;
  }
	
  public int MaxWidth(int uno, int due, int tre, int quattro){
    int MaxInt = uno;
    if(due>MaxInt) MaxInt=due;
    if(tre>MaxInt) MaxInt=tre;
    if(quattro>MaxInt) MaxInt=quattro;
    return MaxInt;
  }

  public int MaxWidth(int uno, int due, int tre){
    int MaxInt = uno;
    if(due>MaxInt) MaxInt=due;
    if(tre>MaxInt) MaxInt=tre;
    return MaxInt;
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
  // mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if(evt.getClickCount()>1){
      if (itsSubWindow != null) {
	itsSubWindow.show();
	itsSketchPad.itsFirstClick = true;
      }
      //ask MaxApplication to create a new Patcher with this FtsPatcher
      //
      else {
	itsSubWindow = MaxApplication.getApplication().NewSubPatcherWindow( itsFtsObject);
	((ErmesSketchWindow)itsSketchPad.GetSketchWindow()).AddToSubWindowList(itsSubWindow);
      }
    }
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
	
  //--------------------------------------------------------
  // Load
  //--------------------------------------------------------
  public boolean Load(String file, String path) {
    itsNameString = file;
    pathForLoading = path;
    Dimension d = SetDimension(itsNameString);
    preferredSize.width = d.width;
    preferredSize.height = d.height;
    currentRect.width = preferredSize.width;
    currentRect.height = preferredSize.height;
    return true;
  }
	
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fill3DRect(itsX+1, itsY+1, currentRect.width-2, currentRect.height-2, true);
    g.draw3DRect(itsX+3, itsY+3, currentRect.width-6, currentRect.height-6, false);
    
    int xPoints[] = {itsX+7,itsX+7,itsX+13};
    int yPoints[] = {itsY+6,itsY+18,itsY+12};
    g.fillPolygon(xPoints, yPoints, 3);
    g.setColor(Color.black);
    g.drawRect(itsX+0,itsY+ 0, currentRect.width-1, currentRect.height-1);
    g.drawRect(itsX+4, itsY+4, currentRect.width-8, currentRect.height-8);
    g.drawLine(itsX+7,itsY+6,itsX+7,itsY+/*18*/currentRect.height-6);
    g.drawLine(itsX+7,itsY+6,itsX/*+13*/+currentRect.width/6,itsY/*+12*/+currentRect.height/2);
    g.drawLine(itsX+/*13*/currentRect.width/6,itsY/*+12*/+currentRect.height/2,itsX+7,itsY/*+18*/+currentRect.height-6);
    g.setFont(itsFont);
    g.drawString(itsNameString, itsX+(currentRect.width-itsFontMetrics.stringWidth(itsNameString))/2/*currentRect.width/6+3*/,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2);		
    
    g.setColor(Color.black);
    g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  void ResizeToNewFont(Font theFont) {
    if(!itsResized){
      Resize(itsFontMetrics.stringWidth(itsNameString) + 32 - currentRect.width,
	     itsFontMetrics.getHeight() + 10 - currentRect.height);
    }
    else ResizeToText(0,0);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsNameString) + 32) 
      aWidth = itsFontMetrics.stringWidth(itsNameString) + 32;
    if(aHeight<itsFontMetrics.getHeight() + 10) 
      aHeight = itsFontMetrics.getHeight() + 10;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < itsFontMetrics.stringWidth(itsNameString)+32)||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 10))
      return false;
    else return true;
  }

  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    int aMaxWidth = MaxWidth(itsFontMetrics.stringWidth(itsNameString)+32,
			    (itsInletList.size())*12, (itsOutletList.size())*12);
    Resize(aMaxWidth-currentRect.width, itsFontMetrics.getHeight() + 10 - currentRect.height);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension minimumSize() {
    return new Dimension(80,24);
  }
    
  //--------------------------------------------------------
  // preferredSize()
  //--------------------------------------------------------
  public Dimension preferredSize() {
    return minimumSize();
  }
}












