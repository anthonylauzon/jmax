package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import java.lang.Math;

/**
 * The graphic connection. It handles the paint, the autorouting, 
 * the loading/saving of connections. It has user interface
 * methods (mouseclick, move, doubleclick...).
 */ 

public class ErmesConnection {
	
  public ErmesObjInlet itsInlet;
  public ErmesObjOutlet itsOutlet;
  int itsInletNum;
  int itsOutletNum;
  Vector itsPoints;
  Vector itsHSegmentList;
  Vector itsVSegmentList; 
  Point itsStartPoint, itsEndPoint;
  ErmesSketchPad itsSketchPad;
  ErmesConnectionSet itsConnectionSet;
  public FtsConnection itsFtsConnection;
  ErmesObject	itsFromObject;
  ErmesObject	 itsToObject;
  boolean itsErrorState;
  boolean itsSelected;
  public boolean itsAutorouted;
  static int debug_count = 1;
  boolean mustSayToSketch = false;	//when it is ready
  
  final int DELTA = 6;
  
  /////////////////////////
  
  static Point itsTempPt1 = new Point(0,0);
  static Point itsTempPt2 = new Point(0,0);
  static Point itsTempPt3 = new Point(0,0);
  static Point itsTempPt4 = new Point(0,0);
  static Point itsTempPt5 = new Point(0,0);
  static Point itsTempPt6 = new Point(0,0);
  static Point itsTempPt21 = new Point(0,0);
  static Point itsTempPt22 = new Point(0,0);
  static Point itsTempPt23 = new Point(0,0);
  static Point itsTempPt24 = new Point(0,0);
  static Point itsTempPt25 = new Point(0,0);
  static Point itsTempPt26 = new Point(0,0);
  
  static Rectangle itsRect1 = new Rectangle();
  static Rectangle itsRect2 = new Rectangle();
  static Rectangle itsRect3 = new Rectangle();
  static Rectangle itsRect4 = new Rectangle();
  static Rectangle itsRect5 = new Rectangle();
  static Rectangle itsRectTemp1 = new Rectangle();
  static Rectangle itsRectTemp2 = new Rectangle();
  static Rectangle itsRectTemp3 = new Rectangle();
  static Rectangle itsRectTemp4 = new Rectangle();
  static Rectangle itsRectTemp5 = new Rectangle();
  static Rectangle itsRectTemp21 = new Rectangle();
  static Rectangle itsRectTemp22 = new Rectangle();
  static Rectangle itsRectTemp23 = new Rectangle();
  static Rectangle itsRectTemp24 = new Rectangle();
  static Rectangle itsRectTemp25 = new Rectangle();
  
  /////////////////////////
  
  public Vector GetHSegmentList() {return itsHSegmentList;}
  public Vector GetVSegmentList() {return itsVSegmentList;}
  public Vector GetPointList() {return itsPoints;}
  public Point GetStartPoint() {return itsStartPoint;}
  public Point GetEndPoint() {return itsEndPoint;}
  public ErmesSketchPad GetSketchPad(){return itsSketchPad;}
  public boolean GetErrorState() {return itsErrorState;}
  public ErmesConnectionSet GetConnectionSet() {return (itsConnectionSet);}
  public ErmesObjInlet GetInlet() {return itsInlet;}
  public ErmesObjOutlet GetOutlet() {return itsOutlet;}
  public boolean GetSelected() {return itsSelected;}
  
	

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //	NOTE this constructor builds also an instance of FtsConnection. It is intended
  //	to be used from user-driven connections, unlike the second constructor, which receives
  //	the FtsConnection. This is going to change in a middle-term (4/03/97)
  //--------------------------------------------------------
  public ErmesConnection(ErmesSketchPad theSketchPad, ErmesObjInlet theInlet, ErmesObjOutlet theOutlet, boolean doAutorouting){
		
    Point theStart = theOutlet.GetAnchorPoint();
    Point theEnd = theInlet.GetAnchorPoint();
    itsStartPoint = new Point(theStart.x, theStart.y);
    itsEndPoint = new Point(theEnd.x, theEnd.y);
    itsPoints = new Vector();
    itsHSegmentList = new Vector();
    itsVSegmentList = new Vector();
    itsConnectionSet = theOutlet.itsConnectionSet;
    
    itsSketchPad = theSketchPad;
    itsInlet = theInlet;
    itsOutlet = theOutlet;
    itsFromObject  = itsOutlet.GetOwner();
    itsToObject  = itsInlet.GetOwner();
    itsErrorState = false;
    itsFtsConnection = new FtsConnection(itsFromObject.itsFtsObject,theOutlet.GetOutletNum(), 
					 itsToObject.itsFtsObject, theInlet.GetInletNum());
    itsSelected = false;
    itsAutorouted = doAutorouting;
    if(itsAutorouted) AutoRouting();
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
    ErmesConnSegment aSegment;
    Rectangle aRect;
    
    g.setColor(itsSketchPad.getBackground());
    if((itsAutorouted)&&(!itsErrorState)){
      for (Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();) {
	aSegment = (ErmesConnSegment)e.nextElement();
	aRect = aSegment.Bounds();
	g.fillRect(aRect.x, aRect.y, aRect.width, aRect.height);
      }
      for (Enumeration e1 = itsHSegmentList.elements(); e1.hasMoreElements();) {
	aSegment = (ErmesConnSegment)e1.nextElement();
	aRect = aSegment.Bounds();
	g.fillRect(aRect.x, aRect.y, aRect.width, aRect.height);
      }
    }
    else{//if is not autorouted (or is an error connection)
      if(java.lang.Math.abs(itsStartPoint.x-itsEndPoint.x)>50){
	g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
	g.drawLine(itsStartPoint.x, itsStartPoint.y+1, itsEndPoint.x, itsEndPoint.y+1);	
      }
      else{
	g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
	g.drawLine(itsStartPoint.x-1, itsStartPoint.y, itsEndPoint.x-1, itsEndPoint.y);	
      }
    }
  }

  public void GimmeACall() {
    mustSayToSketch = true;	
  }
  
  public boolean IsAutorouted() {
    return itsAutorouted;
  }
	
  /* start e.m. 4/03 */
  public ErmesConnection(ErmesSketchPad theSketchPad, ErmesObjInlet theInlet, ErmesObjOutlet theOutlet, FtsConnection theFtsConnection, boolean theAutorouted){
		
    Point theStart = theOutlet.GetAnchorPoint();
    Point theEnd = theInlet.GetAnchorPoint();
    itsStartPoint = new Point(theStart.x, theStart.y);
    itsEndPoint = new Point(theEnd.x, theEnd.y);
    itsPoints = new Vector();
    itsHSegmentList = new Vector();
    itsVSegmentList = new Vector();
    itsConnectionSet = theOutlet.itsConnectionSet;
    
    itsSketchPad = theSketchPad;
    itsInlet = theInlet;
    itsOutlet = theOutlet;
    itsErrorState = false;
    itsFtsConnection = theFtsConnection;
    itsSelected = false;
    itsAutorouted = theAutorouted;
  }
		/* end e.m. 4/03 */
  public ErmesConnection(ErmesObject fromObj, ErmesObject toObj, ErmesSketchPad theSketchPad, int theOutlet, int theInlet, FtsConnection theFtsConnection, boolean theAutorouted){
    itsFtsConnection = theFtsConnection;
    itsFromObject = fromObj;
    itsToObject = toObj;
    itsSketchPad = theSketchPad;
    itsInletNum = theInlet;
    itsOutletNum = theOutlet;
    itsPoints = new Vector();
    itsHSegmentList = new Vector();
    itsVSegmentList = new Vector();
    itsSelected = false;
    itsErrorState = false;
    itsAutorouted = theAutorouted;
  }
  
  public void update(FtsConnection theFtsConnection) {
    
    itsFtsConnection = theFtsConnection;
    itsInlet = (ErmesObjInlet) itsToObject.itsInletList.elementAt(itsInletNum);
    itsOutlet = (ErmesObjOutlet) itsFromObject.itsOutletList.elementAt(itsOutletNum);
    Point theStart = itsOutlet.GetAnchorPoint();
    Point theEnd = itsInlet.GetAnchorPoint();
    itsStartPoint = new Point(theStart.x, theStart.y);
    itsEndPoint = new Point(theEnd.x, theEnd.y);
    itsConnectionSet = itsOutlet.itsConnectionSet;
    itsOutlet.AddConnection(this);
    itsOutlet.ChangeState(false,true);
    itsInlet.AddConnection(this); 
    itsInlet.ChangeState(false,true);
    if(!GetErrorState()) {
      itsSketchPad.SaveConnectionRgn(this);
      itsOutlet.itsConnectionSet.Add(this);
    }
    itsSketchPad.itsConnections.addElement(this);
    
    if(itsAutorouted) AutoRouting();
  }
  
  public boolean GetAutorouted(){
    return itsAutorouted;
  }
  
  //////// versione semplificata dell'autorouting con un massimo di solo cinque segmenti...
  public boolean AutoRouting(){		
    if(!ThreeSegmRouting()){
      if(FiveSegmRouting())
	return true;
      else{
	itsErrorState = true;
	//ErrorConnectionRouting();
	return false;
      }
    }
    return true;
  }	
	
	
	
  //--------------------------------------------------------
  //	ErrorConnectionRouting
  //	three segments connections
  //--------------------------------------------------------
  public void ErrorConnectionRouting(){
    
    Point aPoint1, aPoint2,aPoint3;
    ErmesConnSegment aSegment1;
    ErmesConnSegment aSegment2;
																			
    aPoint1 = new Point(itsStartPoint.x, itsStartPoint.y);
    aPoint2 = new Point(itsStartPoint.x, itsEndPoint.y);
    aPoint3 = new Point(itsEndPoint.x, itsEndPoint.y);
    
    aSegment1 = new ErmesConnSegment(aPoint1, aPoint2, this);	
    aSegment2 = new ErmesConnSegment(aPoint2, aPoint3, this);
    
    itsPoints.addElement(aPoint1);
    itsPoints.addElement(aPoint2);
    itsPoints.addElement(aPoint3);
    
    itsVSegmentList.addElement(aSegment1);
    itsHSegmentList.addElement(aSegment2);
  }
  
