package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.mda.*;

/**
 * The generic "extern" object in ermes. (example: adc1~) 
 */
public class ErmesObjExternal extends ErmesObjEditableObject {

  public boolean iAmPatcher = false;
  private String itsBackupText = new String();

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
    if(!IsResizeTextCompat(0,0)) RestoreDimensions(false);

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
    try{
      itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, itsArgs);
    }
    catch (FtsException e){
      Toolkit.getDefaultToolkit().beep();
      System.out.println("Cannot create object: " + itsArgs);
      
    }
    if (itsFtsObject instanceof FtsContainerObject)
      YouArePatcher(true);
  }

  public void redefineFtsObject()
  {
    GetSketchWindow().itsPatcher.watch("deleteConnection", GetSketchWindow());

    try
      {
	itsFtsObject = Fts.redefineFtsObject(itsFtsObject, itsArgs);
      }
    catch (FtsException e)
      {
	System.out.println("Error in redefining object, action cancelled");
	Toolkit.getDefaultToolkit().beep();
	restoreText();
	ParseText(itsArgs);
      }

    this.YouArePatcher(itsFtsObject instanceof FtsContainerObject);

    GetSketchWindow().itsPatcher.removeWatch("deleteConnection", GetSketchWindow());
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  
   public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
     if(evt.getClickCount()>1) {
       
       if(itsFtsObject instanceof FtsObjectWithData){
	 try{
	   // New !!! Actually the same thing can be done for 
	   // patchers now !!!
	   
	   MaxData data;
	   
	   data = ((FtsObjectWithData) itsFtsObject).getData();
	   
	   Mda.edit(data);
	 }
	 catch (MaxDocumentException e)
	   {
	     // SHould do something better
	     System.err.println(e);
	   }
       }
     }
     else if (!itsSketchPad.itsRunMode) itsSketchPad.ClickOnObject(this, evt, x, y);
     return true;
   }

  public void RestartEditing() {
    // if((iAmPatcher)&&(itsSubWindow != null)){
    // GetSketchWindow().CreateFtsGraphics(itsSubWindow);
    // //itsSubWindow.dispose();
    // //itsSubWindow = null;
    // }
    super.RestartEditing();
  }
  

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  int paintCount = 0;
  public void Paint_specific(Graphics g) {
    //System.err.println(". "+(paintCount++));
    //(new Throwable()).printStackTrace();
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);

    g.fillRect(itsX+1,itsY+1,currentRect.width-2, currentRect.height-2);
    g.fill3DRect(itsX+2, itsY+2, currentRect.width-4, currentRect.height-4, true);
    
    if(!itsSelected) g.setColor(Color.white);
    else g.setColor(itsLangNormalColor);
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
  /*public void setSize(int theH, int theV) {
    Dimension d = new Dimension(theH, theV);
    super.Resize1(d.width, d.height);
    currentRect.setSize(d.width, d.height);
    d.width -= WIDTH_DIFF;		
    d.height -= HEIGHT_DIFF;
    if (itsSketchPad != null) itsSketchPad.repaint();
  }*/
  
  /*public void setSize(Dimension d) {
    setSize(d.width, d.height);
  }*/
}





