package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;
import ircam.jmax.utils.List;
import ircam.jmax.utils.ListIterator;

/**
 * A "topological" region, and the associated services.
 * used by the graphic objects and connections.
 * A region is a list of shapes.
 */
class ErmesRegion
{
  private List itsAreas;
  private Rectangle minimumRect;
  
  //--------------------------------------------------------
  //	Constructor
  //--------------------------------------------------------
  public ErmesRegion()
  {
    itsAreas = new List();
    minimumRect = new Rectangle();
  }
	
  //--------------------------------------------------------
  //	Copy constructor
  //--------------------------------------------------------
  public ErmesRegion(ErmesRegion theRgn)
  {
    ErmesArea  aErmesArea;

    itsAreas = new List();
    for (Enumeration l = itsAreas.elements(); l.hasMoreElements();) {
      itsAreas.addElement(l.nextElement());
    }
  }
  
  //--------------------------------------------------------
  //	Constructor
  //--------------------------------------------------------
  public ErmesRegion(ErmesArea theErmesArea)
  {
    itsAreas = new List();
    itsAreas.addElement(theErmesArea);
  }
  
	
  List getList() {
    return itsAreas;
  }

  //--------------------------------------------------------
  //	IsEmpty
  //--------------------------------------------------------
  public boolean IsEmpty()
  {
    return itsAreas.isEmpty();
  }
  

  public int length() {
    int i=0;
    for(Enumeration li = itsAreas.elements(); li.hasMoreElements();i++){
      li.nextElement();
    }
    return i;
  }
  
  //--------------------------------------------------------
  //	removeArea
  //--------------------------------------------------------
  public void removeArea(ErmesArea theArea){
    itsAreas.removeElement(theArea);
  }
  
  public void addArea(ErmesArea theArea) {
    itsAreas.addElement(theArea);
  }

  public void RemoveAllElements(){
    itsAreas.removeAllElements();
  }

  //--------------------------------------------------------
  //	PointInRgn
  //--------------------------------------------------------
  public boolean PointInRgn(Point thePoint)
  {
    Rectangle aRect;
    
    for(Enumeration li = itsAreas.elements(); li.hasMoreElements();){
      aRect = ((ErmesArea) li.nextElement()).getArea();
      if(aRect.contains(thePoint.x, thePoint.y)) return true;
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	RectInRgn
  //--------------------------------------------------------
  public boolean RectInRgn(Rectangle theRect){
    Rectangle aRect;
    boolean aBoolean = false;
    
    for(Enumeration li = itsAreas.elements(); li.hasMoreElements();){
      aRect = ((ErmesArea) li.nextElement()).getArea();
      if(aRect.intersects(theRect)){
	return true;
      }
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	IsInRgn
  //--------------------------------------------------------
  public boolean IsInRgn(ErmesArea theErmesArea){
    return itsAreas.isInList(theErmesArea);
  }
  

  //--------------------------------------------------------
  //	MinimumRect
  //--------------------------------------------------------
  public Rectangle MinimumRect(){
    int top, left, bottom, right;
    Rectangle aRect;
    aRect = ((ErmesArea) itsAreas.next()).getArea();
    top = aRect.y;left = aRect.x;
    bottom = aRect.y + aRect.height;
    right = aRect.x + aRect.width;
    	
    for(Enumeration li = itsAreas.elements(); li.hasMoreElements();){
      aRect = ((ErmesArea) li.nextElement()).getArea();
      if(aRect.x<left) left = aRect.x;
      if(aRect.y<top) top = aRect.y;
      if(aRect.y+aRect.height>bottom) bottom = aRect.y + aRect.height;
      if(aRect.x +aRect.width>right) right = aRect.x + aRect.width;
    }
    minimumRect.setBounds(left, top, right-left, bottom-top);
    return minimumRect;
  }

  public void paintRegion(Graphics g) {
    Rectangle aRect;

    g.setColor(Color.red);
    for(Enumeration li = itsAreas.elements(); li.hasMoreElements();){
      aRect = ((ErmesArea) li.nextElement()).getArea();
      g.fillRect(aRect.x, aRect.y, aRect.width, aRect.height);
    }
  }

  public void writeRegionOnErrorStream(){
    System.err.println("");
    for (Enumeration li = itsAreas.elements(); 
	 li.hasMoreElements();) {
      System.err.println(((ErmesArea) li.nextElement()).getArea().toString());
    }
  }
}