  //--------------------------------------------------------
  //	ThreeSegmRouting
  //	three segments connections
  //--------------------------------------------------------
  public boolean ThreeSegmRouting()
  {	
    Point aPoint1 = new Point(0,0);
    Point aPoint2 = new Point(0,0);
    Point aPoint3 = new Point(0,0);
    Point aPoint4 = new Point(0,0);
		
    ErmesConnSegment aSegment1;
    ErmesConnSegment aSegment2;
    ErmesConnSegment aSegment3;
    
    int step = 5;
    boolean error = false;
		
    if (itsStartPoint.y < itsEndPoint.y)// case 1 sorg upper than dest
      {																	
	if(itsEndPoint.y - itsStartPoint.y >= 20)//if the vertical distance between sorg and dest is greater than 20 pixels
	  {
	    itsTempPt1.x = itsStartPoint.x;//sorgente
	    itsTempPt1.y = itsStartPoint.y;
	    itsTempPt2.y = itsEndPoint.y -DELTA;
	    itsTempPt2.x = itsStartPoint.x;
	    itsTempPt3.y = itsEndPoint.y -DELTA;
	    itsTempPt3.x = itsEndPoint.x;
	    itsTempPt4.x = itsEndPoint.x;//destinazione
	    itsTempPt4.y = itsEndPoint.y;
	    
	    if (itsStartPoint.x < itsEndPoint.x) //case 1.1 sorg x dest
	      {
		itsRect1.y = itsTempPt1.y + /*8*/6 -1;//segue correzione segment1
		itsRect1.x = itsTempPt1.x-1;
		itsRect1.height = itsTempPt2.y -itsTempPt1.y +2-/*8*/6;
		itsRect1.width = 3;
		itsRect2.y = itsTempPt2.y-1;
		itsRect2.x = itsTempPt2.x-1;
		itsRect2.height = 3;
		itsRect2.width = itsTempPt3.x - itsTempPt2.x +2;
		itsRect3.y = itsTempPt3.y -1;
		itsRect3.x = itsTempPt3.x -1;
		itsRect3.height = itsTempPt4.y - itsTempPt3.y +2-/*8*/6;//segue correzione segment3
		itsRect3.width = 3;
		
		////// verso l'alto  ////////////////
		for(int k1 = 0;((IntersectRgn(itsRect1, itsStartPoint))||
				(IntersectRgn(itsRect2, itsStartPoint))&&
				(k1<itsTempPt3.y - itsTempPt1.y -DELTA)); k1+=step)		   
		  {
		    itsRect1.height-=step;
		    itsRect2.y-=step;
		    itsRect3.y-=step;
		    itsRect3.height+=step;
		  }
		itsTempPt2.y = itsTempPt3.y = itsRect2.y + 1;
	      }
	    else //  Case 1.2 sorg right dest
	      {	
		itsRect1.y = itsTempPt1.y + /*8*/6 -1;//segue correzione segment1
		itsRect1.x = itsTempPt1.x-1;
		itsRect1.height = itsTempPt2.y -itsTempPt1.y +2-/*8*/6;
		itsRect1.width = 3;
		itsRect2.y = itsTempPt3.y-1;
		itsRect2.x = itsTempPt3.x-1;
		itsRect2.height = 3;
		itsRect2.width = itsTempPt2.x - itsTempPt3.x +2;
		itsRect3.y = itsTempPt3.y -1;
		itsRect3.x = itsTempPt3.x -1;
		itsRect3.height = itsTempPt4.y -itsTempPt3.y +2-/*8*/6;//segue correzione segment5
		itsRect3.width = 3;
					
		for(int k1 = 0;((IntersectRgn(itsRect1, itsStartPoint))||
				(IntersectRgn(itsRect2, itsStartPoint))&&
				(k1<itsTempPt3.y - itsTempPt1.y -DELTA)); k1+=step)
		  {
		    itsRect1.height-=step;
		    itsRect2.y-=step;
		    itsRect3.height+=step;
		    itsRect3.y-=step;
		  }
		itsTempPt2.y = itsTempPt3.y = itsRect2.y + 1;
	      }
	    if((IntersectRgn(itsRect1, itsStartPoint))||(IntersectRgn(itsRect2, itsStartPoint))||
	       (IntersectElement(itsRect3)))
	      error = true;
	  }
	else//se sorg e dest sono verticalmente troppo vicini (trattato negli altri autorouting)
	  return false;
	
	if(!error){
	  aPoint1 = new Point(itsTempPt1.x, itsTempPt1.y);
	  aPoint2 = new Point(itsTempPt2.x, itsTempPt2.y);
	  aPoint3 = new Point(itsTempPt3.x, itsTempPt3.y);
	  aPoint4 = new Point(itsTempPt4.x, itsTempPt4.y);
	  
	  itsRect1.y-= /*8*/6;//correzione segment1
	  itsRect3.height+=/*8*/6;//correzione segment7
	  //first segment
	  
	  aSegment1 = new ErmesConnSegment(aPoint1, aPoint2, this);	
	  aSegment2 = new ErmesConnSegment(aPoint2, aPoint3, this);
	  aSegment3 = new ErmesConnSegment(aPoint3, aPoint4, this);
	  
	  itsPoints.addElement(aPoint1);
	  itsPoints.addElement(aPoint2);
	  itsPoints.addElement(aPoint3);
	  itsPoints.addElement(aPoint4);
	  
	  itsVSegmentList.addElement(aSegment1);
	  itsHSegmentList.addElement(aSegment2);
	  itsVSegmentList.addElement(aSegment3);
	  
	  return true;
	}
	else
	  return false;
      }//fine caso 1
    else//il caso due viene trattato solo nel routing a cinque
      return false;
  }
	
	
  //--------------------------------------------------------
  //	FiveSegmRouting
  //	five segments connections
  //--------------------------------------------------------
  public boolean FiveSegmRouting(){
    Point aPoint1 = new Point(0,0);Point aPoint2 = new Point(0,0);
    Point aPoint3 = new Point(0,0);Point aPoint4 = new Point(0,0);
    Point aPoint5 = new Point(0,0);Point aPoint6 = new Point(0,0);
    
    ErmesConnSegment aSegment1,aSegment2, aSegment3, aSegment4, aSegment5;
    
    int step = 5;
    int k1, k2, k3, k4, k6, k7;
    int temp;
    int spostamento1, spostamento2;
    spostamento1 = spostamento2 = 0;
    boolean error1, error2;
    error1 = error2 = false;
    boolean noAutoRouting = false;
    boolean inverted = false;
    
    
    if (itsStartPoint.y < itsEndPoint.y){// case 1 sorg upper than dest																	
      if(itsEndPoint.y - itsStartPoint.y >= 20){//if the vertical distance between sorg and dest is greater than 20 pixels
	if (itsStartPoint.x < itsEndPoint.x){//case 1.1 sorg x dest
	  //prova verso l'alto
	  
	  itsTempPt1.y = itsStartPoint.y;//sorgente
	  itsTempPt1.x = itsStartPoint.x;
	  itsTempPt2.y = itsStartPoint.y +DELTA;
	  itsTempPt2.x = itsStartPoint.x;
	  itsTempPt3.y = itsStartPoint.y +DELTA;
	  itsTempPt3.x = itsStartPoint.x + 25;
	  itsTempPt4.y = itsStartPoint.y +DELTA;
	  itsTempPt4.x = itsStartPoint.x + 25;
	  itsTempPt5.x = itsEndPoint.x;
	  itsTempPt5.y = itsStartPoint.y +DELTA;
	  itsTempPt6.x = itsEndPoint.x;//destinazione
	  itsTempPt6.y = itsEndPoint.y;
	  
	  
	  itsRectTemp1.y = itsTempPt1.y + /*8*/6 - 1;//segue correzione segment1
	  itsRectTemp1.x = itsTempPt1.x-1;
	  itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y +2-/*8*/6;//segue correzione segment1
	  itsRectTemp1.width = 3;
	  itsRectTemp2.y = itsTempPt2.y-1;
	  itsRectTemp2.x = itsTempPt2.x-1;
	  itsRectTemp2.height = 3;
	  itsRectTemp2.width = itsTempPt3.x - itsTempPt2.x +2;
	  itsRectTemp3.y = itsTempPt4.y -1;
	  itsRectTemp3.x = itsTempPt4.x -1;
	  itsRectTemp3.height = 3;
	  itsRectTemp3.width = 3;
	  itsRectTemp4.y = itsTempPt4.y -1;
	  itsRectTemp4.x = itsTempPt4.x -1;
	  itsRectTemp4.height = 3;
	  itsRectTemp4.width = itsTempPt5.x - itsTempPt4.x +2;
	  itsRectTemp5.y = itsTempPt5.y -1;
	  itsRectTemp5.x = itsTempPt5.x -1;
	  itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp5.width = 3;
	  
	  //limitation
	  for(k1 = 0;((IntersectRgn(itsRectTemp4, itsStartPoint))&&(itsRectTemp4.y>0)); k1+=step){
	    itsRectTemp3.y-=step;
	    itsRectTemp3.height+=step;
	    itsRectTemp4.y-=step;
	    itsRectTemp5.y-=step;
	    itsRectTemp5.height+=step;
	  }
	  itsTempPt4.y = itsTempPt5.y = itsRectTemp4.y+1;
	  spostamento1+=k1;
	  
	  if((IntersectRgn(itsRectTemp1, itsStartPoint))||(IntersectRgn(itsRectTemp2, itsStartPoint))||
	     (IntersectRgn(itsRectTemp3, itsStartPoint))||(IntersectRgn(itsRectTemp4, itsStartPoint))||
	     (IntersectElement(itsRectTemp5)))
	    error1 = true;
	  ///////////////// fine prova verso l'alto /////////////////////////
	  ////////////// prova verso il basso ////////////////////////
	  itsTempPt21.x = itsTempPt1.x;
	  itsTempPt21.y = itsTempPt1.y;
	  itsTempPt22.y = itsEndPoint.y - DELTA;
	  itsTempPt22.x = itsStartPoint.x;
	  itsTempPt23.y = itsEndPoint.y - DELTA;
	  itsTempPt23.x = itsEndPoint.x - 15;
	  itsTempPt24.y = itsEndPoint.y - DELTA;
	  itsTempPt24.x = itsEndPoint.x - 15;
	  itsTempPt25.y = itsEndPoint.y - DELTA;
	  itsTempPt25.x = itsEndPoint.x;
	  itsTempPt26.x = itsTempPt6.x;
	  itsTempPt26.y = itsTempPt6.y;
	  
	  itsRectTemp21.y = itsTempPt21.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp21.x = itsTempPt21.x-1;
	  itsRectTemp21.height = itsTempPt22.y - itsTempPt21.y +2-/*8*/6;//segue correzione segment1
	  itsRectTemp21.width = 3;
	  itsRectTemp22.y = itsTempPt22.y-1;
	  itsRectTemp22.x = itsTempPt22.x-1;
	  itsRectTemp22.height = 3;
	  itsRectTemp22.width = itsTempPt23.x - itsTempPt22.x + 2;
	  itsRectTemp23.y = itsTempPt24.y -1;
	  itsRectTemp23.x = itsTempPt24.x -1;
	  itsRectTemp23.height = 3;
	  itsRectTemp23.width = 3;
	  itsRectTemp24.y = itsTempPt24.y -1;
	  itsRectTemp24.x = itsTempPt24.x -1;
	  itsRectTemp24.height = 3;
	  itsRectTemp24.width = itsTempPt25.x - itsTempPt24.x +2;
	  itsRectTemp25.y = itsTempPt25.y -1;
	  itsRectTemp25.x = itsTempPt25.x -1;
	  itsRectTemp25.height = itsTempPt26.y - itsTempPt25.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp25.width = 3;
	  
	  for(k2 = 0;(IntersectRgn(itsRectTemp22, itsStartPoint))&&(k2<100*step); k2+=step){// abbassa itsRect4 
	    itsRectTemp21.height+=step;
	    itsRectTemp22.y+=step;
	    itsRectTemp23.height+=step;
	  }
	  itsTempPt22.y = itsTempPt23.y = itsRectTemp22.y + 1;
	  spostamento2+=k2;
					
	  if((IntersectRgn(itsRectTemp21, itsStartPoint))||(IntersectRgn(itsRectTemp22, itsStartPoint))||
	     (IntersectRgn(itsRectTemp23, itsStartPoint))||(IntersectRgn(itsRectTemp24, itsStartPoint))||
	     (IntersectElement(itsRectTemp25)))
	    error2 = true;
	  ////////////// fine prova verso il basso
	}//end case 1.1
	else{ //  Case 1.2 sorg right dest
	  
	  //prova verso l'alto
	  itsTempPt1.y = itsStartPoint.y;//sorgente
	  itsTempPt1.x = itsStartPoint.x;
	  itsTempPt2.y = itsStartPoint.y +DELTA;
	  itsTempPt2.x = itsStartPoint.x;
	  itsTempPt3.y = itsStartPoint.y +DELTA;
	  itsTempPt3.x = itsStartPoint.x - 15;
	  itsTempPt4.y = itsStartPoint.y +DELTA;
	  itsTempPt4.x = itsStartPoint.x - 15;
	  itsTempPt5.x = itsEndPoint.x;
	  itsTempPt5.y = itsStartPoint.y + DELTA;
	  itsTempPt6.x = itsEndPoint.x;//destinazione
	  itsTempPt6.y = itsEndPoint.y;
	  
	  itsRectTemp1.y = itsTempPt1.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp1.x = itsTempPt1.x-1;
	  itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y +2-/*8*/6;
	  itsRectTemp1.width = 3;
	  itsRectTemp2.y = itsTempPt3.y-1;
	  itsRectTemp2.x = itsTempPt3.x-1;
	  itsRectTemp2.height = 3;
	  itsRectTemp2.width = itsTempPt2.x - itsTempPt3.x + 2;
	  itsRectTemp3.y = itsTempPt4.y -1;
	  itsRectTemp3.x = itsTempPt4.x -1;
	  itsRectTemp3.height = 3;
	  itsRectTemp3.width = 3;
	  itsRectTemp4.y = itsTempPt5.y -1;
	  itsRectTemp4.x = itsTempPt5.x -1;
	  itsRectTemp4.height = 3;
	  itsRectTemp4.width = itsTempPt4.x - itsTempPt5.x +2;
	  itsRectTemp5.y = itsTempPt5.y -1;
	  itsRectTemp5.x = itsTempPt5.x -1;
	  itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp5.width = 3;
	  
	  //limitation
	  for(k1 = 0;((IntersectRgn(itsRectTemp4, itsStartPoint))&&(itsRectTemp4.y>0)); k1+=step){
	    itsRectTemp3.y-=step;
	    itsRectTemp3.height+=step;
	    itsRectTemp4.y-=step;
	    itsRectTemp5.y-=step;
	    itsRectTemp5.height+=step;
	  }
	  itsTempPt4.y = itsTempPt5.y = itsRectTemp4.y+1;
	  spostamento1+=k1;
	  
	  if((IntersectRgn(itsRectTemp1, itsStartPoint))||(IntersectRgn(itsRectTemp2, itsStartPoint))||
	     (IntersectRgn(itsRectTemp3, itsStartPoint))||(IntersectRgn(itsRectTemp4, itsStartPoint))||
	     (IntersectElement(itsRectTemp5)))
	    error1 = true;
	  ///////////////// fine prova verso l'alto /////////////////////////
	  ////////////////  prova verso il basso ///////////////////////
	  
	  itsTempPt21.y = itsStartPoint.y;//sorgente
	  itsTempPt21.x = itsStartPoint.x;
	  itsTempPt22.y = itsEndPoint.y -DELTA;
	  itsTempPt22.x = itsStartPoint.x;
	  itsTempPt23.y = itsEndPoint.y -DELTA;
	  itsTempPt23.x = itsEndPoint.x +25;
	  itsTempPt24.y = itsEndPoint.y -DELTA;
	  itsTempPt24.x = itsEndPoint.x +25;
	  itsTempPt25.x = itsEndPoint.x;
	  itsTempPt25.y = itsEndPoint.y - DELTA;
	  itsTempPt26.x = itsEndPoint.x;//destinazione
	  itsTempPt26.y = itsEndPoint.y;
	  
	  itsRectTemp21.y = itsTempPt21.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp21.x = itsTempPt21.x-1;
	  itsRectTemp21.height = itsTempPt22.y - itsTempPt21.y +2-/*8*/6;
	  itsRectTemp21.width = 3;
	  itsRectTemp22.y = itsTempPt23.y-1;
	  itsRectTemp22.x = itsTempPt23.x-1;
	  itsRectTemp22.height = 3;
	  itsRectTemp22.width = itsTempPt22.x - itsTempPt23.x + 2;
	  itsRectTemp23.y = itsTempPt24.y -1;
	  itsRectTemp23.x = itsTempPt24.x -1;
	  itsRectTemp23.height = 3;
	  itsRectTemp23.width = 3;
	  itsRectTemp24.y = itsTempPt25.y -1;
	  itsRectTemp24.x = itsTempPt25.x -1;
	  itsRectTemp24.height = 3;
	  itsRectTemp24.width = itsTempPt24.x - itsTempPt25.x +2;
	  itsRectTemp25.y = itsTempPt25.y -1;
	  itsRectTemp25.x = itsTempPt25.x -1;
	  itsRectTemp25.height = itsTempPt26.y - itsTempPt25.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp25.width = 3;
	  
	  for(k2 = 0;(IntersectRgn(itsRectTemp22, itsStartPoint))&&(k2<100*step); k2+=step){
	    itsRectTemp21.height+=step;
	    itsRectTemp22.y+=step;
	    itsRectTemp23.height+=step;
	  }
	  itsTempPt23.y = itsTempPt22.y = itsRectTemp22.y+1;
	  spostamento2+=k2;
	  
	  if((IntersectRgn(itsRectTemp21, itsStartPoint))||(IntersectRgn(itsRectTemp22, itsStartPoint))||
	     (IntersectRgn(itsRectTemp23, itsStartPoint))||(IntersectRgn(itsRectTemp24, itsStartPoint))||
	     (IntersectElement(itsRectTemp25)))
	    error2 = true;
	  ///////////////// fine prova verso il basso /////////////////////////
	}//////////////////////////////////// End case 1.2
	
	////////////// determina se e' meglio verso il basso o verso l'alto//////////
	if(!(error1)){
	  if((error2)||(spostamento1 <= spostamento2)){
	    itsRect1 = itsRectTemp1;
	    itsRect2 = itsRectTemp2;
	    itsRect3 = itsRectTemp3;
	    itsRect4 = itsRectTemp4;
	    itsRect5 = itsRectTemp5;
	    aPoint1 = itsTempPt1;
	    aPoint2 = itsTempPt2;
	    aPoint3 = itsTempPt3;
	    aPoint4 = itsTempPt4;
	    aPoint5 = itsTempPt5;
	    aPoint6 = itsTempPt6;
	  }
	  else{
	    itsRect1 = itsRectTemp21;
	    itsRect2 = itsRectTemp22;
	    itsRect3 = itsRectTemp23;
	    itsRect4 = itsRectTemp24;
	    itsRect5 = itsRectTemp25;
	    aPoint1 = itsTempPt21;
	    aPoint2 = itsTempPt22;
	    aPoint3 = itsTempPt23;
	    aPoint4 = itsTempPt24;
	    aPoint5 = itsTempPt25;
	    aPoint6 = itsTempPt26;
	  }	
	}
	else
	  if(!error2){
	    itsRect1 = itsRectTemp21;
	    itsRect2 = itsRectTemp22;
	    itsRect3 = itsRectTemp23;
	    itsRect4 = itsRectTemp24;
	    itsRect5 = itsRectTemp25;
	    aPoint1 = itsTempPt21;
	    aPoint2 = itsTempPt22;
	    aPoint3 = itsTempPt23;
	    aPoint4 = itsTempPt24;
	    aPoint5 = itsTempPt25;
	    aPoint6 = itsTempPt26;
	  }
	  else//se sono sbagliati entrambi
	    noAutoRouting = true;
      }
      else{//end case 1.3//if the vertical distance between sorg and dest is inferior to 20 pixels   case 1.3///////////////
	
	//end case 1.3//if the vertical distance between sorg and dest is inferior to 20 pixels   case 1.3///////////////
	if((itsStartPoint.x < itsEndPoint.x)&&
	   (itsEndPoint.x - itsStartPoint.x > 40)){ //case 1.1 sorg left dest
	  itsTempPt1.x = itsStartPoint.x;//sorgente
	  itsTempPt1.y = itsStartPoint.y;
	  itsTempPt2.y = itsStartPoint.y +DELTA;
	  itsTempPt2.x = itsStartPoint.x;
	  itsTempPt3.y = itsStartPoint.y +DELTA;
	  itsTempPt3.x = itsStartPoint.x +25;//usate verso l'alto
	  itsTempPt4.x = itsStartPoint.x +25;
	  itsTempPt4.y = itsEndPoint.y -DELTA;
	  itsTempPt5.x =itsEndPoint.x;
	  itsTempPt5.y = itsEndPoint.y -DELTA;
	  itsTempPt6.x = itsEndPoint.x;//destinazione
	  itsTempPt6.y = itsEndPoint.y;
	  
	  itsTempPt21.x = itsTempPt1.x;
	  itsTempPt21.y = itsTempPt1.y;
	  itsTempPt22.x = itsTempPt2.x;
	  itsTempPt22.y = itsTempPt2.y;
	  itsTempPt23.y = itsStartPoint.y +DELTA;
	  itsTempPt23.x = itsEndPoint.x -15;
	  itsTempPt24.x = itsEndPoint.x -15;
	  itsTempPt24.y = itsEndPoint.y -DELTA;
	  itsTempPt25.x = itsTempPt5.x;
	  itsTempPt25.y = itsTempPt5.y;
	  itsTempPt26.x = itsTempPt6.x;
	  itsTempPt26.y = itsTempPt6.y;
	  
	  itsRectTemp1.y = itsTempPt1.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp1.x = itsTempPt1.x-1;
	  itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y+2-/*8*/6;
	  itsRectTemp1.width = 3;
	  itsRectTemp2.y = itsTempPt2.y-1;
	  itsRectTemp2.x = itsTempPt2.x-1;
	  itsRectTemp2.width = itsTempPt3.x-itsTempPt2.x +2;
	  itsRectTemp2.height = 3;
	  itsRectTemp3.y = itsTempPt4.y -1;
	  itsRectTemp3.x = itsTempPt4.x -1;
	  itsRectTemp3.height = itsTempPt3.y - itsTempPt4.y +2;
	  itsRectTemp3.width = 3;
	  itsRectTemp4.y = itsTempPt4.y -1;
	  itsRectTemp4.x = itsTempPt4.x -1;
	  itsRectTemp4.height = 3;
	  itsRectTemp4.width = itsTempPt5.x - itsTempPt4.x +2;
	  itsRectTemp5.y = itsTempPt5.y -1;
	  itsRectTemp5.x = itsTempPt5.x -1;
	  itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp5.width = 3;
	  
	  
	  itsRectTemp21.x = itsRectTemp1.x;
	  itsRectTemp21.y = itsRectTemp1.y;
	  itsRectTemp21.width = itsRectTemp1.width;
	  itsRectTemp21.height = itsRectTemp1.height;
	  itsRectTemp22.y = itsTempPt22.y -1;
	  itsRectTemp22.x = itsTempPt22.x -1;
	  itsRectTemp22.height = 3;
	  itsRectTemp22.width = itsTempPt23.x - itsTempPt22.x +2;
	  itsRectTemp23.y = itsTempPt24.y -1;
	  itsRectTemp23.x = itsTempPt24.x -1;
	  itsRectTemp23.height = itsTempPt23.y - itsTempPt24.y +2;
	  itsRectTemp23.width = 3;
	  itsRectTemp24.y = itsTempPt24.y -1;
	  itsRectTemp24.x = itsTempPt24.x -1;
	  itsRectTemp24.width = itsTempPt25.x - itsTempPt24.x+2;
	  itsRectTemp24.height = 3;
	  itsRectTemp25.y = itsTempPt25.y -1;
	  itsRectTemp25.x = itsTempPt25.x -1;
	  itsRectTemp25.height = itsTempPt26.y - itsTempPt25.y +2-/*8*/6;
	  itsRectTemp25.width = 3;
	  
	  //////  verso l'alto  ////////////////
	  
	  //limitation
	  for(k1 = 0;((IntersectRgn(itsRectTemp4, itsStartPoint))&&(itsRectTemp4.y>0)); k1+=step){
	    itsRectTemp3.y-=step;
	    itsRectTemp3.height+=step;
	    itsRectTemp4.y-=step;
	    itsRectTemp5.y-=step;
	    itsRectTemp5.height+=step;
	  }
	  itsTempPt4.y = itsTempPt5.y = itsRectTemp4.y + 1;
	  spostamento1+=k1;
	  
	  if((IntersectRgn(itsRectTemp1, itsStartPoint))||(IntersectRgn(itsRectTemp2, itsStartPoint))||
	     (IntersectRgn(itsRectTemp3, itsStartPoint))||(IntersectRgn(itsRectTemp4, itsStartPoint))||
	     (IntersectElement(itsRectTemp5)))
	    error1 = true;
	  /////////  fine verso l'alto ////////
	  ////////  verso il basso //////////
	  
	  for(k2 = 0;(IntersectRgn(itsRectTemp22, itsStartPoint))&&(k2<100*step); k2+=step){
	    itsRectTemp21.height+=step;
	    itsRectTemp22.y+=step;
	    itsRectTemp23.height+=step;
	  }
	  itsTempPt22.y = itsTempPt23.y = itsRectTemp22.y+1;
	  spostamento2+=k2;
	  
	  if((IntersectRgn(itsRectTemp21, itsStartPoint))||(IntersectRgn(itsRectTemp22, itsStartPoint))||
	     (IntersectRgn(itsRectTemp23, itsStartPoint))||(IntersectRgn(itsRectTemp24, itsStartPoint))||
	     (IntersectElement(itsRectTemp25)))
	    error2 = true;
	  ////////////// fine prova verso il basso
	  
	}//end case 1.3.1
	else if((itsStartPoint.x >= itsEndPoint.x)&&
		(itsStartPoint.x - itsEndPoint.x > 40)){//if sorg.x > dest.x case 1.3.2
	  
	  itsTempPt1.x = itsStartPoint.x;//sorgente
	  itsTempPt1.y = itsStartPoint.y;
	  itsTempPt2.y = itsStartPoint.y +DELTA;
	  itsTempPt2.x = itsStartPoint.x;
	  itsTempPt3.y = itsStartPoint.y +DELTA;
	  itsTempPt3.x = itsStartPoint.x - 15;
	  itsTempPt4.x = itsStartPoint.x - 15;
	  itsTempPt4.y = itsEndPoint.y -DELTA;
	  itsTempPt5.x =itsEndPoint.x;
	  itsTempPt5.y = itsEndPoint.y -DELTA;
	  itsTempPt6.x = itsEndPoint.x;//destinazione
	  itsTempPt6.y = itsEndPoint.y;
	  
	  itsTempPt21.x = itsTempPt1.x;
	  itsTempPt21.y = itsTempPt1.y;
	  itsTempPt22.x = itsTempPt2.x;
	  itsTempPt22.y = itsTempPt2.y;
	  itsTempPt23.y = itsStartPoint.y +DELTA;
	  itsTempPt23.x = itsEndPoint.x + 25;
	  itsTempPt24.x = itsEndPoint.x + 25;
	  itsTempPt24.y = itsEndPoint.y - DELTA;
	  itsTempPt25.x = itsEndPoint.x;
	  itsTempPt25.y = itsEndPoint.y - DELTA;
	  itsTempPt26.x = itsTempPt6.x;
	  itsTempPt26.y = itsTempPt6.y;
	  
	  
	  itsRectTemp1.y = itsTempPt1.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp1.x = itsTempPt1.x-1;
	  itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y +2-/*8*/6;
	  itsRectTemp1.width = 3;
	  itsRectTemp2.y = itsTempPt3.y-1;
	  itsRectTemp2.x = itsTempPt3.x-1;
	  itsRectTemp2.width = itsTempPt2.x - itsTempPt3.x + 2;
	  itsRectTemp2.height = 3;
	  itsRectTemp3.y = itsTempPt4.y -1;
	  itsRectTemp3.x = itsTempPt4.x -1;
	  itsRectTemp3.height = itsTempPt3.y - itsTempPt4.y +2;
	  itsRectTemp3.width = 3;
	  itsRectTemp4.y = itsTempPt5.y -1;
	  itsRectTemp4.x = itsTempPt5.x -1;
	  itsRectTemp4.height = 3;
	  itsRectTemp4.width = itsTempPt4.x - itsTempPt5.x +2;
	  itsRectTemp5.y = itsTempPt5.y -1;
	  itsRectTemp5.x = itsTempPt5.x -1;
	  itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment5
	  itsRectTemp5.width = 3;				
	  
	  itsRectTemp21.x = itsRectTemp1.x;
	  itsRectTemp21.y = itsRectTemp1.y;
	  itsRectTemp21.width = itsRectTemp1.width;
	  itsRectTemp21.height = itsRectTemp1.height;
	  itsRectTemp22.y = itsTempPt23.y -1;
	  itsRectTemp22.x = itsTempPt23.x -1;
	  itsRectTemp22.height = 3;
	  itsRectTemp22.width = itsTempPt22.x -itsTempPt23.x +2;
	  itsRectTemp23.y = itsTempPt24.y -1;
	  itsRectTemp23.x = itsTempPt24.x -1;
	  itsRectTemp23.height = itsTempPt23.y - itsTempPt24.y +2;
	  itsRectTemp23.width = 3;
	  itsRectTemp24.y = itsTempPt25.y -1;
	  itsRectTemp24.x = itsTempPt25.x -1;
	  itsRectTemp24.width = itsTempPt24.x - itsTempPt25.x +2;
	  itsRectTemp24.height = 3;
	  itsRectTemp25.y = itsTempPt25.y -1;
	  itsRectTemp25.x = itsTempPt25.x -1;
	  itsRectTemp25.height = itsTempPt26.y - itsTempPt25.y +2-/*8*/6;
	  itsRectTemp25.width = 3;
	  
	  //////  verso l'alto  ////////////////
	  //limitation
	  for(k1 = 0;((IntersectRgn(itsRectTemp4, itsStartPoint))&&(itsRectTemp4.y>0)); k1+=step){
	    itsRectTemp3.y-=step;
	    itsRectTemp3.height+=step;
	    itsRectTemp4.y-=step;
	    itsRectTemp5.y-=step;
	    itsRectTemp5.height+=step;
	  }
	  itsTempPt4.y = itsTempPt5.y = itsRectTemp4.y +1;
	  spostamento1+=k1;
	  
	  if((IntersectRgn(itsRectTemp1, itsStartPoint))||(IntersectRgn(itsRectTemp2, itsStartPoint))||
	     (IntersectRgn(itsRectTemp3, itsStartPoint))||(IntersectRgn(itsRectTemp4, itsStartPoint))||
	     (IntersectElement(itsRectTemp5)))
	    error1 = true;
	  ///////////////// fine verso l'alto /////////////////////////
	  ////////  verso il basso //////////
	  
	  for(k2 = 0;(IntersectRgn(itsRectTemp22, itsStartPoint))&&(k2<100*step); k2+=step){
	    itsRectTemp21.height+=step;
	    itsRectTemp22.y+=step;
	    itsRectTemp23.height+=step;
	  }
	  itsTempPt22.y = itsTempPt23.y = itsRectTemp22.y +1;
	  spostamento2+=k2;
	  
	  if((IntersectRgn(itsRectTemp21, itsStartPoint))||(IntersectRgn(itsRectTemp22, itsStartPoint))||
	     (IntersectRgn(itsRectTemp23, itsStartPoint))||(IntersectRgn(itsRectTemp24, itsStartPoint))||
	     (IntersectElement(itsRectTemp25)))
	    error2 = true;
	  ////////////// fine prova verso il basso
	  
	}//end case 1.3.2
	else{
	  error1 = error2 = true;
	}
	
	if(!(error1)){
	  if((error2)||(spostamento1 <= spostamento2)){
	      itsRect1 = itsRectTemp1;
	      itsRect2 = itsRectTemp2;
	      itsRect3 = itsRectTemp3;
	      itsRect4 = itsRectTemp4;
	      itsRect5 = itsRectTemp5;
	      aPoint1 = itsTempPt1;
	      aPoint2 = itsTempPt2;
	      aPoint3 = itsTempPt3;
	      aPoint4 = itsTempPt4;
	      aPoint5 = itsTempPt5;
	      aPoint6 = itsTempPt6;
	  }
	  else{
	    itsRect1 = itsRectTemp21;
	    itsRect2 = itsRectTemp22;
	    itsRect3 = itsRectTemp23;
	    itsRect4 = itsRectTemp24;
	    itsRect5 = itsRectTemp25;
	    aPoint1 = itsTempPt21;
	    aPoint2 = itsTempPt22;
	    aPoint3 = itsTempPt23;
	    aPoint4 = itsTempPt24;
	    aPoint5 = itsTempPt25;
	    aPoint6 = itsTempPt26;
	  }	
	}
	else
	  if(!error2){
	    itsRect1 = itsRectTemp21;
	    itsRect2 = itsRectTemp22;
	    itsRect3 = itsRectTemp23;
	    itsRect4 = itsRectTemp24;
	    itsRect5 = itsRectTemp25;
	    aPoint1 = itsTempPt21;
	    aPoint2 = itsTempPt22;
	    aPoint3 = itsTempPt23;
	    aPoint4 = itsTempPt24;
	    aPoint5 = itsTempPt25;
	    aPoint6 = itsTempPt26;
	  }
	  else
	    noAutoRouting = true;				
      }
      if(!noAutoRouting){
	itsRect1.y-= /*8*/6;//correzione segment1
	itsRect1.height+=/*8*/6;
	itsRect5.height+=/*8*/6;//correzione segment5
	
	
	aSegment1 = new ErmesConnSegment(aPoint1, aPoint2, this);
	aSegment2 = new ErmesConnSegment(aPoint2, aPoint3, this);
	aSegment3 = new ErmesConnSegment(aPoint3, aPoint4, this);
	aSegment4 = new ErmesConnSegment(aPoint4, aPoint5, this);
	aSegment5 = new ErmesConnSegment(aPoint5, aPoint6, this);
	
	itsPoints.addElement(aPoint1);
	itsPoints.addElement(aPoint2);
	itsPoints.addElement(aPoint3);
	itsPoints.addElement(aPoint4);
	itsPoints.addElement(aPoint5);
	itsPoints.addElement(aPoint6);
	
	itsVSegmentList.addElement(aSegment1);
	itsHSegmentList.addElement(aSegment2);
	itsVSegmentList.addElement(aSegment3);
	itsHSegmentList.addElement(aSegment4);
	itsVSegmentList.addElement(aSegment5);
	
	return true;
      }
      else
	return false;
    }//end case 1
    else{//(itsStartPoint.y > itsEndPoint.y) // case 2 Sorg lower than dest
      if (itsStartPoint.x > itsEndPoint.x){ // case 2.1 sorg right of dest
	itsTempPt1.x = itsStartPoint.x;//sorgente
	itsTempPt1.y = itsStartPoint.y;
	itsTempPt2.x = itsStartPoint.x;
	itsTempPt2.y = itsStartPoint.y +DELTA;
	itsTempPt3.x = itsStartPoint.x - 15;
	itsTempPt3.y = itsStartPoint.y +DELTA;
	itsTempPt4.x = itsStartPoint.x - 15;
	itsTempPt4.y = itsEndPoint.y - DELTA;
	itsTempPt5.x = itsEndPoint.x;
	itsTempPt5.y = itsEndPoint.y - DELTA;
	itsTempPt6.x = itsEndPoint.x;//destinazione
	itsTempPt6.y = itsEndPoint.y;
	
	itsRectTemp1.y = itsTempPt1.y + /*8*/6-1;//segue correzione segment1
	itsRectTemp1.x = itsTempPt1.x-1;
	itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y +2-/*8*/6;
	itsRectTemp1.width = 3;
	itsRectTemp2.y = itsTempPt3.y-1;
	itsRectTemp2.x = itsTempPt3.x-1;
	itsRectTemp2.height = 3;
	itsRectTemp2.width = itsTempPt2.x - itsTempPt3.x +2;
	itsRectTemp3.y = itsTempPt4.y-1;
	itsRectTemp3.x =  itsTempPt4.x-1;
	itsRectTemp3.height =  itsTempPt3.y - itsTempPt4.y +2;
	itsRectTemp3.width =  3;
	itsRectTemp4.y = itsTempPt5.y-1;
	itsRectTemp4.x =  itsTempPt5.x-1;
	itsRectTemp4.height =  3;
	itsRectTemp4.width =  itsTempPt4.x - itsTempPt5.x +2;
	itsRectTemp5.y = itsTempPt5.y-1;
	itsRectTemp5.x = itsTempPt5.x-1;
	itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment3
	itsRectTemp5.width = 3;
	
	itsRectTemp21.setBounds(itsRectTemp1.x, itsRectTemp1.y, itsRectTemp1.width, itsRectTemp1.height);
	itsRectTemp22.setBounds(itsRectTemp2.x, itsRectTemp2.y, itsRectTemp2.width, itsRectTemp2.height);
	itsRectTemp23.setBounds(itsRectTemp3.x, itsRectTemp3.y, itsRectTemp3.width, itsRectTemp3.height);
	itsRectTemp24.setBounds(itsRectTemp4.x, itsRectTemp4.y, itsRectTemp4.width, itsRectTemp4.height);
	itsRectTemp25.setBounds(itsRectTemp5.x, itsRectTemp5.y, itsRectTemp5.width, itsRectTemp5.height);
	
	itsTempPt21.x = itsTempPt1.x;
	itsTempPt21.y = itsTempPt1.y;
	itsTempPt22.x = itsTempPt2.x;
	itsTempPt22.y = itsTempPt2.y;
	itsTempPt23.x = itsTempPt3.x;
	itsTempPt23.y = itsTempPt3.y;
	itsTempPt24.x = itsTempPt4.x;
	itsTempPt24.y = itsTempPt4.y;
	itsTempPt25.x = itsTempPt5.x;
	itsTempPt25.y = itsTempPt5.y;
	itsTempPt26.x = itsTempPt6.x;
	itsTempPt26.y = itsTempPt6.y;
	
	//da togliere sicuramente!!!!!
	
	for(k6 = 0; (IntersectRgn(itsRectTemp2, itsStartPoint))&&(k6<100*step); k6+=step)
	  itsRectTemp2.y+=step;
	
	itsRectTemp3.height+=k6;
	itsRectTemp1.height+=k6;
	itsTempPt2.y+=k6;
	itsTempPt3.y+=k6;
	///////fin qui!!!!!!!
	
	/// verso sinistra
	k3 = 0;
	for(k1 = 0;(((IntersectRgn(itsRectTemp3, itsStartPoint))||
		     (IntersectRgn(itsRectTemp4, itsStartPoint)))&&(itsRectTemp3.x>0)); k1+=step){
	  itsRectTemp3.x-=step;
	  if(!inverted){
	    if(itsRectTemp4.width > step)
	      itsRectTemp4.width-=step;
	    else{
	      temp = itsRectTemp4.width - step;
	      itsRectTemp4.width = -temp;
	      itsRectTemp4.x += temp;
	      inverted = true;
	    }
	  }
	  else {
	    itsRectTemp4.x-=step;
	    itsRectTemp4.width+=step;
	  }
	  itsRectTemp2.x-= step;
	  itsRectTemp2.width+=step;	
	  for(; (IntersectRgn(itsRectTemp2, itsStartPoint)); k3+=step){
	    itsRectTemp2.y+=step;
	    itsRectTemp3.height+=step;
	    itsRectTemp1.height+=step;	
	  }
	}
	itsTempPt3.x-=k1;
	itsTempPt4.x-=k1;
	itsTempPt3.y+=k3;
	itsTempPt2.y+=k3;
	if(IntersectRgn(itsRectTemp1, itsStartPoint)||IntersectRgn(itsRectTemp2, itsStartPoint)||
	   IntersectRgn(itsRectTemp3, itsStartPoint)||IntersectRgn(itsRectTemp4, itsStartPoint)||
	   IntersectElement(itsRectTemp5))
	  error1 = true;
	
	////// verso destra
	
	//da togliere forse!!!!!!!!!!!!!
	for(k7 = 0;((IntersectRgn(itsRectTemp24, itsStartPoint))&&(itsRectTemp24.y>0)); k7+=step)
	  itsRectTemp24.y-=step;
	
	itsRectTemp23.y-=k7;
	itsRectTemp23.height+=k7;
	itsRectTemp25.y-=k7;
	itsRectTemp25.height+=k7;
	itsTempPt25.y-=k7;
	itsTempPt24.y-=k7;
	////fin qui////////
	
	k4 = 0;
	inverted = false;
	
	for(k2 = 0;((IntersectRgn(itsRectTemp23, itsStartPoint))&&(itsRectTemp24.y>0))&&(k2<100*step); k2+=step){
	  itsRectTemp23.x+=step;
	  if(!inverted){
	    if(itsRectTemp22.width > step){
	      itsRectTemp22.x+=step;
	      itsRectTemp22.width-=step;
	    }
	    else{
	      itsRectTemp22.x += step;
	      itsRectTemp22.width = - (itsRectTemp22.width - step);
	      inverted = true;
	    }
	  }
	  else itsRectTemp22.width+=step;
	  
	  itsRectTemp24.width+=step;
	  for(; (IntersectRgn(itsRectTemp24, itsStartPoint)); k4+=step){
	      itsRectTemp24.y-=step;
	      itsRectTemp23.y-=step;
	      itsRectTemp23.height+=step;
	      itsRectTemp25.y-=step;
	      itsRectTemp25.height+=step;
	  }
	}
	//itsTempPt24.x+=k1;
	//itsTempPt23.x+=k1;
	itsTempPt24.x+=k2;
	itsTempPt23.x+=k2;
	itsTempPt24.y-=k4;
	itsTempPt25.y-=k4;
	
	if(IntersectRgn(itsRectTemp21, itsStartPoint)||IntersectRgn(itsRectTemp22, itsStartPoint)||
	   IntersectRgn(itsRectTemp23, itsStartPoint)||IntersectRgn(itsRectTemp24, itsStartPoint)||
	   IntersectElement(itsRectTemp25))
	  error2 = true;
	
	spostamento1 = ((itsTempPt1.x - itsRectTemp3.x +1) + java.lang.Math.abs(itsRectTemp3.x - itsTempPt6.x +1) + k3 + k6);
	spostamento2 = ((itsRectTemp23.x - itsTempPt26.x +1) + java.lang.Math.abs(itsTempPt21.x - itsRectTemp23.x +1) + k4 + k7);
	
      }		////////////////// end case 2.1
      else{////////////////////////////case 2.2 dest right of sorg
	
	if(itsEndPoint.x - itsStartPoint.x>=30){//introdotto per eliminare il "nodo"
	  
	  itsTempPt1.x = itsStartPoint.x;//sorgente
	  itsTempPt1.y = itsStartPoint.y;
	  itsTempPt2.x = itsStartPoint.x;
	  itsTempPt2.y = itsStartPoint.y +DELTA;
	  itsTempPt3.x = itsStartPoint.x + 25;
	  itsTempPt3.y = itsStartPoint.y +DELTA;
	  itsTempPt4.x = itsStartPoint.x + 25;
	  itsTempPt4.y = itsEndPoint.y - DELTA;
	  itsTempPt5.x = itsEndPoint.x;
	  itsTempPt5.y = itsEndPoint.y - DELTA;
	  itsTempPt6.x = itsEndPoint.x;//destinazione
	  itsTempPt6.y = itsEndPoint.y;
	  
	  itsRectTemp1.y = itsTempPt1.y + /*8*/6-1;//segue correzione segment1
	  itsRectTemp1.x = itsTempPt1.x-1;
	  itsRectTemp1.height = itsTempPt2.y - itsTempPt1.y +2-/*8*/6;
	  itsRectTemp1.width = 3;
	  itsRectTemp2.y = itsTempPt2.y-1;
	  itsRectTemp2.x = itsTempPt2.x-1;
	  itsRectTemp2.height = 3;
	  itsRectTemp2.width = itsTempPt3.x - itsTempPt2.x +2;
	  itsRectTemp3.y = itsTempPt4.y-1;
	  itsRectTemp3.x =  itsTempPt4.x-1;
	  itsRectTemp3.height =  itsTempPt3.y - itsTempPt4.y +2;
	  itsRectTemp3.width =  3;
	  itsRectTemp4.y = itsTempPt4.y-1;
	  itsRectTemp4.x =  itsTempPt4.x-1;
	  itsRectTemp4.height =  3;
	  itsRectTemp4.width =  itsTempPt5.x - itsTempPt4.x +2;
	  itsRectTemp5.y = itsTempPt5.y-1;
	  itsRectTemp5.x = itsTempPt5.x-1;
	  itsRectTemp5.height = itsTempPt6.y - itsTempPt5.y +2-/*8*/6;//segue correzione segment3
	  itsRectTemp5.width = 3;
	  
	  
	  itsRectTemp21.setBounds(itsRectTemp1.x, itsRectTemp1.y, itsRectTemp1.width, itsRectTemp1.height);
	  itsRectTemp22.setBounds(itsRectTemp2.x, itsRectTemp2.y, itsRectTemp2.width, itsRectTemp2.height);
	  itsRectTemp23.setBounds(itsRectTemp3.x, itsRectTemp3.y, itsRectTemp3.width, itsRectTemp3.height);
	  itsRectTemp24.setBounds(itsRectTemp4.x, itsRectTemp4.y, itsRectTemp4.width, itsRectTemp4.height);
	  itsRectTemp25.setBounds(itsRectTemp5.x, itsRectTemp5.y, itsRectTemp5.width, itsRectTemp5.height);
	  
	  itsTempPt21.x = itsTempPt1.x;
	  itsTempPt21.y = itsTempPt1.y;
	  itsTempPt22.x = itsTempPt2.x;
	  itsTempPt22.y = itsTempPt2.y;
	  itsTempPt23.x = itsTempPt3.x;
	  itsTempPt23.y = itsTempPt3.y;
	  itsTempPt24.x = itsTempPt4.x;
	  itsTempPt24.y = itsTempPt4.y;
	  itsTempPt25.x = itsTempPt5.x;
	  itsTempPt25.y = itsTempPt5.y;
	  itsTempPt26.x = itsTempPt6.x;
	  itsTempPt26.y = itsTempPt6.y;
	  
	  // verso destra
	  
	  //da levare probabilmente!!!!!!!!
	  for(k7 = 0; (IntersectRgn(itsRectTemp2, itsStartPoint))&&(k7<100*step); k7+=step)
	    itsRectTemp2.y+=step;
	  
	  itsRectTemp3.height+=k7;
	  itsRectTemp1.height+=k7;	
	  itsTempPt2.y+=k7;
	  itsTempPt3.y+=k7;
	  ////fino a qui !!!!!!!!!!!!!!!
	  
	  k3 = 0;	
	  
	  for(k1 = 0;((IntersectRgn(itsRectTemp3, itsStartPoint)||
		       IntersectRgn(itsRectTemp4, itsStartPoint))&&(itsRectTemp4.y>0)&&(k1<100*step)); k1+=step){
	    itsRectTemp3.x+=step;
	    if(!inverted){
	      if(itsRectTemp4.width > step){
		itsRectTemp4.x+=step;
		itsRectTemp4.width-=step;
	      }
	      else{
		temp = step - itsRectTemp4.width;
		itsRectTemp4.x += itsRectTemp4.width;
		itsRectTemp4.width = temp;
		inverted = true;
	      }
	    }
	    else itsRectTemp4.width+=step;
	    
	    itsRectTemp2.width+=step;
	    
	    for(; (IntersectRgn(itsRectTemp2, itsStartPoint)); k3+=step){
	      itsRectTemp2.y+=step;
	      itsRectTemp3.height+=step;
	      itsRectTemp1.height+=step;
	    }		
	  }
	  itsTempPt3.x+=k1;
	  itsTempPt4.x+=k1;
	  itsTempPt3.y+=k3;
	  itsTempPt2.y+=k3;
	  
	  if((IntersectRgn(itsRectTemp1, itsStartPoint))||(IntersectRgn(itsRectTemp2, itsStartPoint))||
	     (IntersectRgn(itsRectTemp3, itsStartPoint))||(IntersectRgn(itsRectTemp4, itsStartPoint))||
	     (IntersectElement(itsRectTemp5)))
	    error1 = true;
	  
	  // verso sinistra
	  
	  //forse eliminare!!!!!!!!
	  for(k6 = 0; ((IntersectRgn(itsRectTemp24, itsStartPoint))&&(itsRectTemp24.y>0)); k6+=step)
	    itsRectTemp24.y-=step;
	  
	  itsRectTemp23.y-=k6;
	  itsRectTemp23.height+=k6;
	  itsRectTemp25.y-=k6;
	  itsRectTemp25.height+=k6;
	  itsTempPt24.y-=k6;
	  itsTempPt25.y-=k6;
	  //fin qui!!!!!!!!
	  
	  k4 = 0;
	  inverted = false;
	  
	  for(k2 = 0;((IntersectRgn(itsRectTemp23, itsStartPoint))&&(itsRectTemp23.x>0)&&(itsRectTemp24.y>0)); k2+=step){
	    itsRectTemp23.x-=step;
	    if(!inverted){
	      if(itsRectTemp22.width > step)
		itsRectTemp22.width-=step;
	      else{
		temp = itsRectTemp22.width - step;
		itsRectTemp22.x += temp;
		itsRectTemp22.width = - temp;
		inverted = true;
	      }
	    }
	    else{
	      itsRectTemp22.x-=step;
	      itsRectTemp22.width+=step;
	    }
	    
	    itsRectTemp24.x-=step;
	    itsRectTemp24.width+=step;
	    for(; (IntersectRgn(itsRectTemp24, itsStartPoint)); k4+=step){
	      itsRectTemp24.y-=step;
	      itsRectTemp23.y-=step;
	      itsRectTemp23.height-=step;
	      itsRectTemp25.y-=step;
	      itsRectTemp25.height-=step;
	    }  
	  }
	  itsTempPt24.x-=k2;
	  itsTempPt23.x-=k2;
	  itsTempPt24.y-=k4;
	  itsTempPt25.y-=k4;
	  
	  if((IntersectRgn(itsRectTemp21, itsStartPoint))||(IntersectRgn(itsRectTemp22, itsStartPoint))||
	     (IntersectRgn(itsRectTemp23, itsStartPoint))||(IntersectRgn(itsRectTemp24, itsStartPoint))||
	     (IntersectElement(itsRectTemp25)))
	    error2 = true;
	  
	  spostamento1 = ((itsRectTemp3.x +1 - itsTempPt1.x) + java.lang.Math.abs(itsTempPt6.x - itsRectTemp3.x +1) + k3 + k7);
	  spostamento2 = ((itsTempPt26.x - itsRectTemp23.x + 1) + java.lang.Math.abs(itsTempPt21.x - itsRectTemp23.x -1) + k4 + k6);
	}
	else{//introdotto per eliminare il "nodo"!!!!
	  error1 = error2 = true;				
	}
      }////////////////////// End case 2.2
      
      if(!error1){
	if((spostamento1 <= spostamento2)||error2){
	  itsRect1 = itsRectTemp1;
	  itsRect2 = itsRectTemp2;
	  itsRect3 = itsRectTemp3;
	  itsRect4 = itsRectTemp4;
	  itsRect5 = itsRectTemp5;
	  aPoint1 = itsTempPt1;
	  aPoint2 = itsTempPt2;
	  aPoint3 = itsTempPt3;
	  aPoint4 = itsTempPt4;
	  aPoint5 = itsTempPt5;
	  aPoint6 = itsTempPt6;
	}
	else{
	  itsRect1 = itsRectTemp21;
	  itsRect2 = itsRectTemp22;
	  itsRect3 = itsRectTemp23;
	  itsRect4 = itsRectTemp24;
	  itsRect5 = itsRectTemp25;
	  aPoint1 = itsTempPt21;
	  aPoint2 = itsTempPt22;
	  aPoint3 = itsTempPt23;
	  aPoint4 = itsTempPt24;	
	  aPoint5 = itsTempPt25;
	  aPoint6 = itsTempPt26;
	}
      }
      else
	if(!error2){
	  itsRect1 = itsRectTemp21;
	  itsRect2 = itsRectTemp22;
	  itsRect3 = itsRectTemp23;
	  itsRect4 = itsRectTemp24;
	  itsRect5 = itsRectTemp25;
	  aPoint1 = itsTempPt21;
	  aPoint2 = itsTempPt22;
	  aPoint3 = itsTempPt23;
	  aPoint4 = itsTempPt24;	
	  aPoint5 = itsTempPt25;
	  aPoint6 = itsTempPt26;
	}
	else
	  noAutoRouting = true;
      
      itsRect1.y-=/*8*/6;//correzione segment1
      itsRect1.height+=/*8*/6;
      itsRect5.height+=/*8*/6;//correzione segment5
			
      if(!noAutoRouting){
	
	aSegment1 = new ErmesConnSegment(aPoint1, aPoint2, this);
	aSegment2 = new ErmesConnSegment(aPoint2, aPoint3, this);
	aSegment3 = new ErmesConnSegment(aPoint3, aPoint4, this);
	aSegment4 = new ErmesConnSegment(aPoint4, aPoint5, this);
	aSegment5 = new ErmesConnSegment(aPoint5, aPoint6, this);
	
	itsPoints.addElement(aPoint1);
	itsPoints.addElement(aPoint2);
	itsPoints.addElement(aPoint3);
	itsPoints.addElement(aPoint4);
	itsPoints.addElement(aPoint5);
	itsPoints.addElement(aPoint6);
	
	itsVSegmentList.addElement(aSegment1);
	itsHSegmentList.addElement(aSegment2);
	itsVSegmentList.addElement(aSegment3);
	itsHSegmentList.addElement(aSegment4);
	itsVSegmentList.addElement(aSegment5);
	
	return true;
      } 
      else return false;
    }//end case 2
}


