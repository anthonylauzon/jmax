package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;
import ircam.jmax.editors.ermes.*;

/**
 * A Set of connection coming out from an outlet.
 * It handles the circles.
 */
public class ErmesConnectionSet {
		
  int itsCount;
  Vector itsConnectionList;
  Vector itsCircles;
  ErmesObjOutlet itsOutlet;
  ErmesSketchPad itsSketchPad;
  static int itsStep = 3;
  ErmesRegion itsVSegmentRgn, itsHSegmentRgn;
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesConnectionSet(ErmesObjOutlet theOutlet){
    itsConnectionList = new Vector();
    itsCircles = new Vector();
    itsCount = 0;
    itsOutlet = theOutlet;
    itsSketchPad = itsOutlet.GetOwner().GetSketchPad();
    itsHSegmentRgn = new ErmesRegion();
    itsVSegmentRgn = new ErmesRegion();
  }
		
  //--------------------------------------------------------
  //	GetHSegmentRgn
  //--------------------------------------------------------
  public ErmesRegion GetHSegmentRgn(){
    return itsHSegmentRgn;
  }
  
  //--------------------------------------------------------
  //	GetVSegmentRgn
  //--------------------------------------------------------
  public ErmesRegion GetVSegmentRgn(){
    return itsVSegmentRgn;
  }
  
  //--------------------------------------------------------
  // Add
  // add a connection to the ConnectionSet
  //--------------------------------------------------------
  public void  Add(ErmesConnection theConnection){
    itsConnectionList.addElement(theConnection);
    SaveRgn(theConnection);
    itsCount++; 
    UpdateCircles();
  }
  
  //--------------------------------------------------------
  // Remove
  // remove a connection to the ConnectionSet
  //--------------------------------------------------------
  public void Remove(ErmesConnection theConnection){
    itsConnectionList.removeElement(theConnection);
    RemoveRgn(theConnection);
    itsCount--;
    UpdateCircles();
  }
  
   public void RemoveAllConnections(){
    itsConnectionList.removeAllElements();
    itsHSegmentRgn.RemoveAllElements();
    itsVSegmentRgn.RemoveAllElements();
    itsCount = 0;
    itsCircles.removeAllElements();
  }



  //--------------------------------------------------------
  // FetchItem
  //--------------------------------------------------------
  public ErmesConnection FetchItem(int theIndex){
    return (ErmesConnection)(itsConnectionList.elementAt(theIndex));
  }
  
  //--------------------------------------------------------
  // GetCount
  //--------------------------------------------------------
  public int GetCount(){
    return itsCount;
  }
		
  public Vector GetCircles(){
    return itsCircles;
  }
  
  public ErmesSketchPad GetSketchPad(){
    return itsSketchPad;
  }
  
  
  //--------------------------------------------------------
  // IsInList
  //--------------------------------------------------------		
  public boolean IsInList(ErmesConnection theConnection){
    if(itsConnectionList.contains(theConnection))
      return true;
    else
      return false;
  }
		
