package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
//import com.sun.java.swing.Timer;

/**
 * The "message box" graphic object.
 */
class ErmesObjMessage extends ErmesObjEditableObject /*2203implements ActionListener*/{
  boolean itsFlashing = false;
  //static Timer itsTimer;
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjMessage(){
    super();
    //    if (itsTimer == null)  itsTimer = new Timer(50, this);
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
    itsFtsPatcher = GetSketchWindow().itsPatcher;
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = theFtsObject.getDescription();
    
    super.Init(theSketchPad,  theFtsObject);
    ParseText(itsArgs);
    Resize(0, itsFontMetrics.getHeight()*itsParsedTextVector.size()+2*HEIGHT_DIFF-currentRect.height);
    //#@!if(!itsResized){
      if(!IsResizeTextCompat(0,0)) RestoreDimensions();
      //#@!}
    return true;  
  }

	
  //--------------------------------------------------------
  // makeFtsObject, redefineFtsObject
  // starting of the graphic/FTS mix
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "messbox", itsArgs);
      }
    catch (FtsException e)
      {
	// ENZO !!!! AIUTO :->
	System.out.println("Error in Object Instantiation");
      }
  }

  public void redefineFtsObject()
  {
    ((FtsMessageObject)itsFtsObject).setMessage(itsArgs);
  }
  
  public boolean MouseUp_specific(MouseEvent e, int x, int y){
    if (!itsSketchPad.itsRunMode) return false;
    else {
      if (itsFlashing) {
	itsFlashing = false;
	Paint_specific(itsSketchPad.getGraphics());
      }
    }
    return true;
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      if (itsFtsObject != null){
	itsFtsObject.sendMessage(0, "bang", null);
	itsFlashing = true;
	Paint_specific(itsSketchPad.getGraphics());
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
    
  }

  //--------------------------------------------------------
  // resize
  //--------------------------------------------------------
  public void setSize(int theH, int theV) {
    Dimension d = new Dimension(theH, theV);
    super.Resize1(d.width, d.height);
    currentRect.setSize(d.width, d.height);
    d.width -= (WIDTH_DIFF-6);		
    d.height -= HEIGHT_DIFF;
    if (itsSketchPad != null) itsSketchPad.repaint();
  }
  
  public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }
  
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    
    if((!itsSelected)&&(!itsFlashing)) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
 
    g.fillRect(itsX+1,itsY+1,currentRect.width-2, currentRect.height-2);
    g.fill3DRect(itsX+2, itsY+2, currentRect.width-4, currentRect.height-4, true);
    
    if(itsFlashing) g.setColor(itsLangSelectedColor);
    else{
      if(!itsSelected) g.setColor(Color.white);
      else g.setColor(itsLangNormalColor);
    }
    g.fillRect(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF-/*6*/2), currentRect.height-HEIGHT_DIFF);
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
	theGraphics.drawString(aString, itsX+(WIDTH_DIFF-/*6*/2), itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(itsJustification == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, itsX+(currentRect.width-itsFontMetrics.stringWidth(aString))-(WIDTH_DIFF/*-6*/-2), itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
  }
}