  //--------------------------------------------------------
  //	IntersectRgn
  //	say if theRect intersect some region
  //	(elemes and segments) in the sketchpad
  //--------------------------------------------------------
  public boolean IntersectRgn(Rectangle theRect, Point theSource){
    if(itsSketchPad.GetElementRegion().RectInRgn(theRect))
      return true;
    else 
      if(theRect.height == 3)//Horizontal line
	if((itsSketchPad.GetHSegmRgn().RectInRgn(theRect))&&
	   (DifferentSource(theRect, theSource)))
	  return true;//different source, overlapping not allowed
	else 
	  return false;//same source, overlapping is allowed
      else//vertical line
	if((itsSketchPad.GetVSegmRgn().RectInRgn(theRect))&&
	   (DifferentSource(theRect, theSource)))
	  return true;//different source, overlapping not allowed
	else 
	  return false;//same source, overlapping is allowed
  }
	
  //--------------------------------------------------------
  //	IntersectElement
  //	say if theRect intersect some element region in teh sketchpad
  //--------------------------------------------------------
  public boolean IntersectElement(Rectangle theRect){
    if(itsSketchPad.GetElementRegion().RectInRgn(theRect))
      return true;
    else return false;
  }
	
  //--------------------------------------------------------
  //	DifferentSource
  //  say if theRect intersect some region
  //	of connection with different source 
  //--------------------------------------------------------
  public boolean DifferentSource(Rectangle theRect, Point theSource){
    boolean horizontal = false;
    ErmesConnection aConnection;
    ErmesConnSegment aSegment;
    ErmesRegion aRegion = new ErmesRegion();
    Rectangle aRect = new Rectangle();
    
    if(theRect.height == 3)//horizontal segment
      horizontal = true;
    for(int k=0; k <itsOutlet.itsConnections.size();k++){
      aConnection = (ErmesConnection )(itsOutlet.itsConnections.elementAt(k));
      if(horizontal)
	for(int i=0; i<aConnection.itsHSegmentList.size(); i++){
	  aSegment = (ErmesConnSegment) aConnection.itsHSegmentList.elementAt(i);
	  aRect = aSegment.Bounds();
	  aRegion.Add(aRect);
	}
      else
	for(int i=0; i<(aConnection.itsVSegmentList.size()); i++){
	  aSegment = (ErmesConnSegment) aConnection.itsVSegmentList.elementAt(i);
	  aRect = aSegment.Bounds();
	  aRegion.Add(aRect);
	}
    }
    if(aRegion.RectInRgn(theRect))
      return false;
    else 
      return true;
  }

	
  //--------------------------------------------------------
  //	PrepareToRouting
  //  prepare the connection to a new autorouting
  //--------------------------------------------------------
  //prepara la connessione per un nuovo autorouting a seguito dello spostamento di sorg o dest
  public void PrepareToRouting(){
    
    // destruction of the points allocated in the Autorouting function
    itsPoints.removeAllElements();
    // destruction of the segments allocated in the Autorouting function
    itsHSegmentList.removeAllElements();
    //forse non li cancella ->sarebbe da fare un repaint subito?????
    itsVSegmentList.removeAllElements();
    
    itsErrorState = false;
    
    itsStartPoint.x = itsOutlet.GetAnchorPoint().x;
    itsStartPoint.y = itsOutlet.GetAnchorPoint().y;
    itsEndPoint.x = itsInlet.GetAnchorPoint().x;
    itsEndPoint.y = itsInlet.GetAnchorPoint().y;
  }
	
