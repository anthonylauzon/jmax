package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

/**
 * The generic "extern" object in ermes. (example: adc1~) 
 */
public class ErmesObjExternal extends ErmesObjEditableObject {

  public boolean iAmPatcher = false;
  public ErmesSketchWindow itsSubWindow = null;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjExternal(){
    super();
  }
	
  public boolean AreYouPatcher() {
    return iAmPatcher;
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
    
    //(new Throwable()).printStackTrace();
    ParseText(itsArgs);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = theFtsObject.getDescription().trim();

    super.Init(theSketchPad, theFtsObject);

    if (theFtsObject instanceof FtsContainerObject)
      this.YouArePatcher(true);
    
    ParseText(itsArgs);
    if(!IsResizeTextCompat(0,0)) RestoreDimensions();

    return true;		// Why this method return a value ????
  }

  public void YouArePatcher(boolean what) {
    iAmPatcher = what;
  }
	
  public void update(FtsObject theFtsObject) {
    super.update(theFtsObject);
  }


  //--------------------------------------------------------
  // makeFtsObject and redefineFtsObject() 
  // starting of the graphic/FTS mix
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, itsArgs);
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }

    if (itsFtsObject instanceof FtsContainerObject)
      YouArePatcher(true);
  }

  public void redefineFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.redefineFtsObject(itsFtsObject, itsArgs);
      }
    catch (FtsException e)
      {
	// Here pop up error box or something ..
      }
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
     if (!itsSketchPad.itsRunMode) {
       if(evt.getClickCount()>1) {
	 if (iAmPatcher) {	
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
     }
     else return true;	//run mode, no editing, no subpatcher opening (?)
   }

  public void RestartEditing() { //extends ErmesObjEditableObject.RestartEditing()
    if((iAmPatcher)&&(itsSubWindow != null)){
      GetSketchWindow().CreateFtsGraphics(itsSubWindow);
      itsSubWindow.dispose();
      itsSubWindow = null;
    }
    super.RestartEditing();
  }
  


  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fillRect(itsX+1,itsY+1,currentRect.width-2, currentRect.height-2);
    g.fill3DRect(itsX+2, itsY+2, currentRect.width-4, currentRect.height-4, true);
    
    g.setColor(Color.white);
    g.fillRect(itsX+8, itsY+2, currentRect.width-(WIDTH_DIFF+4), currentRect.height-HEIGHT_DIFF-4);
    g.setColor(Color.black);
    g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
    
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont(itsFont);
    DrawParsedString(g);
  }
	
  private void DrawParsedString(Graphics theGraphics){
    String aString;
    int i=0;
    int insetY =(currentRect.height-itsFontMetrics.getHeight()*itsParsedTextVector.size())/2;//2

    if(itsJustification == itsSketchPad.CENTER_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, itsX+(currentRect.width-itsFontMetrics.stringWidth(aString))/2, itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }    
    else if(itsJustification == itsSketchPad.LEFT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, itsX+WIDTH_DIFF/*-4*/, itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(itsJustification == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, itsX+(currentRect.width-itsFontMetrics.stringWidth(aString))-(WIDTH_DIFF/*-4*/), itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
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
    d.width -= WIDTH_DIFF;		
    d.height -= HEIGHT_DIFF;
    if (itsSketchPad != null) itsSketchPad.repaint();
  }
  
  public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }
}





