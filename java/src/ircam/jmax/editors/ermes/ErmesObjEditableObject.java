package ircam.jmax.editors.ermes;


import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal).
 */
/*abstract*/ class ErmesObjEditableObject extends ErmesObject {
	
	
  int WIDTH_DIFF = 10/*14*/;
  int HEIGHT_DIFF = 2;
	
  protected int FIELD_HEIGHT;
  protected int FIELD_WIDTH;
  Dimension preferredSize = new Dimension(100, 25);//hu-hu 
  Dimension currentMinimumSize = new Dimension();
  String 	  itsArgs = new String();
  public Vector itsParsedTextVector = new Vector();
  public String itsMaxString = "";
  //public void setSize(int theH, int theV) {}; 
  public boolean resized = false;
  public boolean itsInEdit = true;
  String itsBackupText = new String();;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjEditableObject(){
    super();
  }

  //--------------------------------------------------------
  // Init from skratch
  //--------------------------------------------------------

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);
    
    if (theString.equals("")) {
      FIELD_HEIGHT = itsFontMetrics.getHeight();
      FIELD_WIDTH = itsFontMetrics.stringWidth("0");
      preferredSize = new Dimension(70, FIELD_HEIGHT+2*HEIGHT_DIFF);
      itsSketchPad.GetEditField().setFont(getFont());
      itsSketchPad.GetEditField().setText("");
      itsSketchPad.GetEditField().itsOwner = this; //redirect the only editable field to point here...
      setJustification(itsSketchPad.itsJustificationMode);
      makeCurrentRect(x,y); //redo it..
      
      itsSketchPad.GetEditField().setBounds(getItsX()+4, getItsY()+1, getItsWidth()-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight() + 20);
      //DoublePaint();
      itsSketchPad.editStatus = itsSketchPad.EDITING_OBJECT;
      
      itsSketchPad.GetEditField().setVisible(true);
      itsSketchPad.GetEditField().requestFocus();
    }
    return true;
  }
  

  //--------------------------------------------------------
  // Init from ftsObjects
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad,  FtsObject theFtsObject) {

    // MDC: itsArgs is set by the Init methods of the subclasses

    FontMetrics temporaryFM = theSketchPad.getFontMetrics(theSketchPad.getFont());
    FIELD_HEIGHT = temporaryFM.getHeight();

    int lenght = temporaryFM.stringWidth(itsArgs);	//*
    preferredSize = new Dimension(lenght+2*WIDTH_DIFF, FIELD_HEIGHT+2*HEIGHT_DIFF);	//*
    super.Init(theSketchPad, theFtsObject);
    
    Integer aJustification = (Integer)theFtsObject.get("jsf");
    if(aJustification == null) setJustification(itsSketchPad.itsJustificationMode);
    else setJustification(aJustification.intValue());

    itsInEdit = false;
    //ChangeJustification(itsSketchPad.LEFT_JUSTIFICATION);
    return true;
  }

  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
	
  void backupText() {
    itsBackupText = itsArgs.toString();
  }

  void restoreText() {
    itsArgs = itsBackupText.toString();
  }

  public void RestartEditing(){
    if (itsSketchPad.GetEditField() != null) itsSketchPad.GetEditField().setEditable(true);
    
    itsSketchPad.GetEditField().setFont(getFont());
    itsSketchPad.GetEditField().setText(itsArgs);
    itsSketchPad.GetEditField().itsOwner = this; 


    if(itsParsedTextVector.size()==0)
      itsSketchPad.GetEditField().setBounds(getItsX()+4, getItsY()+1, getItsWidth()-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight()*2);
    else
      itsSketchPad.GetEditField().setBounds(getItsX()+4, getItsY()+1, getItsWidth()-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight()*(itsParsedTextVector.size()+1));
    

    itsMaxString = "";
    itsParsedTextVector.removeAllElements();
    
    itsSketchPad.GetEditField().setVisible(true);
    itsSketchPad.GetEditField().requestFocus();
    itsSketchPad.GetEditField().setCaretPosition(itsArgs.length());
  }

  public boolean MouseUp(MouseEvent evt,int x,int y){
    if(itsInEdit) return true;
    else return super.MouseUp(evt, x, y);
  }

  public boolean IsResizedObject(int theWidth){
    return (theWidth>MaxWidth(itsFontMetrics.stringWidth(itsArgs)+2*WIDTH_DIFF,
			    (itsInletList.size())*12, (itsOutletList.size())*12));
  }

  public void RestoreDimensions(boolean paintNow){

    int aMaxWidth = MaxWidth(itsFontMetrics.stringWidth(itsMaxString)+2*WIDTH_DIFF,
			    (itsInletList.size())*12, (itsOutletList.size())*12);
   
    int aHeightDiff = itsFontMetrics.getHeight()*itsParsedTextVector.size()+2*HEIGHT_DIFF-getItsHeight();
    int aWidthDiff = aMaxWidth-getItsWidth();
    if (aHeightDiff == 0 && aWidthDiff == 0) return;
    Resize(aWidthDiff, aHeightDiff);
    if (paintNow) {
      if (aHeightDiff < 0 || aWidthDiff <0)
	itsSketchPad.repaint();
      else DoublePaint();
    }    
    else {
      if (aHeightDiff < 0 || aWidthDiff <0)
	itsSketchPad.markSketchAsDirty();
      else itsSketchPad.addToDirtyObjects(this);
    } 
  }

  public int MaxWidth(int uno, int due, int tre){
    int MaxInt = uno;
    if(due>MaxInt) MaxInt=due;
    if(tre>MaxInt) MaxInt=tre;
    return MaxInt;
  }



  //--------------------------------------------------------
  // SetFieldText
  //--------------------------------------------------------
  protected void SetFieldText(String theString){
    FontMetrics fm = itsSketchPad.GetEditField().getFontMetrics(itsSketchPad.getFont());
    int lenght = fm.stringWidth(theString);
    Dimension d1 = preferredSize;/*minimumSize();*/
    while(lenght > d1.width-20)
      d1.width += 20;
    preferredSize = d1;	
    Resize1(d1.width, d1.height);	
    itsSketchPad.GetEditField().setText(theString);
    itsSketchPad.GetEditField().setSize(d1.width-WIDTH_DIFF, d1.height-HEIGHT_DIFF);
    itsSketchPad.GetEditField().repaint();
    itsSketchPad.validate();//??
    itsSketchPad.repaint();
  }
	
  void ResizeToNewFont(Font theFont) {
    ResizeToText(0,0);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString) + 2*WIDTH_DIFF) aWidth = itsFontMetrics.stringWidth(itsMaxString) + 2*WIDTH_DIFF;
    if(aHeight<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF) aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF;
    Resize(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    String temp = itsArgs;
    if((getItsWidth()+theDeltaX <itsFontMetrics.stringWidth(itsMaxString) +2*WIDTH_DIFF)||
       (getItsHeight()+theDeltaY<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF))
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

    itsParsedTextVector.removeAllElements();

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

  //--------------------------------------------------------
  // RunModeSetted
  //--------------------------------------------------------
  void RunModeSetted(){
    //if (itsEditField != null) itsEditField.setEditable(false);
  }
	
  //--------------------------------------------------------
  // MoveBy
  //--------------------------------------------------------
  public void MoveBy(int theDeltaH, int theDeltaV) {
    super.MoveBy(theDeltaH, theDeltaV);
  }
	
  public void MoveOutlets(){
    ErmesObjOutlet aOutlet;
    for (Enumeration e=itsOutletList.elements(); e.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet) e.nextElement();
      aOutlet.MoveTo(aOutlet.itsX, getItsY()+getItsHeight());
      //ReroutingConnections(aOutlet);
    }
  }


  protected void DrawParsedString(Graphics theGraphics){
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
	theGraphics.drawString(aString, getItsX()+WIDTH_DIFF/*-4*/, getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
    else if(getJustification() == itsSketchPad.RIGHT_JUSTIFICATION){
      for (Enumeration e = itsParsedTextVector.elements(); e.hasMoreElements();) {
	aString = (String)e.nextElement();
	theGraphics.drawString(aString, getItsX()+(getItsWidth()-itsFontMetrics.stringWidth(aString))-(WIDTH_DIFF/*-4*/), getItsY()+itsFontMetrics.getAscent()+insetY+itsFontMetrics.getHeight()*i);
	i++;
      }
    }
  }

  //--------------------------------------------------------
  // minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    if(itsParsedTextVector.size()==0) return getPreferredSize();
    else {
      currentMinimumSize.width = itsFontMetrics.stringWidth(itsMaxString)+2*WIDTH_DIFF;
      currentMinimumSize.height = itsFontMetrics.getHeight()*itsParsedTextVector.size()+HEIGHT_DIFF;
      return currentMinimumSize;
    }
  }
  
  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
	
}