  //--------------------------------------------------------
  //	IsToMoving
  //  say if the connection is to moving only or to re-routing
  //--------------------------------------------------------
  public boolean IsToMoving(int theDeltaH, int theDeltaV){
    
    ErmesConnSegment aSegment;
    Rectangle aRect;
    Point aPoint;
    int i;
    aPoint = itsOutlet.GetAnchorPoint();
    for(i = 0; i<itsHSegmentList.size(); i++){
      aSegment = (ErmesConnSegment)(itsHSegmentList.elementAt(i));
      aRect = aSegment.Bounds();
      aRect.x+=theDeltaH;
      aRect.y+=theDeltaV;
      if(IntersectRgn(aRect, aPoint))
	return false;
    }
    aSegment = (ErmesConnSegment)(itsVSegmentList.elementAt(0));
    aRect = aSegment.Bounds();
    aRect.x+=theDeltaH;
    aRect.y+=(theDeltaV + 7);
    if(IntersectRgn(aRect, aPoint))
      return false;
    
    for(i = 1; i<(itsVSegmentList.size())-1; i++){
      aSegment = (ErmesConnSegment)(itsVSegmentList.elementAt(i));
      aRect = aSegment.Bounds();
      aRect.x+=theDeltaH;
      aRect.y+=theDeltaV;
      if(IntersectRgn(aRect, aPoint))
	return false;
    }
    return true;
  }
	
	
  //--------------------------------------------------------
  //	MoveConnection
  //  Move a connection 
  //--------------------------------------------------------
  public void MoveConnection(int theDeltaH, int theDeltaV){
    ErmesConnSegment aSegment;
    int i;
    
    itsStartPoint.x = itsOutlet.GetAnchorPoint().x;
    itsStartPoint.y = itsOutlet.GetAnchorPoint().y;
    itsEndPoint.x = itsInlet.GetAnchorPoint().x;
    itsEndPoint.y = itsInlet.GetAnchorPoint().y;
    
    for(i = 0; i<itsHSegmentList.size(); i++){
      aSegment = (ErmesConnSegment)(itsHSegmentList.elementAt(i));
      aSegment.MoveBy(theDeltaH, theDeltaV);
    }
    for(i = 0; i<itsVSegmentList.size(); i++){
      aSegment = (ErmesConnSegment)(itsVSegmentList.elementAt(i));
      aSegment.MoveBy(theDeltaH, theDeltaV);
    }
    UpdateBreakPoints(theDeltaH, theDeltaV);
  }
  