  //--------------------------------------------------------
  //	Circles
  //	determine the points where draw the circles
  //--------------------------------------------------------	
  public void Circles(){
    
    ErmesConnCircle aCircle;
    ErmesConnection aConnection;
    Point aPoint = new Point(0,0);Point aPointH1 = new Point(0,0);
    Point aPointH2 = new Point(0,0);Point aPointV1 = new Point(0,0);
    Point aPointV2 = new Point(0,0); Point aMemPoint = new Point(0,0);
    
    for (Enumeration e = itsConnectionList.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      if(!(aConnection.GetErrorState())){
	//RemoveRgn(aConnection);//debug
	for(int j = 1; j<(aConnection.GetPointList().size())-1; j++){
	  aPoint = (Point)(aConnection.GetPointList().elementAt(j));
	  aPointH1.y = aPointH2.y = aPoint.y;
	  aPointH1.x = aPoint.x + itsStep;
	  aPointH2.x = aPoint.x - itsStep;
	  aPointV1.x = aPointV2.x = aPoint.x;
	  aPointV1.y = aPoint.y + itsStep;
	  aPointV2.y = aPoint.y - itsStep;
	  if((itsHSegmentRgn.PointInRgn(aPoint)&&itsHSegmentRgn.PointInRgn(aPointH1)&&
	      itsHSegmentRgn.PointInRgn(aPointH2)&&itsVSegmentRgn.PointInRgn(aPointV1))||
	     (itsHSegmentRgn.PointInRgn(aPoint)&&itsHSegmentRgn.PointInRgn(aPointH1)&&
	      itsHSegmentRgn.PointInRgn(aPointH2)&&itsVSegmentRgn.PointInRgn(aPointV2))||
	     (itsVSegmentRgn.PointInRgn(aPoint)&&itsVSegmentRgn.PointInRgn(aPointV1)&&
	      itsVSegmentRgn.PointInRgn(aPointV2)&&itsHSegmentRgn.PointInRgn(aPointH1))||
	     (itsVSegmentRgn.PointInRgn(aPoint)&&itsVSegmentRgn.PointInRgn(aPointV1)&&
	      itsVSegmentRgn.PointInRgn(aPointV2)&&itsHSegmentRgn.PointInRgn(aPointH2))){
	    if(!CircleInList(aPoint)){//controllo per non inserire punti doppi	
	      aCircle = new ErmesConnCircle(aPoint.x-2, aPoint.y-2, this);
	      itsCircles.addElement(aCircle);
	    }
	  }		   	
	}
      }
    }
  }
		
		
  //--------------------------------------------------------
  // RemoveRgn
  //--------------------------------------------------------	
  public void RemoveRgn(ErmesConnection theConnection){
    Rectangle aRect;
    ErmesConnSegment aSegment;
    for (Enumeration e = theConnection.GetHSegmentList().elements() ; e.hasMoreElements() ;) {
      aSegment = (ErmesConnSegment)e.nextElement();
      aRect = aSegment.Bounds();
      itsHSegmentRgn.Remove(aRect);
    }
    for (Enumeration e1 = theConnection.GetVSegmentList().elements() ; e1.hasMoreElements() ;) {
      aSegment = (ErmesConnSegment)e1.nextElement();
      aRect = aSegment.Bounds();
      itsVSegmentRgn.Remove(aRect);
    }
    
  }
		
  //--------------------------------------------------------
  // SaveRgn
  //--------------------------------------------------------	
  public void SaveRgn(ErmesConnection theConnection){
    Rectangle aRect;
    ErmesConnSegment aSegment;
    for (Enumeration e = theConnection.GetHSegmentList().elements() ; e.hasMoreElements() ;) {
      aSegment = (ErmesConnSegment)e.nextElement();
      aRect = aSegment.Bounds();
      itsHSegmentRgn.Add(aRect);
    }
    for (Enumeration e1 = theConnection.GetVSegmentList().elements() ; e1.hasMoreElements() ;) {
      aSegment = (ErmesConnSegment)e1.nextElement();
      aRect = aSegment.Bounds();
      itsVSegmentRgn.Add(aRect);
    }
    
  }
  
  //--------------------------------------------------------
  //	CircleInList
  //	control if a point is in the list of new circles(used by Circles function)
  //--------------------------------------------------------	
  public boolean CircleInList(Point thePoint){
    ErmesConnCircle aCircle;
    Point aPoint = new Point(0,0);
    for (Enumeration e = itsCircles.elements(); e.hasMoreElements();) {
      aCircle = (ErmesConnCircle)e.nextElement();
      aPoint.x = aCircle.GetX()+2;
      aPoint.y = aCircle.GetY()+2;
      if ((aPoint.x == thePoint.x)&&(aPoint.y == thePoint.y)) return true;
    }
    return false;
  }
  
  
  
  //--------------------------------------------------------
  // DrawCircles
  //--------------------------------------------------------	
  public void DrawCircles(Graphics g){
    ErmesConnCircle aCircle;
    for (Enumeration e = itsCircles.elements(); e.hasMoreElements();) {
      aCircle = (ErmesConnCircle)e.nextElement();
      aCircle.Paint(g);
    }
  }
  
		
  //--------------------------------------------------------
  // UpdateCircles
  //--------------------------------------------------------	
  public void UpdateCircles(){
    itsCircles.removeAllElements();
    Circles();
  }
  
  
  public void Refresh(){
    ErmesConnection aConnection;
    for (Enumeration e = itsConnectionList.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection)e.nextElement();
    }
  }
  
}










