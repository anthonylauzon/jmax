package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "message box" graphic object.
 */
class ErmesObjMessage extends ErmesObjEditableObject {
  boolean itsFlashing = false;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjMessage(){
    super();
  }
	
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSelected = false;			
    itsSketchPad = theSketchPad;    
    laidOut = false;				
    itsX = x;					       
    itsY = y;					       
    itsArgs = theString;			
    if (theString.equals("")) super.Init(theSketchPad, x, y);	//we don't have arguments yet
    else super.Init(theSketchPad, x, y, theString);//OK, we have the args
    itsFtsPatcher = ((ErmesSketchWindow) (itsSketchPad.itsSketchWindow)).itsDocument.itsPatcher;
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, FtsGraphicDescription theFtsGraphic,FtsObject theFtsObject) {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = theFtsObject.getArgumentsDescription().trim();

    super.Init(theSketchPad, theFtsGraphic, theFtsObject);
    return true;  
  }
	
  //--------------------------------------------------------
  // makeFtsObject, redefineFtsObject
  // starting of the graphic/FTS mix
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "message", itsArgs);    
  }

  public void redefineFtsObject()
  {
    itsFtsObject.setArgumentsDescription(itsArgs);
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      if (itsFtsObject != null){
	itsFtsObject.sendMessage(0, "bang", null);
	ErmesObjMessThread aMessThread = itsSketchPad.GetMessThread();
	aMessThread.SetMessage(this);
	if(aMessThread.isAlive())aMessThread.resume();
	else aMessThread.start();
      }
    }
    else if(evt.getClickCount()>1) {
      if (itsSketchPad.GetEditField() != null) itsSketchPad.GetEditField().setEditable(true);
      //return true;
      //starting from here the bug 55 additions
      itsSketchPad.GetEditField().setFont(itsFont);
      itsSketchPad.GetEditField().setText(itsArgs);//warning: what will it happen if itsArgs is not here yet?
      itsSketchPad.GetEditField().itsOwner = this; //redirect the only editable field to point here...
      itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF-6), itsFontMetrics.getHeight()+20);
	       	
      itsSketchPad.GetEditField().setVisible(true);
      itsSketchPad.GetEditField().requestFocus();
      // until here bug 55
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
    
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
    d.width -= (WIDTH_DIFF-6);		
    d.height -= HEIGHT_DIFF;
    itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, d.width, itsSketchPad.GetEditField().getFontMetrics(itsSketchPad.GetEditField().getFont()).getHeight()+20);
    if (itsSketchPad != null) itsSketchPad.repaint();
    
  }
  
  public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }
  
  //--------------------------------------------------------
  // ConnectionRequested
  //--------------------------------------------------------
  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    if (!theRequester.IsInlet())	//if is an outlet...
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); // then, is it's an inlet
  }
  
  //--------------------------------------------------------
  // ConnectionAbort
  //--------------------------------------------------------
  public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
    theRequester.ChangeState(false, theRequester.connected);
    itsSketchPad.ResetConnect();
    return true;	//for now, everything is allowed
  }
  
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
 
    g.fillRect(itsX+1,itsY+1,currentRect.width-2, currentRect.height-2);
    g.fill3DRect(itsX+2, itsY+2, currentRect.width-4, currentRect.height-4, true);
    
    if(itsFlashing) g.setColor(itsLangNormalColor);
    else g.setColor(Color.white);
    g.fillRect(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF-6), currentRect.height-HEIGHT_DIFF);
    g.setColor(Color.black);
    g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
    
    g.setColor(Color.black);
    g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont(itsFont);
    g.drawString(itsArgs, itsX+(currentRect.width-itsFontMetrics.stringWidth(itsArgs))/2,
		 itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2);
  }
}

