  //--------------------------------------------------------
  //	UpdateBreakPoints
  //--------------------------------------------------------
  public void UpdateBreakPoints(int theDeltaH, int theDeltaV){
    Point aPoint;
    for (Enumeration e = itsPoints.elements(); e.hasMoreElements();) {
      aPoint = (Point)e.nextElement();
      aPoint.x+=theDeltaH;
      aPoint.y+=theDeltaV;
    }
  }
	
  //--------------------------------------------------------
  //	CalcNewPoints
  //  
  //--------------------------------------------------------
  public void CalcNewPoints(){
    Point aPoint1, aPoint2;
    ErmesConnSegment aSegment;
    int i;
    //old break points destrucion
    itsPoints.removeAllElements();
    //new break-points creation
    for (Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();){
      aSegment = (ErmesConnSegment)e.nextElement();
      aPoint1 = new Point(0,0);
      aPoint2 = new Point(0,0);
      aPoint1.x = aSegment.GetStartPoint().x;
      aPoint1.y = aSegment.GetStartPoint().y;
      aPoint2.x = aSegment.GetEndPoint().x;
      aPoint2.y = aSegment.GetEndPoint().y;
      itsPoints.addElement(aPoint1);
      itsPoints.addElement(aPoint2);
    }
  }
  
  //--------------------------------------------------------
  //	Select
  //  select a connection
  //--------------------------------------------------------
  public boolean Select(){
    itsSelected = true;
    return true;
  }
	
