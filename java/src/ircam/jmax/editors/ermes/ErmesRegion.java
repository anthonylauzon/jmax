package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

/**
 * A "topological" region, and the associated services.
 * used by the graphic objects and connections.
 */
class ErmesRegion
{
  private Vector itsRegions;
  
  //--------------------------------------------------------
  //	Constructor
  //--------------------------------------------------------
  public ErmesRegion()
  {
    itsRegions = new Vector();
  }
	
  //--------------------------------------------------------
  //	Copy constructor
  //--------------------------------------------------------
  public ErmesRegion(ErmesRegion theRgn)
  {
    Rectangle aRect;
    itsRegions = new Vector();
    for(int i=0; i<theRgn.itsRegions.size() ; i++)
      {
	aRect = new Rectangle();
	aRect = (Rectangle)theRgn.itsRegions.elementAt(i);
	itsRegions.addElement(aRect);
      }
  }
  
  //--------------------------------------------------------
  //	Constructor
  //--------------------------------------------------------
  public ErmesRegion(Rectangle theRect)
  {
    itsRegions = new Vector();
    Rectangle aRect = new Rectangle();
    aRect = theRect;
    itsRegions.addElement(aRect);
  }
  
	
  //--------------------------------------------------------
  //	IsEmpty
  //--------------------------------------------------------
  public boolean IsEmpty()
  {
    if(itsRegions.size() == 0) return true;
    else return false;
  }
  
  //--------------------------------------------------------
  //	Remove
  //--------------------------------------------------------
  public void Remove(Rectangle theRect){
    Rectangle aRect = new Rectangle();
    for(Enumeration e = itsRegions.elements(); e.hasMoreElements();) {
      aRect = (Rectangle)e.nextElement();
      if(aRect.equals(theRect)){
	itsRegions.removeElement(aRect);
	return;
      }
    }			
  }
  
  public void RemoveAllElements(){
    itsRegions.removeAllElements();
  }

  //--------------------------------------------------------
  //	Add
  //--------------------------------------------------------
  public void Add(Rectangle theRect){
    Rectangle aRect = new Rectangle();
    aRect.x = theRect.x;
    aRect.y = theRect.y;
    aRect.width = theRect.width;
    aRect.height = theRect.height;
    itsRegions.addElement(aRect);
  }
  
  //--------------------------------------------------------
  //	PointInRgn
  //--------------------------------------------------------
  public boolean PointInRgn(Point thePoint)
  {
    Rectangle aRect;
    Point aPoint = new Point(thePoint.x,thePoint.y);
    for(int i=0; i<itsRegions.size() ; i++){
      aRect = (Rectangle)itsRegions.elementAt(i);
      if(aRect.contains(aPoint.x, aPoint.y)) return true;
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	RectInRgn
  //--------------------------------------------------------
  public boolean RectInRgn(Rectangle theRect){
    Rectangle aRect;
    boolean aBoolean = false;
    
    for(int i=0; i<itsRegions.size() ; i++){
      aRect = (Rectangle)itsRegions.elementAt(i);
      if(aRect.intersects(theRect)){
	aBoolean = true;
	break;
      }
    }
    return aBoolean;
  }
  
  //--------------------------------------------------------
  //	IsInRgn
  //--------------------------------------------------------
  public boolean IsInRgn(Rectangle theRect){
    return itsRegions.contains(theRect);
  }
  

  public void LeggiRegione(){
    Rectangle aRect = new Rectangle();
    int k = itsRegions.size();
    int i = 0;
    for(i = 0; i< k; i++) {
      aRect = (Rectangle)itsRegions.elementAt(i);
    }
  }	
	
  //--------------------------------------------------------
  //	MinimumRect
  //--------------------------------------------------------
  public Rectangle MinimumRect(){
    int top, left, bottom, right;
    Rectangle aRect;
    aRect = (Rectangle)itsRegions.elementAt(0);
    top = aRect.y;left = aRect.x;
    bottom = aRect.y + aRect.height;
    right = aRect.x + aRect.width;
    	
    for(int i=1; i<itsRegions.size() ; i++){
      aRect = (Rectangle)itsRegions.elementAt(i);
      if(aRect.x<left) left = aRect.x;
      if(aRect.y<top) top = aRect.y;
      if(aRect.y+aRect.height>bottom) bottom = aRect.y + aRect.height;
      if(aRect.x +aRect.width>right) right = aRect.x + aRect.width;
    }
    Rectangle minimumRect = new Rectangle(left, top, right-left, bottom-top);
    return minimumRect;
  }
}

