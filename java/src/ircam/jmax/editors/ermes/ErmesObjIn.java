package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The graphic inlet contained in subpatchers
 */
class ErmesObjIn extends ErmesObject {
  static Dimension minimumSize = new Dimension(15, 15);
  static Dimension preferredSize = new Dimension(20, 20);
  Dimension textDimensions = new Dimension();
  int itsId;
  
  public ErmesObjIn(){
    super();
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    Dimension d = getPreferredSize();

    itsId = ((FtsInletObject) theFtsObject).getPosition();

    super.Init(theSketchPad, theFtsObject); 

    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
  //We need here the information about the maximum number of inlets

    itsSketchPad = theSketchPad;  // (fd) itsSketchPad is set in ErmesObject::Init

    // >>> (fd) this piece of code which was after calling super.Init(),
    // must be BEFORE it, because super.Init() calls makeFtsObject that uses member 
    // variable itsId which was not initialized.
    // i.e. 1) the constructor calls super()
    // 2) the constructor of the super class calls an abstract function defined in the subclass
    // 3) this function is called on a partially initialized object.
    // May be this is a potential source of problems ?
    int temp = theSketchPad.GetSketchWindow().itsPatcher.getNumberOfInlets();
    if (temp == 0) {//top patcher special case!
      //make the first
      temp = 1;
      GetSketchWindow().itsPatcher.setNumberOfInlets(1);
    }
    
    if (theSketchPad.inCount < temp)
      itsId = theSketchPad.inCount++;   //for now no deleting handled
    else
      itsId = temp - 1;
    // <<< (fd)

    super.Init(theSketchPad, x, y, theString);	//this was not here...
    makeCurrentRect(x, y);

    //it was here super.Init(theSketchPad, x, y, theString);	//set itsX, itsY

    return true;
  }
	

  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "inlet", Integer.toString(itsId));
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

    ((FtsInletObject)itsFtsObject).setPosition(itsId);
  }
  
  
  public void Paint_specific(Graphics g) {
  	
  	textDimensions.setSize(itsFontMetrics.stringWidth(""+(itsId+1)), itsFontMetrics.getHeight());	
	  	
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fillRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2);
    g.fill3DRect(getItsX()+2,getItsY()+2, getItsWidth()-4,  getItsHeight()-4, true);
    g.setColor(Color.black);
    //the box
    g.drawRect(getItsX()+0,getItsY()+ 0, getItsWidth()-1, getItsHeight()-1);
    //the triangle
    g.drawLine(getItsX()+1,getItsY()+1, getItsX()+getItsWidth()/2, getItsY()+getItsHeight()/2);
    g.drawLine(getItsX()+getItsWidth()/2,getItsY()+getItsHeight()/2, getItsX()+getItsWidth()-1, getItsY()+1);
    //g.drawString(""+(itsId+1), itsX + 7, itsY + 7+7);
    g.setFont(getFont());
    g.drawString(""+(itsId+1), getItsX()+2, getItsY()-2);
  }
	
  void ResizeToNewFont(Font theFont) {
    itsSketchPad.repaint();
  }

  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) {
      return itsSketchPad.ClickOnObject(this, evt, x, y);
    }
    else  {	//we want to choose among the different Inlet number
      itsSketchPad.itsInPop.SetNewOwner(this);
      itsSketchPad.itsInPop.show(itsSketchPad, getItsX(), getItsY());
    }
    return true;
  }

  void ChangeInletNo(int numberChoosen) {
    if (itsId != numberChoosen) {
      itsId = numberChoosen;
      redefineFtsObject();
    }
    itsSketchPad.repaint();
  }
	
    //--------------------------------------------------------
    //minimum and preferred sizes
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










