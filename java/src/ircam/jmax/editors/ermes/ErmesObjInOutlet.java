package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;


/**
 * The abstract base class for (object's) graphic inlet and outlets
 */
abstract public class ErmesObjInOutlet {
  
  public boolean selected;
  public boolean connected;
  int itsX, itsY;
  ErmesObject itsOwner;
  boolean laidOut = false;
  public Vector itsConnections;
  Dimension preferredSize;
  int itsAnchorX, itsAnchorY; 
  boolean itsAlreadyMoveIn = false;
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjInOutlet(ErmesObject theOwner, int x_coord, int y_coord){
    
    itsConnections = new Vector();
    itsX = itsAnchorX = x_coord;
    itsY = itsAnchorY = y_coord;
    
    itsOwner = theOwner;
    selected = false;
    connected = false;
    preferredSize = new Dimension(7,9);
    if (IsInlet()) itsY-=9;
    else itsY+= itsOwner.currentRect.height;
  }
  
  abstract public void Paint(Graphics g);

  
  //--------------------------------------------------------
  //	Repaint
  //--------------------------------------------------------
  public void Repaint() {
    if (itsOwner.itsSketchPad.offGraphics == null) return;
    if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
    Update(itsOwner.GetSketchPad().offGraphics);
    itsOwner.GetSketchPad().DrawLinesOffScreen();
  }

  //--------------------------------------------------------
  //	Update
  //--------------------------------------------------------
  public void Update(Graphics g) {
    int aHeight;
    if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
    g.setColor(itsOwner.GetSketchPad().getBackground());
    if(IsInlet())aHeight=preferredSize.height;
    else aHeight=preferredSize.height+1;
    g.fillRect(itsX, itsY, preferredSize.width, aHeight);
    Paint(g);
  }
    
  //--------------------------------------------------------
  //	Bounds
  //--------------------------------------------------------
  public Rectangle Bounds(){
    return new Rectangle(itsX, itsY, preferredSize.width, preferredSize.height);
  }
  
  //--------------------------------------------------------
  //	SetConnected
  //--------------------------------------------------------
  public void SetConnected(boolean theConnected){
    ChangeState(selected, theConnected);
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
  public Vector GetConnections(){
    return itsConnections;
  }
  
  //--------------------------------------------------------
  //	IsInlet
  //--------------------------------------------------------
  abstract boolean IsInlet(); /*{return true;}*/
  
  //--------------------------------------------------------
  //	GetAnchorPoint
  //--------------------------------------------------------
  abstract Point GetAnchorPoint() /*{return null;}*/;  
  
  //--------------------------------------------------------
  //	ChangeState
  //--------------------------------------------------------
  public void ChangeState(boolean theSelState, boolean theConState){
    selected = theSelState;
    connected = theConState;
    Repaint();
  }
  
  //--------------------------------------------------------
  //	MoveBy
  //--------------------------------------------------------
  public boolean MoveBy(int theDeltaX, int theDeltaY) {
    itsX+=theDeltaX; itsY+=theDeltaY;
    return true;
  }
  
  public boolean MoveTo(int theX, int theY) {
    itsX=theX; itsY=theY;
    return true;
  }
  
  //--------------------------------------------------------
  //	AddConnection
  //--------------------------------------------------------
  public void AddConnection(ErmesConnection theConnection){
    itsConnections.addElement(theConnection);
  }
	
	
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




