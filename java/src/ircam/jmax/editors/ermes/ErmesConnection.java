package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import java.lang.Math;

/**
 * The graphic connection. It handles the paint, the autorouting, 
 * the loading/saving of connections. It has user interface
 * methods (mouseclick, move, doubleclick...).
 */ 

public class ErmesConnection implements ErmesDrawable{
	
  public ErmesObjInlet itsInlet;
  public ErmesObjOutlet itsOutlet;
  int itsInletNum;
  int itsOutletNum;
  //Point itsStartPoint, itsEndPoint;
  ErmesSketchPad itsSketchPad;
  FtsConnection itsFtsConnection;
  ErmesObject	itsFromObject;
  ErmesObject	 itsToObject;
  boolean itsErrorState;
  boolean itsSelected;
  static int debug_count = 1;
  boolean mustSayToSketch = false;	//when it is ready
  boolean itsDirtyFlag;
  
  final int DELTA = 6;
  
  public Point getStartPoint() {return itsOutlet.GetAnchorPoint();}
  public Point getEndPoint() {return itsInlet.GetAnchorPoint();}
  public ErmesSketchPad GetSketchPad(){return itsSketchPad;}
  public ErmesObjInlet GetInlet() {return itsInlet;}
  public ErmesObjOutlet GetOutlet() {return itsOutlet;}
  public boolean GetSelected() {return itsSelected;}
  public ErmesObject GetSourceObject(){return itsOutlet.GetOwner();}
  public ErmesObject GetDestObject(){return itsInlet.GetOwner();}

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //	NOTE this constructor builds also an instance of FtsConnection. It is intended
  //	to be used from user-driven connections, unlike the second constructor, which receives
  //	the FtsConnection. This is going to change in a middle-term (4/03/97)
  //--------------------------------------------------------
  public ErmesConnection(ErmesSketchPad theSketchPad, ErmesObjInlet theInlet, ErmesObjOutlet theOutlet) throws FtsException{
		
    itsSketchPad = theSketchPad;
    itsInlet = theInlet;
    itsOutlet = theOutlet;
    itsFromObject  = itsOutlet.GetOwner();
    itsToObject  = itsInlet.GetOwner();

    try
      {
	itsFtsConnection = Fts.makeFtsConnection(itsFromObject.itsFtsObject,theOutlet.GetOutletNum(), 
					     itsToObject.itsFtsObject, theInlet.GetInletNum());
	itsFtsConnection.setRepresentation(this);
      }
    catch (FtsException e)
      {
	// ????????
	System.out.println("Connection Error");
	Toolkit.getDefaultToolkit().beep();
	throw e;
      }

    itsSelected = false;
  }

  public boolean getDirty() {
    //return itsDirtyFlag;
    return true;//for now, just for test
  }

  public void setDirty(boolean b) {
    itsDirtyFlag = b;
  }