  //--------------------------------------------------------
  //	Deselect
  //	deselect a connection
  //--------------------------------------------------------
  public boolean Deselect(){
    itsSelected = false;
    return true;
  }
	
	
  //--------------------------------------------------------
  //	Delete
  //  remove all the connection segments from the sketchpad
  //--------------------------------------------------------
  public void Delete(){
    /*ErmesConnSegment aSegment;
      for (Enumeration e = itsHSegmentList.elements(); e.hasMoreElements();) {
      aSegment = ((ErmesConnSegment)e.nextElement());
      itsSketchPad.remove(aSegment);
      }
      for (Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();) {
      aSegment = ((ErmesConnSegment)e.nextElement());
      itsSketchPad.remove(aSegment);
      }
      //itsFtsConnection.delete();
		*/
  }
  
  
  //--------------------------------------------------------
  //	UpdateAllSegments
  //  reshape the previous and the next segment, 
  //  when a segment is dragged
  //--------------------------------------------------------
  public void UpdateAllSegments(ErmesConnSegment theSegment,boolean theHor,int theDelta){
    ErmesConnSegment aSegment1, aSegment2;
    boolean trovato = false;
    int i;
    if(theHor){
      for(i=0; ((i<itsHSegmentList.size())&&(trovato==false)); i++){
	aSegment1 = (ErmesConnSegment )(itsHSegmentList.elementAt(i));
	if(aSegment1 == theSegment)
	  trovato = true;
      }
      aSegment1 = (ErmesConnSegment)(itsVSegmentList.elementAt(i-1));
      aSegment2 = (ErmesConnSegment )(itsVSegmentList.elementAt(i));
      
      if(!(aSegment1.IsInverted(true, false, theDelta))){
	aSegment1.UpdateEndPt(0, theDelta);
	if(aSegment1.VersoAlto()){
	  aSegment1.itsY+=theDelta;
	  aSegment1.UpdatePreferredSize(0, -theDelta);
	  
	}
	else{
	  aSegment1.UpdatePreferredSize(0, theDelta);
	  
	}
      }
      
      if(!(aSegment2.IsInverted(false, false, theDelta))){
	aSegment2.UpdateStartPt(0, theDelta);
	if(aSegment2.VersoAlto()){
	  aSegment2.UpdatePreferredSize(0, theDelta);
	  
	}
	else{
	  aSegment2.itsY+=theDelta;
	  aSegment2.UpdatePreferredSize(0, -theDelta);
	}
      }
    }
    else{
      for(i=0; ((i<itsVSegmentList.size())&&(trovato==false)); i++){
	aSegment1 = (ErmesConnSegment )(itsVSegmentList.elementAt(i));
	if(aSegment1 == theSegment)
	  trovato = true;
      }
      aSegment1 = (ErmesConnSegment )(itsHSegmentList.elementAt(i-2));
      aSegment2 = (ErmesConnSegment )(itsHSegmentList.elementAt(i-1));
      
      if(!(aSegment1.IsInverted(true, true, theDelta))){
	aSegment1.UpdateEndPt(theDelta, 0);
	if(aSegment1.VersoDestra()){
	  aSegment1.UpdatePreferredSize(theDelta, 0);
	}
	else{
	  aSegment1.itsX+=theDelta;
	  aSegment1.UpdatePreferredSize(-theDelta, 0);
	}
      }
      if(!(aSegment2.IsInverted(false, true, theDelta))){
	aSegment2.UpdateStartPt(theDelta, 0);
	if(aSegment2.VersoDestra()){
	  aSegment2.itsX+=theDelta;	
	  aSegment2.UpdatePreferredSize(-theDelta, 0);
	}
	else{
	  aSegment2.UpdatePreferredSize(theDelta, 0);
	}
      }
    }
  }
	
