package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

import ircam.jmax.utils.*;

/**
 * The abstract base class for (object's) graphic inlet and outlets
 */
abstract public class ErmesObjInOutlet implements ErmesDrawable{
  
  protected boolean selected = false;
  protected boolean connected = false; //ignore the connected flag
  protected int itsX, itsY;
  ErmesObject itsOwner;

  protected final int UPDATE_AND_DRAW = 0;
  protected final int DRAW = 1;
  protected int itsCurrentDrawingMethod = DRAW;
  
  public MaxVector itsConnections;
  static Dimension preferredSize;
  boolean itsAlreadyMoveIn = false;
  Rectangle currentBounds = new Rectangle();
  Rectangle currentSensibleBounds = new Rectangle();
  Point itsAnchorPoint = new Point();
  

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjInOutlet(ErmesObject theOwner, int x_coord, int y_coord){
    
    itsConnections = new MaxVector();
    itsX = x_coord;
    itsY = y_coord;

    itsOwner = theOwner;
    selected = false;
    connected = false;
    preferredSize = new Dimension(7,9);

    if (IsInlet())
      itsY-=9;
    else
      itsY+= itsOwner.getItsHeight();

    recomputeBounds();
    updateAnchorPoint();
  }


  public void Paint(Graphics g) {
    if (g== null) return;
    if (itsCurrentDrawingMethod == DRAW) 
      Paint_specific(g);
    else Update(g);
  }

  abstract protected void Paint_specific(Graphics g);

  protected void DoublePaint() {
    
    Graphics aGraphics = itsOwner.itsSketchPad.getGraphics();
    if (aGraphics != null) {
      Paint(aGraphics);	
    }
    
    if (itsOwner.itsSketchPad.offScreenPresent) Paint(itsOwner.itsSketchPad.GetOffGraphics());
  }

  //--------------------------------------------------------
  //	Repaint //to be dropped?
  //--------------------------------------------------------
  void Repaint(boolean paintNow) {
    ErmesSketchPad mySketch = itsOwner.itsSketchPad;
    if (paintNow) {
      Graphics g = mySketch.getGraphics();
      Update(g);
      g.dispose();
      Update(mySketch.offGraphics);
      for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) {
	((ErmesConnection)e.nextElement()).DoublePaint();
      }
      mySketch.removeDirtyInOutlet(this);
    }
    else {
      mySketch.addToDirtyInOutlets(this);
      for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) {
	mySketch.addToDirtyConnections((ErmesConnection)e.nextElement());
      }
    }
  }

  //--------------------------------------------------------
  //	Update
  //--------------------------------------------------------
  public void Update(Graphics g) {
    int aHeight;
    if(!itsOwner.itsSketchPad.itsGraphicsOn || g == null) return;
    g.setColor(itsOwner.itsSketchPad.getBackground());
    if(IsInlet())aHeight=preferredSize.height;
    else aHeight=preferredSize.height+1;
    g.fillRect(itsX, itsY, preferredSize.width, aHeight);
    Paint_specific(g);
  }
    
  //--------------------------------------------------------
  //	Bounds
  //--------------------------------------------------------

  final Rectangle Bounds(){
    return currentBounds;
  }
  
  final Rectangle getSensibleBounds() {
    return currentSensibleBounds;
  }

  private final void recomputeBounds() {
    if (IsInlet())
      currentSensibleBounds.setBounds(itsX-2, itsY+4, preferredSize.width+4, preferredSize.height);
    else 
      currentSensibleBounds.setBounds(itsX-2, itsY-4,preferredSize.width+4, preferredSize.height);

    currentBounds.setBounds(itsX, itsY, preferredSize.width, preferredSize.height);

  }

  //--------------------------------------------------------
  //	SetConnected
  //--------------------------------------------------------
  public void SetConnected(boolean theConnected, boolean paintNow){
    ChangeState(selected, theConnected, paintNow);
  }
  
  //--------------------------------------------------------
  //	GetItsX
  //--------------------------------------------------------
  public int GetItsX(){
    return itsX; 
  }
	
  //--------------------------------------------------------
  //	GetItsY
  //--------------------------------------------------------
  public int GetItsY(){
    return itsY; 
  }
  
  //--------------------------------------------------------
  //	GetConnected
  //--------------------------------------------------------
  public boolean GetConnected(){
    return connected; 
  }
	
  public boolean GetSelected(){
    return selected;
  }
  
  //--------------------------------------------------------
  //	GetOwner
  //--------------------------------------------------------
  public ErmesObject GetOwner(){
    return itsOwner;
  }
  
  //--------------------------------------------------------
  //	GetConnections
  //--------------------------------------------------------
  public MaxVector GetConnections(){
    return itsConnections;
  }
  
  //--------------------------------------------------------
  //	IsInlet
  //--------------------------------------------------------

  // This method shoud not exists; differences between inlets
  // and outlets should not be decided here, but in the
  // code of the two classes, by specialing methods

  abstract boolean IsInlet(); /*{return true;}*/
  
  //--------------------------------------------------------
  //	GetAnchorPoint
  //--------------------------------------------------------
  Point GetAnchorPoint() {
    return itsAnchorPoint;
  }  
  
  //--------------------------------------------------------
  //	ChangeState
  //--------------------------------------------------------
  public void ChangeState(boolean theSelState, boolean theConState, boolean paintNow){
    ErmesSketchPad mySketch = itsOwner.itsSketchPad;
    boolean toPaint = false;
    if (selected != theSelState) {
      toPaint = true;
      itsCurrentDrawingMethod = UPDATE_AND_DRAW;
      selected = theSelState;
    }
    //if (connected != theConState) {
    //toPaint = true;
      connected = theConState;
      //}

    if (toPaint) {
      //if (paintNow) {

      Graphics g = mySketch.getGraphics();
      Paint(g);
      g.dispose();

      Paint(mySketch.offGraphics);
      for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) {
	((ErmesConnection)e.nextElement()).DoublePaint();	  
      }
    }
    //else {//do not paint now
    //mySketch.addToDirtyInOutlets(this);
    //for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) {
    //  mySketch.addToDirtyConnections((ErmesConnection)e.nextElement());	  
    //}
    //}    
  }
  
  //--------------------------------------------------------
  //	MoveBy
  //--------------------------------------------------------
  public void MoveBy(int theDeltaX, int theDeltaY) {
    MoveTo(itsX+theDeltaX, itsY+=theDeltaY);
  }
  
  public void MoveTo(int theX, int theY) {
    itsX=theX; itsY=theY;
    recomputeBounds();
    updateAnchorPoint();
    itsCurrentDrawingMethod = DRAW;
  }
  
  //--------------------------------------------------------
  //	AddConnection
  //--------------------------------------------------------
  public void AddConnection(ErmesConnection theConnection){
    itsConnections.addElement(theConnection);
  }
	
  //--------------------------------------------------------
  //	updateAnchorPoint
  //--------------------------------------------------------
  
  abstract void updateAnchorPoint();
	
  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize();
    
  }
  
  //--------------------------------------------------------
  //	preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
}







