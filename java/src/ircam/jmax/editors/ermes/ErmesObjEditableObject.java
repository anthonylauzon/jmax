package ircam.jmax.editors.ermes;


import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal).
 */
/*abstract*/ class ErmesObjEditableObject extends ErmesObject {
	
	
  final int WIDTH_DIFF = 14;
  final int HEIGHT_DIFF = 2;
	
  //protected static ErmesObjEditField itsEditField = null;
  protected int FIELD_HEIGHT;
  Dimension preferredSize = null;
  String 	  itsArgs;
  public Vector itsParsedTextVector = new Vector();
  //public int itsLineMaxWidth = 0;
  public String itsMaxString = "";
  public void setSize(int theH, int theV) {}; 
  public boolean resized = false;
  public boolean itsInEdit = true;
  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjEditableObject(){
    super();
    //itsFont = ErmesSketchPad.sketchFont;
    //itsSketchPad.getFontMetrics(itsFont);
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester) {return true;}
  public boolean ConnectionAbort(ErmesObjInOutlet theRequester) {return true;}

	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  //this 'glue' method is called when the external (or the message) have not their arguments yet
  //and we have to create also the JEditableField 
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y) {
    itsX = x; itsY = y;
    itsSketchPad = theSketchPad;
    itsFont = itsSketchPad.sketchFont;
    /* if (itsEditField == null)	{
      itsEditField = new ErmesObjEditField(this);
      itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
      FIELD_HEIGHT = itsFontMetrics.getHeight();
      itsEditField.hide();
    }*/
    // else {
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    FIELD_HEIGHT = itsFontMetrics.getHeight();
    preferredSize = new Dimension(70,FIELD_HEIGHT+2*HEIGHT_DIFF);
    itsSketchPad.GetEditField().setFont(itsFont);
    itsSketchPad.GetEditField().setText("");
    itsSketchPad.GetEditField().itsOwner = this; //redirect the only editable field to point here...
    // }
    currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
    Reshape(itsX, itsY, preferredSize.width, preferredSize.height);
    
    itsSketchPad.GetEditField().setBounds(itsX+4, itsY+1, currentRect.width-(WIDTH_DIFF-6), itsFontMetrics.getHeight() + 20);	
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
    //prova:
    /*itsX = x; itsY = y;
      itsSketchPad = theSketchPad;*/
    itsFont = itsSketchPad.sketchFont;
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont); //bugsgi*/
    FIELD_HEIGHT = itsFontMetrics.getHeight();
    preferredSize = new Dimension(70,FIELD_HEIGHT+2*HEIGHT_DIFF);
    /*currentRect = new Rectangle(x, y, preferredSize.width, preferredSize.height);
      Reshape(itsX, itsY, preferredSize.width, preferredSize.height);*/
    super.Init(theSketchPad, x, y, args);
    
    //bugsgi itsEditField.reshape(itsX+4-itsSketchPad.tx, itsY+1-itsSketchPad.ty, currentRect.width-(WIDTH_DIFF-6), currentRect.height-HEIGHT_DIFF);
    // itsEditField.reshape(itsX+4-itsSketchPad.tx, itsY+1-itsSketchPad.ty, currentRect.width-(WIDTH_DIFF-6), 
    //					itsFontMetrics.getHeight() + 20 /*?*/); //bugsgi		
    ResizeToText(0, 0);	//will it work?
    return true;
  }

  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsGraphicDescription theFtsGraphic, FtsObject theFtsObject) {
  	//the Fontmetrics problem: the ErmesObject.Init function need the currentRect already set, 
  	// but for editable objects this depends from itsFontMetrics.
  	// malheureusement, the itsFontMetrics field is built... in ErmesObject.Init !

    // MDC: itsArgs is set by the Init methods of the subclasses

    FontMetrics temporaryFM = theSketchPad.getFontMetrics(theSketchPad.getFont());
    FIELD_HEIGHT = temporaryFM.getHeight();
    //*preferredSize = new Dimension(70, FIELD_HEIGHT+2*HEIGHT_DIFF);
    int lenght = temporaryFM.stringWidth(itsArgs);	//*
    preferredSize = new Dimension(lenght+2*WIDTH_DIFF, FIELD_HEIGHT+2*HEIGHT_DIFF);	//*
    super.Init(theSketchPad, theFtsGraphic, theFtsObject);

    //*int lenght = itsFontMetrics.stringWidth(theString);
    //*Resize1(lenght+2*WIDTH_DIFF, FIELD_HEIGHT+2*HEIGHT_DIFF);	//desperado
        
    return true;
  }
	
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if(evt.getClickCount()>1) {
      if (itsSketchPad.GetEditField() != null) {
	itsSketchPad.GetEditField().setEditable(true);
	//itsParsedTextVector.removeAllElements();//??????????????????
	itsInEdit = true;
      }
    }
    else itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }
	
  public boolean MouseUp(MouseEvent evt,int x,int y){
    if(itsInEdit) return true;
    else return super.MouseUp(evt, x, y);
  }

  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    int aMaxWidth = MaxWidth(itsFontMetrics.stringWidth(itsMaxString)+WIDTH_DIFF,
			    (itsInletList.size())*12, (itsOutletList.size())*12);
   
    int aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size()+HEIGHT_DIFF-currentRect.height;

    Resize(aMaxWidth-currentRect.width, aHeight);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
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
    preferredSize = d1;	//6/06
    itsSketchPad.RemoveElementRgn(this);//???
    Resize1(d1.width, d1.height);	
    itsSketchPad.SaveOneElementRgn(this);//???
    //preferredSize = d1;//prova prova
    itsSketchPad.GetEditField().setText(theString);
    itsSketchPad.GetEditField().setSize(d1.width-WIDTH_DIFF, d1.height-HEIGHT_DIFF);
    itsSketchPad.GetEditField().repaint();
    itsSketchPad.validate();//??
    itsSketchPad.repaint();
  }
	
  void ResizeToNewFont(Font theFont) {
    if(!itsResized){
      Resize(itsFontMetrics.stringWidth(itsMaxString) + WIDTH_DIFF - currentRect.width,
	     itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF - currentRect.height);
    }
    else ResizeToText(0,0);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString) + WIDTH_DIFF) aWidth = itsFontMetrics.stringWidth(itsMaxString) + WIDTH_DIFF;
    if(aHeight<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF) aHeight = itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
  }
  
  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    String temp = itsArgs;
    if((currentRect.width+theDeltaX <itsFontMetrics.stringWidth(itsMaxString) +WIDTH_DIFF)||
       (currentRect.height+theDeltaY<itsFontMetrics.getHeight()*itsParsedTextVector.size() + HEIGHT_DIFF))
      return false;
    else return true;
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
	

  //--------------------------------------------------------
  // minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize();
  }
  
  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
	
}







