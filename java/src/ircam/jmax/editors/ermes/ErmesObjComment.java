package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "comment" graphic object
 */
class ErmesObjComment extends ErmesObject {
  Dimension preferredSize = new Dimension(100, 100); //hu-hu	
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
    super.Init(theSketchPad, x, y, theString);

    FIELD_HEIGHT = itsFontMetrics.getHeight();
    preferredSize = new Dimension(70,FIELD_HEIGHT*5);

    itsSketchPad.GetTextArea().setFont(getFont());
    itsSketchPad.GetTextArea().setBackground(Color.white);
    itsSketchPad.GetTextArea().setText("");
    setJustification(itsSketchPad.itsJustificationMode);
    
    itsSketchPad.GetTextArea().itsOwner = this; //redirect the only editable field to point here...
    makeCurrentRect(x, y);
    
    itsSketchPad.GetTextArea().setBounds(getItsX(),getItsY(),getItsWidth(),getItsHeight());
    itsSketchPad.validate();
    itsSketchPad.editStatus = itsSketchPad.EDITING_COMMENT;
    itsSketchPad.GetTextArea().setVisible(true);
    itsSketchPad.GetTextArea().requestFocus();
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
    if(aJustification == null) setJustification(itsSketchPad.itsJustificationMode);
      else setJustification(aJustification.intValue());

    itsSketchPad.GetTextArea().setBackground(Color.white);
    
    ParseText(itsArgs);
    /*#@!if(!itsResized)*/ RestoreDimensions();    
    return true;
  }
	
  // starting of the graphic/FTS mix

  public void makeFtsObject(){
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "comment", itsArgs);
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

    Resize(itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET-getItsWidth(), itsFontMetrics.getHeight()*itsParsedTextVector.size()-getItsHeight());
    itsSketchPad.repaint();
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
    itsSketchPad.GetTextArea().setFont(getFont());
    itsSketchPad.GetTextArea().setText(itsArgs);
    itsSketchPad.GetTextArea().itsOwner = this;

    if(itsParsedTextVector.size()==0)
      itsSketchPad.GetTextArea().setBounds(getItsX(), getItsY(), getItsWidth()+10, itsFontMetrics.getHeight()*5);
    else
      itsSketchPad.GetTextArea().setBounds(getItsX(), getItsY(), getItsWidth()+10,itsFontMetrics.getHeight()*(itsParsedTextVector.size()+1));

    itsMaxString = "";
    itsParsedTextVector.removeAllElements();
    
    itsSketchPad.GetTextArea().setVisible(true);
    itsSketchPad.GetTextArea().requestFocus();
    itsSketchPad.GetTextArea().setCaretPosition(itsArgs.length());
  }

  public void setSize(int theH, int theV) {
    Resize1(theH, theV);
    if (itsSketchPad != null) itsSketchPad.repaint();
  }
  
  public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }

  
  /*void ResizeToNewFont(Font theFont) {
    ResizeToText(0,0);
    }
    */


  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET) aWidth = itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET;
    if(aHeight<itsFontMetrics.getHeight()*itsParsedTextVector.size()) aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size();
    Resize(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    String temp = itsArgs;
    if((getItsWidth()+theDeltaX <itsFontMetrics.stringWidth(itsMaxString)+TEXT_INSET)||
       (getItsHeight()+theDeltaY<itsFontMetrics.getHeight()*itsParsedTextVector.size()))
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
      g.fill3DRect(getItsX(),getItsY(), getItsWidth(), getItsHeight(), true);
    
      //drag box
      if(itsSelected) {
	g.setColor(Color.gray.darker());
	g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
      }
    }
    //text
    if(!itsArgs.equals(" ")){
      g.setColor(Color.black);
      g.setFont(getFont());
      DrawParsedString(g);
    }
  }
	
  private void DrawParsedString(Graphics theGraphics){
    String aString;
    int i=0;
    int insetY =(getItsHeight()-itsFontMetrics.getHeight()*itsParsedTextVector.size())/2;//2
    if(getJustification() == itsSketchPad.CENTER_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(getItsWidth()-itsFontMetrics.stringWidth(aString))/2, getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }    
    else if(getJustification() == itsSketchPad.LEFT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+2, getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(getJustification() == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(getItsWidth()-itsFontMetrics.stringWidth(aString))-2, getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
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









