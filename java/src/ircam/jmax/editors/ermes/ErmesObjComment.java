package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "comment" graphic object
 */
class ErmesObjComment extends ErmesObject {
  Dimension preferredSize = null;	
  String itsArgs = " ";
  Vector itsParsedTextVector = new Vector();
  public String itsMaxString = "";
  int FIELD_HEIGHT;
  final int TEXT_INSET = 10;
  int itsTextRowNumber = 1;

  public ErmesObjComment(){
    super();
  }
	
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSketchPad = theSketchPad;
    itsFont = theSketchPad.sketchFont;			
    itsFontMetrics = theSketchPad.getFontMetrics(itsFont);
    itsX = x; itsY = y;
    if (!theString.equals("")) {//we have arguments (are you creating me from a script?)
      itsArgs = theString;
      preferredSize = new Dimension(70, itsFontMetrics.getHeight()*5);
      super.Init(theSketchPad, x, y, theString);
      return true;
    }
    itsFontMetrics = itsSketchPad.GetTextArea().getFontMetrics(itsFont);
    FIELD_HEIGHT = itsFontMetrics.getHeight();
    preferredSize = new Dimension(70,FIELD_HEIGHT*5);
    itsSketchPad.GetTextArea().setFont(itsFont);
    itsSketchPad.GetTextArea().setBackground(Color.white);
    itsSketchPad.GetTextArea().setText("");
    itsJustification = itsSketchPad.itsJustificationMode;
    currentRect = new Rectangle();
    itsSketchPad.GetTextArea().itsOwner = this; //redirect the only editable field to point here...
    currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
    Reshape(itsX, itsY, preferredSize.width, preferredSize.height);//?
    
    itsSketchPad.GetTextArea().setBounds(itsX,itsY,currentRect.width,currentRect.height);
    itsSketchPad.validate();
    itsSketchPad.editStatus = itsSketchPad.EDITING_COMMENT;
    itsSketchPad.GetTextArea().setVisible(true);
    itsSketchPad.GetTextArea().requestFocus();
    itsFtsPatcher = itsSketchPad.GetSketchWindow().itsPatcher;
    return true;
  }

  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    FontMetrics temporaryFM = theSketchPad.getFontMetrics(theSketchPad.getFont());
    FIELD_HEIGHT = temporaryFM.getHeight();
    itsArgs = theFtsObject.getDescription();
    preferredSize = new Dimension(temporaryFM.stringWidth(itsArgs),FIELD_HEIGHT*5);
    super.Init(theSketchPad,  theFtsObject);
    
    Integer aJustification = (Integer)theFtsObject.get("jsf");
    if(aJustification == null) itsJustification = itsSketchPad.itsJustificationMode;
    else itsJustification = aJustification.intValue();

    itsSketchPad.GetTextArea().setBackground(Color.white);
    
    ParseText(itsArgs);
    if(!itsResized) RestoreDimensions();    
    return true;
  }
	
  // starting of the graphic/FTS mix

  public void makeFtsObject(){
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "comment", itsArgs);
      }
    catch (FtsException e)
      {
	// ENZO !!!! AIUTO :->
	System.out.println("Error in Object Instantiation");
      }
  }

  public void redefineFtsObject(){
    ((FtsCommentObject)itsFtsObject).setComment(itsArgs);
  }
  
  //the 'ResizeToNewFont' method worked very well, so we're using it also in the shift-double_click  
  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    Resize(itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET-currentRect.width, itsFontMetrics.getHeight()*itsParsedTextVector.size()-currentRect.height);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
    return true;
  }

  public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
    return true;	//for now, everything is allowed
  }

  public boolean MouseUp(MouseEvent evt,int x,int y){
    return super.MouseUp(evt, x, y);
  }

  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    if (itsSketchPad.itsRunMode) return true; 
    itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
  
  public void RestartEditing(){
    if (itsSketchPad.GetTextArea() != null) itsSketchPad.GetTextArea().setEditable(true);
    itsSketchPad.GetTextArea().setFont(itsFont);
    itsSketchPad.GetTextArea().setText(itsArgs);
    itsSketchPad.GetTextArea().itsOwner = this;

    if(itsParsedTextVector.size()==0)
      itsSketchPad.GetTextArea().setBounds(itsX, itsY, currentRect.width+10, itsFontMetrics.getHeight()*5);
    else
      itsSketchPad.GetTextArea().setBounds(itsX, itsY, currentRect.width+10,itsFontMetrics.getHeight()*(itsParsedTextVector.size()+1));

    itsMaxString = "";
    itsParsedTextVector.removeAllElements();
    
    itsSketchPad.GetTextArea().setVisible(true);
    itsSketchPad.GetTextArea().requestFocus();
    itsSketchPad.GetTextArea().setCaretPosition(itsArgs.length());
  }

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

  
  void ResizeToNewFont(Font theFont) {
    if(!itsResized){
      Resize(itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET - currentRect.width,
	     itsFontMetrics.getHeight()*itsParsedTextVector.size()- currentRect.height);
    }
    else ResizeToText(0,0);
  }


  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET) aWidth = itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET;
    if(aHeight<itsFontMetrics.getHeight()*itsParsedTextVector.size()) aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size();
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    String temp = itsArgs;
    if((currentRect.width+theDeltaX <itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET)||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight()*itsParsedTextVector.size()))
      return false;
    else return true;
  }

  public void ParseText(String theString){
    int aIndex = theString.indexOf("\n");
    int aOldIndex = -1;
    int aLastIndex = theString.lastIndexOf("\n");
    String aString;
    int length = 0;
    int i = 0;
    while(aIndex!=-1){
      aString = theString.substring(aOldIndex+1, aIndex);
      length = itsFontMetrics.stringWidth(aString);
      if(length> itsFontMetrics.stringWidth(itsMaxString)) 
	itsMaxString = aString;
      itsParsedTextVector.addElement(aString);
      aOldIndex = aIndex;
      aIndex = theString.indexOf("\n", aOldIndex+1);
      i++;
    }
    aString = theString.substring(aOldIndex+1);
    length = itsFontMetrics.stringWidth(aString);
    if(length> itsFontMetrics.stringWidth(itsMaxString)) 
      itsMaxString = aString;
    itsParsedTextVector.addElement(aString);
  }

  public void Paint_specific(Graphics g) {
    if(!itsSketchPad.itsRunMode){ 
      if(itsSelected) g.setColor(Color.gray);
      else g.setColor(itsSketchPad.getBackground());
      g.fill3DRect(itsX,itsY, currentRect.width, currentRect.height, true);
    
      //drag box
      if(itsSelected) {
	g.setColor(Color.gray.darker());
	g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
      }
    }
    //text
    if(!itsArgs.equals(" ")){
      g.setColor(Color.black);
      g.setFont(itsFont);
      DrawParsedString(g);
    }
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
	theGraphics.drawString(aString, itsX+2, itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(itsJustification == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, itsX+(currentRect.width-itsFontMetrics.stringWidth(aString))-2, itsY+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
  }

  void putOtherProperties(FtsObject theFObject){
    if (itsJustification != itsSketchPad.itsJustificationMode)
      theFObject.put("jsf", itsJustification);
  }
  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    if(itsParsedTextVector.size()==0) return getPreferredSize();
    else
      return new Dimension(itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET,
			   itsFontMetrics.getHeight()*itsParsedTextVector.size());
  }

  //If we don't specify this, the canvas might not show up at all
  //(depending on the layout manager).
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}









