package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The graphic inlet contained in subpatchers
 */
class ErmesObjIn extends ErmesObject {
  static Dimension preferredSize = new Dimension(20, 20);
  int itsId;
  //static ErmesObjInOutChoice inOutChoice = null;
  
  public ErmesObjIn(){
    super();
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    Dimension d = getPreferredSize();

    currentRect = new Rectangle(((Integer)theFtsObject.get("pos.x")).intValue(),
				((Integer)theFtsObject.get("pos.y")).intValue(),
				((Integer)theFtsObject.get("size.w")).intValue(),
				((Integer)theFtsObject.get("size.h")).intValue());


    itsId = ((FtsInletObject) theFtsObject).getPosition();

    super.Init(theSketchPad, theFtsObject); 

    theSketchPad.PrepareInChoice();
	
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
  //the sketchPad represents a subpatcher. 
  //We need here the information about the maximum number of inlets
  	
    Dimension d = getPreferredSize();
    currentRect = new Rectangle(x, y, d.width, d.height);
	int temp = ((ErmesSketchWindow)theSketchPad.itsSketchWindow).itsDocument.itsPatcher.getNumberOfInlets();

	if (theSketchPad.inCount < temp)
	  itsId = theSketchPad.inCount++;   //for now no deleting handled
	else
	  itsId = temp - 1;

    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY


    //if (theSketchPad.itsInChoice == null) {	//first inlet of the day, tell the sketch to prepare
		theSketchPad.PrepareInChoice();
    //	theSketchPad.itsInChoice.SetNewOwner(this);
	//}
	
    return true;
  }
	
  // starting of the graphic/FTS mix

  public void makeFtsObject()
  {
    Vector aVector = new Vector();

    aVector.addElement(new Integer(itsId));

    itsFtsObject = new FtsInletObject(itsFtsPatcher, itsId);
  }

  public void redefineFtsObject()
  {
    // Inlets may redefine the arguments, but not the whole description
    // i.e. they remains Inlets

    ((FtsInletObject)itsFtsObject).setPosition(itsId);
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
  
  public void Paint_specific(Graphics g) {
  	//you want to create a Dimension each Paint?
  	Dimension textDimensions = new Dimension(itsFontMetrics.stringWidth(""+(itsId+1)),
  										 itsFontMetrics.getHeight());	
	  	
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    g.setColor(Color.black);
    g.drawRect(itsX+0,itsY+ 0, currentRect.width-1, currentRect.height-1);
    //first line
    //g.drawLine(itsX+1,itsY+currentRect.height/2,itsX+ currentRect.width/2, itsY+currentRect.height-1);
    //second line
    //g.drawLine(itsX+ currentRect.width/2, itsY+currentRect.height-1, itsX+currentRect.width-2,itsY+currentRect.height/2);
	//resize box
    //g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
		
	g.drawString(""+(itsId+1), itsX + 7, itsY + 7+7);
  }
	
  public boolean MouseDown_specific(MouseEvent evt, int x, int y) {
    
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) {
      return itsSketchPad.ClickOnObject(this, evt, x, y);
    }
    else  {	//we want to choose among the different Inlet number
      itsSketchPad.itsInChoice.SetNewOwner(this); //make the Choice pointing to this
      itsSketchPad.itsInChoice.setLocation(x, y);
      itsSketchPad.itsInChoice.setVisible(true);
      //itsSketchPad.itsInChoice.mouseDown(evt, x, y);
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
	// minimumSize()
    //--------------------------------------------------------
    public Dimension getMinimumSize() {
        return getPreferredSize(); //(depending on the layout manager).
    }

    //If we don't specify this, the canvas might not show up at all
    //(depending on the layout manager).
    public Dimension getPreferredSize() {
        return preferredSize;
    }

}
