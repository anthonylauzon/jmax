package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "out" graphic object used in subpatchers.
 */
class ErmesObjOut extends ErmesObject {

  static Dimension preferredSize = new Dimension(20, 20);
  static Dimension minimumSize = new Dimension(15,15); 
  int itsId;

  public ErmesObjOut(){
    super();
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    //Dimension d = getPreferredSize();


    itsId = ((FtsOutletObject) theFtsObject).getPosition();

    super.Init(theSketchPad, theFtsObject); 
	
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    //We need here the information about the maximum number of outlets
  	
    itsSketchPad = theSketchPad;  // (fd) itsSketchPad is set in ErmesObject::Init

    // (fd) {
    // See ErmesObjIn.java for comments.
    int temp = theSketchPad.GetSketchWindow().itsPatcher.getNumberOfInlets();
    
    if (theSketchPad.outCount < temp)
      itsId = theSketchPad.outCount++;   //for now no deleting handled
    else
      itsId = temp - 1;
    // } (fd)

    super.Init(theSketchPad, x, y, theString);	//it was not here...

    makeCurrentRect(x, y);
	
    return true;
  }
	

  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "outlet", Integer.toString(itsId));
      }
    catch (FtsException e)
      {
	// ENZO !!!! AIUTO :->
	System.out.println("Error in Object Instantiation");
      }
  }

  public void redefineFtsObject()
  {
    // Inlets may redefine the arguments, but not the whole description
    // i.e. they remains Inlets

    ((FtsOutletObject)itsFtsObject).setPosition(itsId);
  }
  

  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) {
      return itsSketchPad.ClickOnObject(this, evt, x, y);
    }
    else  {	//we want to choose among the different Outlet number
      itsSketchPad.itsOutPop.SetNewOwner(this); //make the Choice pointing to this
      itsSketchPad.itsOutPop.show(itsSketchPad, getItsX(), getItsY());
    }
    return true;
  }

  void ChangeOutletNo(int numberChoosen) {
    if (itsId != numberChoosen) {
      itsId = numberChoosen;
      redefineFtsObject();
    }
    itsSketchPad.repaint();
  }

  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fillRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2);
    g.fill3DRect(getItsX()+2,getItsY()+2, getItsWidth()-4,  getItsHeight()-4, true);
    g.setColor(Color.black);
    //the box
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    //the triangle
    g.drawLine(getItsX()+1,getItsY()+ getItsHeight()-1, getItsX()+getItsWidth()/2, getItsY()+getItsHeight()/2);
    g.drawLine(getItsX()+getItsWidth()/2,getItsY()+ getItsHeight()/2  , getItsX()+getItsWidth()-1,getItsY()+getItsHeight()-1);	
    //the dragBox  
    //g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    g.setFont(getFont());
    g.drawString(""+(itsId+1), getItsX()+2, getItsY()+getItsHeight() +itsFontMetrics.getHeight()-1);
  }
	
  void ResizeToNewFont(Font theFont) {
    itsSketchPad.repaint();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return minimumSize;
  }

  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    if(aWidth<getMinimumSize().width) aWidth = getMinimumSize().width;
    if(aHeight<getMinimumSize().height) aHeight = getMinimumSize().height;
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
}