  public void ReroutingConn(){
    if(itsAutorouted){
      //qui trasforma la connessione da autorouted in non-autorouted
      if(!itsErrorState) {
	itsSketchPad.RemoveConnRgn(this);
	itsOutlet.itsConnectionSet.Remove(this);
      }
      Delete();
      PrepareToRouting();
      AutoRouting();
      
    }
    else{
      itsAutorouted = true;
      AutoRouting();
    }
    if(!itsErrorState) {
      itsSketchPad.SaveConnectionRgn(this);
      itsOutlet.itsConnectionSet.Add(this);
    }
    Select();
    itsSketchPad.itsSelectedConnections.addElement(this);
  }
  
  public void ChangeRoutingMode(){
    if(itsAutorouted){
      //qui trasforma la connessione da autorouted in non-autorouted
      if(!itsErrorState) {
	itsSketchPad.RemoveConnRgn(this);
	itsOutlet.itsConnectionSet.Remove(this);
      }
      Delete();
      PrepareToRouting();
      itsAutorouted = false;
    }
    else{
      itsAutorouted = true;
      AutoRouting();
      if(!itsErrorState) {
	itsSketchPad.SaveConnectionRgn(this);
	itsOutlet.itsConnectionSet.Add(this);
      }
    }
    Select();
    itsSketchPad.itsSelectedConnections.addElement(this);
    itsSketchPad.SetStartSelect();
  }
  
	
  public boolean MouseDown(MouseEvent evt,int x, int y) {
    if (!itsSketchPad.itsRunMode){
      itsSketchPad.ClickOnConnection(evt, x, y);
    }
    return true;
  }
	
	
	