  //--------------------------------------------------------
  //	Repaint
  //--------------------------------------------------------
  public void Repaint() {
    Update(itsSketchPad.GetOffGraphics());
    Paint(itsSketchPad.GetOffGraphics());
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());
  }
  
  //--------------------------------------------------------
  //	Update
  //--------------------------------------------------------
  public void Update(Graphics g) {
    	
    if(!itsSketchPad.itsGraphicsOn) return;
    Rectangle aRect;
    Point start = getStartPoint();
    Point end = getEndPoint();

    g.setColor(itsSketchPad.getBackground());
    //if is not autorouted (or is an error connection)
    if(java.lang.Math.abs(start.x-end.x)>50){
      g.drawLine(start.x, start.y, end.x, end.y);
      g.drawLine(start.x, start.y+1, end.x, end.y+1);	
    }
    else{
      g.drawLine(start.x, start.y, end.x, end.y);
      g.drawLine(start.x-1, start.y, end.x-1, end.y);	
    }
    
  }

  public ErmesConnection(ErmesSketchPad theSketchPad, ErmesObjInlet theInlet, ErmesObjOutlet theOutlet, FtsConnection theFtsConnection){
		
    itsSketchPad = theSketchPad;
    itsInlet = theInlet;
    itsOutlet = theOutlet;
    itsFtsConnection = theFtsConnection;
    itsSelected = false;
    itsFtsConnection.setRepresentation(this);
  }

  public ErmesConnection(ErmesObject fromObj, ErmesObject toObj, ErmesSketchPad theSketchPad, int theOutlet, int theInlet, FtsConnection theFtsConnection){
    itsFtsConnection = theFtsConnection;
    itsFromObject = fromObj;
    itsToObject = toObj;
    itsSketchPad = theSketchPad;
    itsInletNum = theInlet;
    itsOutletNum = theOutlet;
    itsSelected = false;
    itsFtsConnection.setRepresentation(this);
  }
  
  public void update(FtsConnection theFtsConnection) {
    
    itsFtsConnection = theFtsConnection;
    itsFtsConnection.setRepresentation(this);
    itsInlet = (ErmesObjInlet) itsToObject.itsInletList.elementAt(itsInletNum);
    itsOutlet = (ErmesObjOutlet) itsFromObject.itsOutletList.elementAt(itsOutletNum);
    itsOutlet.AddConnection(this);
    itsOutlet.ChangeState(false,true, false);
    itsInlet.AddConnection(this); 
    itsInlet.ChangeState(false,true, false);
    itsSketchPad.itsConnections.addElement(this);
    
  }
  

  //--------------------------------------------------------
  //	Select
  //  select a connection
  //--------------------------------------------------------
  public boolean Select(boolean paintNow){
    if (!itsSelected) {
      itsSelected = true;
      if (paintNow) DoublePaint();
      else itsSketchPad.addToDirtyConnections(this);
      return true;
    }
    return false;
  }
	
  //--------------------------------------------------------
  //	Deselect
  //	deselect a connection
  //--------------------------------------------------------
  public boolean Deselect(boolean paintNow){
    if (itsSelected) {
      itsSelected = false;
      itsSketchPad.markSketchAsDirty();
      if (paintNow) itsSketchPad.paintDirtyList();
      return true;
    }
    return false;
  }
	
  public boolean MouseDown(MouseEvent evt,int x, int y) {
    if (!itsSketchPad.itsRunMode){
      itsSketchPad.ClickOnConnection(evt, x, y);
    }
    return true;
  }
	
  ////////////////////////// codice rubato senza nessuno scrupolo
	
  boolean IsNearToPoint(int x, int y){

    Point start = getStartPoint();
    Point end = getEndPoint();

      int dx = java.lang.Math.abs(end.x - start.x);
      int dy = java.lang.Math.abs(end.y - start.y);
      
      if (dx > dy) return (SegmentIntersect(x, y-3, x, y+3, start.x,
					    start.y,end.x, end.y));
      else return (SegmentIntersect(x-3, y, x+3, y, start.x, start.y,
				    end.x, end.y));
  }


  private boolean SegmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    int dx21 = x2 - x1;
    int dx31 = x3 - x1;
    int dx41 = x4 - x1;
    int dy21 = y2 - y1;
    int dy31 = y3 - y1;
    int dy41 = y4 - y1;
    int det1 = dx31*dy21 - dy31*dx21;
    int det2 = dx41*dy21 - dy41*dx21;
		
    if ((det1 == 0)&&(det2 == 0)){
      if ((y1 == y2)&&(y1 == y3)&&(y1 == y4))
	return (Scambio(x1, x2, x3, x4));
      else return (Scambio(y1, y2, y3, y4));
    }
    else {
      int dx32 = x3 - x2;
      int dx34 = x3 - x4;
      int dy32 = y3 - y2;
      int dy34 = y3 - y4;
      int det3 = dx34*dy31 - dy34*dx31;
      int det4 = dx34*dy32 - dy34*dx32;
      return ((((det1<=0)&&(det2>=0))||((det1>=0)&&(det2<=0)))&&
	      (((det3<=0)&&(det4>=0))||((det3>=0)&&(det4<=0))));
    }
  }

  boolean Scambio(int x1, int x2, int x3, int x4){
    int temp;
    if (x2 < x1){
      temp = x2;
      x2 = x1; 
      x1 = temp;
    }
    if (x4 < x3){
      temp = x4; 
      x4 = x3; 
      x3 = temp;
    }
    return ((x2 > x3)&&(x4 > x1));
  }
  ///////////////////////	
	
  void DoublePaint(){
    //this double paint is usefull when an object schange its state in run mode
    Graphics aGraphics = itsSketchPad.getGraphics();

    if (aGraphics != null) {	
      Paint(aGraphics);	
    }

    if (itsSketchPad.offScreenPresent)
      Paint(itsSketchPad.GetOffGraphics());
  }

  public void Paint(Graphics g){
    if(!itsSketchPad.itsGraphicsOn) return;
    Point start = getStartPoint();
    Point end = getEndPoint();

    g.setColor(Color.black);
    if(itsSelected){
      if(java.lang.Math.abs(start.x-end.x)>50){
	g.drawLine(start.x, start.y, end.x, end.y);
	g.drawLine(start.x, start.y+1, end.x, end.y+1);	
      }
      else{
	g.drawLine(start.x, start.y, end.x, end.y);
	g.drawLine(start.x-1, start.y, end.x-1, end.y);	
      }
    }
    else
      g.drawLine(start.x, start.y, end.x, end.y);
  }
}
