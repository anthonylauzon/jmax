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
/* NOTE:
This class contains a number of non-optimized code, because
The need have arised to have this object EDITABLE soon (5 Mars 1998)
*/
public class ErmesObjPatcher extends ErmesObjEditableObject {

  //old variable set 
  String itsNameString = new String();
  public ErmesSketchWindow itsSubWindow = null;
  Dimension preferredSize = new Dimension(80,24);
  String pathForLoading;
 
  //--------------------------------------------------------
  // Constructor
  //--------------------------------------------------------
  public ErmesObjPatcher(){
    super();
    HEIGHT_DIFF = 5;
    WIDTH_DIFF = 11;
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSelected = false;			//this was in ErmesObject
    itsSketchPad = theSketchPad;	
    laidOut = false;				
    itsX = x;						
    itsY = y;						
    itsArgs = theString;			
    if (theString.equals("")) super.Init(theSketchPad, x, y);	//we don't have arguments yet
    else super.Init(theSketchPad, x, y, theString); //OK, we have the args
    itsFtsPatcher = GetSketchWindow().itsPatcher;
    
    ParseText(itsArgs);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    // Added by MDC; get the correct String from the object, and then call super

    itsArgs = theFtsObject.getDescription().trim();

    super.Init(theSketchPad, theFtsObject);

    
    ParseText(itsArgs);
    //ChangeJustification(itsSketchPad.LEFT_JUSTIFICATION);
    RestoreDimensions();

    return true;		// Why this method return a value ????
  }

  // starting of the graphic/FTS mix
  // temporary, should probabily change

  public void makeFtsObject()
  {
    itsFtsObject = new FtsPatcherObject(itsFtsPatcher, itsArgs);
  }

  public void RestartEditing() { //extends ErmesObjEditableObject.RestartEditing()
    if(itsSubWindow != null){
      GetSketchWindow().CreateFtsGraphics(itsSubWindow);
      //itsSubWindow.dispose();
      //itsSubWindow = null;
      //itsSubWindow.setVisible(false);
    }
    super.RestartEditing();
  }

  public void redefineFtsObject()
  {
    //the parent patcher could destroy connections...
    GetSketchWindow().itsPatcher.watch("deletedConnection",GetSketchWindow());
    //the children could destroy connections AND objects
    if (itsSubWindow != null) {
      itsSubWindow.itsPatcher.watch("deletedObject", itsSubWindow);
     itsSubWindow.itsPatcher.watch("deletedConnection", itsSubWindow);
    }
    ((FtsPatcherObject)itsFtsObject).redefinePatcher(itsArgs);
   if (itsSubWindow != null)  itsFtsObject.removeWatch(itsSubWindow);  
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
    Dimension d1 = getMinimumSize();
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
    //if (!itsSketchPad.itsRunMode) {
       if(evt.getClickCount()>1) {
	 {	
	   if (itsSubWindow != null) {//show the subpatcher, it's there
	     itsSubWindow.setVisible(true);
	     ErmesSketchPad.RequestOffScreen(itsSketchPad);
	   }
	   else{	//this 'else' shouldn't be reached...
	     itsSubWindow = new ErmesSketchWindow( GetSketchWindow().itsData, (FtsContainerObject) itsFtsObject, GetSketchWindow());
	     MaxApplication.itsSketchWindowList.addElement(itsSubWindow);
	     GetSketchWindow().AddToSubWindowList(itsSubWindow);
	   }
	   return true;
	 }
       }
       itsSketchPad.ClickOnObject(this, evt, x, y);
       return true;
       //}
       /*else return true;*/	//run mode, no editing, no subpatcher opening (?)
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
    
    //the triangle
    g.drawLine(itsX+7,itsY+6,itsX+7,itsY+currentRect.height-6);
    g.drawLine(itsX+7, itsY+6, itsX+currentRect.height/2+2, itsY+currentRect.height/2);
    g.drawLine(itsX+currentRect.height/2+2,itsY+currentRect.height/2, itsX+7, itsY+currentRect.height-6);


    g.setFont(itsFont);
    g.drawString(itsArgs, itsX+(currentRect.width-itsFontMetrics.stringWidth(itsArgs))/2/*currentRect.width/6+3*/,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2);		
    
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  void ResizeToNewFont(Font theFont) {
    if(!itsResized){
      Resize(itsFontMetrics.stringWidth(itsMaxString) + 32 - currentRect.width,
	     itsFontMetrics.getHeight() + 10 - currentRect.height);
    }
    else ResizeToText(0,0);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString) + 32) 
      aWidth = itsFontMetrics.stringWidth(itsMaxString) + 32;
    if(aHeight<itsFontMetrics.getHeight() + 10) 
      aHeight = itsFontMetrics.getHeight() + 10;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < itsFontMetrics.stringWidth(itsMaxString)+32)||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 10))
      return false;
    else return true;
  }

  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    int aMaxWidth = MaxWidth(itsFontMetrics.stringWidth(itsMaxString)+2*WIDTH_DIFF+10,
			    (itsInletList.size())*12, (itsOutletList.size())*12);
    Resize(aMaxWidth-currentRect.width, itsFontMetrics.getHeight() + 10 - currentRect.height);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }

    //--------------------------------------------------------
  // resize
  //--------------------------------------------------------
  public void setSize(int theH, int theV) {
    Dimension d = new Dimension(theH, theV);
    if (itsSketchPad != null) itsSketchPad.RemoveElementRgn(this);
    super.Resize1(d.width, d.height);
    if (itsSketchPad != null) itsSketchPad.SaveOneElementRgn(this);
    currentRect.setSize(d.width, d.height);
    if (itsSketchPad != null) itsSketchPad.repaint();
  }
  
  public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }
 
}












