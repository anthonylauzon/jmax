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
  int itsId;

  public ErmesObjOut(){
    super();
    //setLayout(null);
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    Dimension d = getPreferredSize();

    // warning.. it seems that when the height is 0, 
    // the value is null, instead of new Integer(0)

    /*Integer aInteger = ((Integer)theFtsObject.get("h"));
      currentRect = new Rectangle(((Integer)theFtsObject.get("x")).intValue(),
      ((Integer)theFtsObject.get("y")).intValue(),
      ((Integer)theFtsObject.get("w")).intValue(),
      (aInteger == null)?preferredSize.height:
      ((Integer)theFtsObject.get("h")).intValue());*/
    
    //currentRect = new Rectangle(x, y, d.width, d.height);
    itsId = ((FtsOutletObject) theFtsObject).getPosition();

    super.Init(theSketchPad, theFtsObject); 
    theSketchPad.PrepareOutChoice();
	
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    //the sketchPad represents a subpatcher. 
    //We need here the information about the maximum number of inlets
  	
    itsSketchPad = theSketchPad;

    Dimension d = getPreferredSize();

    currentRect = new Rectangle(x, y, d.width, d.height);
    int temp = GetSketchWindow().itsPatcher.getNumberOfOutlets();

    if (theSketchPad.outCount < temp)
      itsId = theSketchPad.outCount++;   //for now no deleting handled
    else itsId = temp-1;
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY

    theSketchPad.PrepareOutChoice();
	
    return true;
  }
	
  // starting of the graphic/FTS mix

  public void makeFtsObject()
  {
    itsFtsObject = new FtsOutletObject(itsFtsPatcher, itsId);
  }

  public void redefineFtsObject()
  {
    // Inlets may redefine the arguments, but not the whole description
    // i.e. they remains Inlets

    ((FtsOutletObject)itsFtsObject).setPosition(itsId);
  }
  
  public boolean ConnectionRequested(ErmesObjInOutlet theRequester)
  {
    // HERE the checking: is the type of connection requested allowed?
    if (!theRequester.IsInlet())	//if is an outlet...
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); // then, is it's an inlet
  }

  public boolean ConnectionAbort(ErmesObjInOutlet theRequester)
  {
    // HERE the checking: is the type of connection abort allowed?
    // (for now always allowed)
    theRequester.ChangeState(false, theRequester.connected);
    itsSketchPad.ResetConnect();
    return true;	//for now, everything is allowed
  }

  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) {
      return itsSketchPad.ClickOnObject(this, evt, x, y);
    }
    else  {	//we want to choose among the different Outlet number
      itsSketchPad.itsOutPop.SetNewOwner(this); //make the Choice pointing to this
      itsSketchPad.itsOutPop.show(itsSketchPad, itsX, itsY);
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
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    g.setColor(Color.black);
    //the box
    g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
    //the triangle
    g.drawLine(itsX+1,itsY+ currentRect.height-1, itsX+currentRect.width/2, itsY+currentRect.height/2);
    g.drawLine(itsX+currentRect.width/2,itsY+ currentRect.height/2  , itsX+currentRect.width-1,itsY+currentRect.height-1);	
    //the dragBox  
    //g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    g.setFont(itsFont);
    g.drawString(""+(itsId+1), itsX+2, itsY+currentRect.height +itsFontMetrics.getHeight()-1);
  }
	
  void ResizeToNewFont(Font theFont) {
    itsSketchPad.repaint();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(15,15); //(depending on the layout manager).
  }

  //If we don't specify this, the canvas might not show up at all
  //(depending on the layout manager).
  public Dimension getPreferredSize() {
    return preferredSize;
  }

}


