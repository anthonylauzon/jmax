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
    if (theString.equals("")) super.Init(theSketchPad, x, y);	//we don't have arguments yet
    else super.Init(theSketchPad, x, y, theString);//OK, we have the args
    return true;
    /*newold itsFtsPatcher = theSketchPad.GetSketchWindow().itsPatcher;//added
      makeFtsObject();*/
    /*old
      itsSelected = false;			
      itsSketchPad = theSketchPad;    
      setItsX(x);					       
      setItsY(y);					       
      itsArgs = theString;			
      if (theString.equals("")) super.Init(theSketchPad, x, y);	//we don't have arguments yet
      else super.Init(theSketchPad, x, y, theString);//OK, we have the args
      itsFtsPatcher = GetSketchWindow().itsPatcher;
      return true;
      */
  }

  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = (String) theFtsObject.get("value");
    super.Init(theSketchPad,  theFtsObject);
    ParseText(itsArgs);
    if(!IsResizeTextCompat(0,0)) RestoreDimensions(false);
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
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "messbox");
	((FtsMessageObject)itsFtsObject).setMessage(itsArgs);
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

  // Set the text; it is a try; the message box object in the
  // application layer take care of converting the message text to the
  // "value" property; it is an hack, should be done more cleanly.

  protected void FtsValueChanged(Object value) {
    itsArgs = (String) value;
    ParseText(itsArgs);
    if (!IsResizeTextCompat(0, 0)) {
      ResizeToText(0,0);
      itsSketchPad.repaint();
    }
    else Paint(itsSketchPad.getGraphics());
  }

  public boolean NeedPropertyHandler(){
    return true;
  }

  public boolean MouseUp_specific(MouseEvent e, int x, int y){
    if (!itsSketchPad.itsRunMode && !e.isControlDown()) return false;
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
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
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
  /*public void setSize(int theH, int theV) {
    Dimension d = new Dimension(theH, theV);
    super.Resize1(d.width, d.height);
    currentRect.setSize(d.width, d.height);
    d.width -= (WIDTH_DIFF-6);		
    d.height -= HEIGHT_DIFF;
    if (itsSketchPad != null) itsSketchPad.repaint();
  }*/
  
  /*public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }*/
  
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    
    if (g == null) return;
    if (!itsSketchPad.itsRunMode) {
      if((!itsSelected)&&(!itsFlashing)) g.setColor(itsLangNormalColor);
      else g.setColor(itsLangSelectedColor);
    }
    else g.setColor(itsUINormalColor);
 
    g.fillRect(getItsX()+1,getItsY()+1,getItsWidth()-2, getItsHeight()-2);
    g.fill3DRect(getItsX()+2, getItsY()+2, getItsWidth()-4, getItsHeight()-4, true);
    
    if (!itsSketchPad.itsRunMode) {
      if(itsFlashing) g.setColor(itsLangSelectedColor);
      else{
	if(!itsSelected) g.setColor(Color.white);
	else g.setColor(itsLangNormalColor);
      }
    }
    else {
      if(itsFlashing) g.setColor(itsUISelectedColor);
      else g.setColor(Color.white);
    }
  
    g.fillRect(getItsX()+4, getItsY()+1, getItsWidth()-(WIDTH_DIFF-/*6*/2), getItsHeight()-HEIGHT_DIFF);
    g.setColor(Color.black);
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont(getFont());
    DrawParsedString(g);
  }

  private void DrawParsedString(Graphics theGraphics){
    String aString;
    int i=0;
    int insetY =(getItsHeight()-itsFontMetrics.getHeight()*itsParsedTextVector.size())/2;//2
    if(itsJustification == itsSketchPad.CENTER_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(getItsWidth()-itsFontMetrics.stringWidth(aString))/2, getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }    
    else if(itsJustification == itsSketchPad.LEFT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(WIDTH_DIFF-/*6*/2), getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(itsJustification == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(getItsWidth()-itsFontMetrics.stringWidth(aString))-(WIDTH_DIFF/*-6*/-2), getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
  }
}









