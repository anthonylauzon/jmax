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
  Dimension preferredSize = null;
  Dimension currentMinimumSize = new Dimension(); 
  String 	  itsArgs;
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
  // Init
  //--------------------------------------------------------
  //this 'glue' method is called when the external (or the message) have not their arguments yet
  //and we have to create also the (static) ErmesObjEditableField 
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y) {
    itsX = x; itsY = y;
    itsSketchPad = theSketchPad;
    itsFont = itsSketchPad.sketchFont;

    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    FIELD_HEIGHT = itsFontMetrics.getHeight();
    FIELD_WIDTH = itsFontMetrics.stringWidth("0");
    preferredSize = new Dimension(70/*FIELD_WIDTH+2*WIDTH_DIFF*/,FIELD_HEIGHT+2*HEIGHT_DIFF);
    itsSketchPad.GetEditField().setFont(itsFont);
    itsSketchPad.GetEditField().setText("");
    itsSketchPad.GetEditField().itsOwner = this; //redirect the only editable field to point here...
    itsJustification = itsSketchPad.itsJustificationMode;
    currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
    Reshape(itsX, itsY, preferredSize.width, preferredSize.height);
    
    itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight() + 20);
    DoublePaint();
    itsSketchPad.editStatus = itsSketchPad.EDITING_OBJECT;
    
    itsSketchPad.GetEditField().setVisible(true);
    itsSketchPad.GetEditField().requestFocus();
 
    return true;
  }
  
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  //this method is called when the external (or the message) have their arguments
  //this happens (for now) just when we are instantiating from a script
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String args) {
    itsFont = itsSketchPad.sketchFont;
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont); //bugsgi*/
    itsJustification = itsSketchPad.itsJustificationMode;
    FIELD_HEIGHT = itsFontMetrics.getHeight();
    FIELD_WIDTH = itsFontMetrics.stringWidth("0");
    preferredSize = new Dimension(70/*FIELD_WIDTH+2*WIDTH_DIFF*/, FIELD_HEIGHT+2*HEIGHT_DIFF);
    super.Init(theSketchPad, x, y, args);
    
    ResizeToText(0, 0);	//will it work?
    return true;
  }

  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad,  FtsObject theFtsObject) {
  	//the Fontmetrics problem: the ErmesObject.Init function need the currentRect already set, 
  	// but for editable objects this depends from itsFontMetrics.
  	// malheureusement, the itsFontMetrics field is built... in ErmesObject.Init !

    // MDC: itsArgs is set by the Init methods of the subclasses

    FontMetrics temporaryFM = theSketchPad.getFontMetrics(theSketchPad.getFont());
    FIELD_HEIGHT = temporaryFM.getHeight();
    int lenght = temporaryFM.stringWidth(itsArgs);	//*
    preferredSize = new Dimension(lenght+2*WIDTH_DIFF, FIELD_HEIGHT+2*HEIGHT_DIFF);	//*
    super.Init(theSketchPad, theFtsObject);
    
    Integer aJustification = (Integer)theFtsObject.get("jsf");
    if(aJustification == null) itsJustification = itsSketchPad.itsJustificationMode;
    else itsJustification = aJustification.intValue();

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
    System.err.println("guarda che itsArgs e' nullo");
    itsBackupText = itsArgs.toString();
  }

  void restoreText() {
    itsArgs = itsBackupText.toString();
  }

  public void RestartEditing(){
    if (itsSketchPad.GetEditField() != null) itsSketchPad.GetEditField().setEditable(true);
    
    itsSketchPad.GetEditField().setFont(itsFont);
    itsSketchPad.GetEditField().setText(itsArgs);
    itsSketchPad.GetEditField().itsOwner = this; 


    if(itsParsedTextVector.size()==0)
      itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight()*2);
    else
      itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF/*-6*/-2), itsFontMetrics.getHeight()*(itsParsedTextVector.size()+1));
    

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
   
    int aHeightDiff = itsFontMetrics.getHeight()*itsParsedTextVector.size()+2*HEIGHT_DIFF-currentRect.height;
    int aWidthDiff = aMaxWidth-currentRect.width;
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
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString) + 2*WIDTH_DIFF) aWidth = itsFontMetrics.stringWidth(itsMaxString) + 2*WIDTH_DIFF;
    if(aHeight<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF) aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    String temp = itsArgs;
    if((currentRect.width+theDeltaX <itsFontMetrics.stringWidth(itsMaxString) +2*WIDTH_DIFF)||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF))
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
      aOutlet.MoveTo(aOutlet.itsX, itsY+currentRect.height);
      //ReroutingConnections(aOutlet);
    }
  }

  void putOtherProperties(FtsObject theFObject){
    if (itsJustification != itsSketchPad.itsJustificationMode)
      theFObject.put("jsf", itsJustification);
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