  ////////////////////////// codice rubato senza nessuno scrupolo
	
  boolean IsNearToPoint(int x, int y){
    if((!itsAutorouted)||(itsErrorState)){
      int dx = java.lang.Math.abs(itsEndPoint.x - itsStartPoint.x);
      int dy = java.lang.Math.abs(itsEndPoint.y - itsStartPoint.y);
      
      if (dx > dy) return (SegmentIntersect(x, y-3, x, y+3, itsStartPoint.x,
					    itsStartPoint.y,itsEndPoint.x, itsEndPoint.y));
      else return (SegmentIntersect(x-3, y, x+3, y, itsStartPoint.x, itsStartPoint.y,
				    itsEndPoint.x, itsEndPoint.y));
    }
    else{
      ErmesConnSegment aSegment;
      boolean trovato= false;
      for(Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();){
	aSegment = (ErmesConnSegment)e.nextElement();
	if(aSegment.NearToPoint(x,y)){
	  trovato = true;
	  itsSketchPad.MoveSegment(aSegment, x, y);
	  break;
	}
      }
      if(!trovato){
	for (Enumeration e = itsHSegmentList.elements(); e.hasMoreElements();){
	  aSegment = (ErmesConnSegment)e.nextElement();
	  if(aSegment.NearToPoint(x,y)){
	    trovato = true;
	    itsSketchPad.MoveSegment(aSegment, x, y);
	  }
	}
      }
      return trovato;
    }
  }


  boolean SegmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
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
	
  public void Paint(Graphics g){
    if(!itsSketchPad.itsGraphicsOn) return;
    debug_count += 1;
    if (debug_count % ErmesSketchPad.DEBUG_COUNT == 0) {
      /* System.out.print("uscito in ErmesConnection.paint ");
	 System.out.println(debug_count); */
    }
    
    if((itsAutorouted)&&(!itsErrorState)){
      ErmesConnSegment aSegment;
      for (Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();) {
	aSegment = (ErmesConnSegment)e.nextElement();
	aSegment.Paint(g);
      }
      for (Enumeration e1 = itsHSegmentList.elements(); e1.hasMoreElements();) {
	aSegment = (ErmesConnSegment)e1.nextElement();
	aSegment.Paint(g);
      }
    }
    else{
      g.setColor(Color.black);
      if(itsSelected){
	if(java.lang.Math.abs(itsStartPoint.x-itsEndPoint.x)>50){
	  g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
	  g.drawLine(itsStartPoint.x, itsStartPoint.y+1, itsEndPoint.x, itsEndPoint.y+1);	
	}
	else{
	  g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
	  g.drawLine(itsStartPoint.x-1, itsStartPoint.y, itsEndPoint.x-1, itsEndPoint.y);	
	}
      }
      else
	g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
    }
    /*if(!itsAutorouted){
      g.setColor(Color.black);
      if(itsSelected){
      if(java.lang.Math.abs(itsStartPoint.x-itsEndPoint.x)>50){
      g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
      g.drawLine(itsStartPoint.x, itsStartPoint.y+1, itsEndPoint.x, itsEndPoint.y+1);	
      }
      else{
      g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
      g.drawLine(itsStartPoint.x-1, itsStartPoint.y, itsEndPoint.x-1, itsEndPoint.y);	
      }
      }
      else
      g.drawLine(itsStartPoint.x, itsStartPoint.y, itsEndPoint.x, itsEndPoint.y);
      }
      else{
      ErmesConnSegment aSegment;
      for (Enumeration e = itsVSegmentList.elements(); e.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e.nextElement();
      aSegment.Paint(g);
      }
      for (Enumeration e1 = itsHSegmentList.elements(); e1.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e1.nextElement();
      aSegment.Paint(g);
      }
      }*/
  }


}
