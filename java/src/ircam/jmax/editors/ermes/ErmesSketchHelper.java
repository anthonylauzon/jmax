package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;
import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.project.*;
 
/**
 * An helper class used by the SketchPad.
 * It contains only methods accessed by the sketchpad.
 */
class ErmesSketchHelper extends Object{
  ErmesSketchPad itsSketchPad;
  Rectangle normalizedRect = new Rectangle();


  public ErmesSketchHelper(ErmesSketchPad theSketchPad) {
    itsSketchPad = theSketchPad;
  }
  
  //--------------------------------------------------------
  //	AddConnection
  //  adding a connection between two objects
  //--------------------------------------------------------
  ErmesConnection AddConnection(ErmesObject fromObj, ErmesObject toObj, int fromOutlet, int toInlet, FtsConnection fc) {
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;
    
    ErmesConnection aConnection = new ErmesConnection(fromObj, toObj, itsSketchPad, fromOutlet, toInlet, fc);
    aConnection.update(fc);
    return aConnection;
  }
	
  //--------------------------------------------------------
  //	AddObject
  //  adding an object of the given class name in the given location
  //--------------------------------------------------------
  ErmesObject AddObject(Class theClass, FtsObject theFtsObject) {

    ErmesObject aObject = null;	//wasting time...
    Rectangle aRect;
    ErmesObjOutlet aOutlet;
    
    if(itsSketchPad.doSnapToGrid){
      int x, y;

      x = ((Integer)theFtsObject.get("x")).intValue();
      y = ((Integer)theFtsObject.get("y")).intValue();
      //Point aPoint = SnapToGrid(x, y);
      //theFtsObject.put("x", x);
      //theFtsObject.put("y", y);
    }

    try
      {
      aObject = (ErmesObject) theClass.newInstance();
      aObject.Init(itsSketchPad, theFtsObject);
      }
    catch(IllegalAccessException e)
      {
	System.out.println("Internal Error: ErmesSketchHelper.AddObject(" +
			   theClass.getName() + "," + theFtsObject +
			   ") : illegal access" + e);
	return null;
      }
    catch(InstantiationException e) 
      {
	System.out.println("Internal Error: ErmesSketchHelper.AddObject(" +
			   theClass.getName() + "," + theFtsObject +
			   ") : instantiation exception " + e);
	return null;
      }

    itsSketchPad.itsElements.addElement(aObject);
    if (!itsSketchPad.itsToolBar.locked) itsSketchPad.editStatus = itsSketchPad.DOING_NOTHING;	
    return aObject;
  }
  
  //--------------------------------------------------------
  //	ChangeObjectPrecedence
  //--------------------------------------------------------
  public void ChangeObjectPrecedence(ErmesObject theObject){
    itsSketchPad.itsElements.removeElement(theObject);
    itsSketchPad.itsElements.addElement(theObject);	
  }
  
  //--------------------------------------------------------
  //	DeleteObjectConnections
  //	delete the object's connections
  //--------------------------------------------------------
  public void DeleteObjectConnections(ErmesObject theObject, boolean paintNow){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(Enumeration e = theObject.GetInletList().elements() ; e.hasMoreElements() ;) {
      aInlet = (ErmesObjInlet) e.nextElement();
      while (!aInlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aInlet.GetConnections().firstElement();
	DeleteConnection(aConnection, false);
      }
    }
    for(Enumeration e1 = theObject.GetOutletList().elements() ; e1.hasMoreElements() ;) {
      aOutlet = (ErmesObjOutlet) e1.nextElement();
      while (!aOutlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aOutlet.GetConnections().firstElement();
	DeleteConnection(aConnection, false);
      }
    }
    if (paintNow) itsSketchPad.paintDirtyList();
  }
  
  //--------------------------------------------------------
  //	DeleteConnectionByInOut
  //	delete one connection routine
  //--------------------------------------------------------
  
  public void DeleteConnectionByInOut(ErmesObject srcObj, int srcOut, ErmesObject destObj, int destIn, boolean paintNow)
  {
    ErmesObjOutlet out;
    ErmesObjInlet  in;
    ErmesObject aObject = null;
    ErmesConnection aConnection = null;

    out = (ErmesObjOutlet) srcObj.itsOutletList.elementAt(srcOut);
    in  = (ErmesObjInlet) destObj.itsInletList.elementAt(destIn);
    
    for(Enumeration e = out.itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection) e.nextElement();
      if(aConnection.itsInlet == in) break;
    }

    if(aConnection!=null) {
      DeleteConnection(aConnection, false);
      if (paintNow) itsSketchPad.paintDirtyList();
    }
  }


  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one connection routine
  //--------------------------------------------------------
  public void DeleteConnection(ErmesConnection theConnection, boolean paintNow) {
    ErmesObjOutlet aOutlet = theConnection.GetOutlet();
    ErmesObjInlet aInlet = theConnection.GetInlet();
    
    itsSketchPad.itsSelectedConnections.removeElement(theConnection);
    itsSketchPad.itsConnections.removeElement(theConnection);
    aOutlet.GetConnections().removeElement(theConnection);
    aInlet.GetConnections().removeElement(theConnection);
    if(aInlet.GetConnections().size()==0) aInlet.SetConnected(false, false);
    if(aOutlet.GetConnections().size()==0) aOutlet.SetConnected(false, false);
    if (theConnection.itsFtsConnection != null) theConnection.itsFtsConnection.delete();	//delete from FTS
    itsSketchPad.markSketchAsDirty();
    if (paintNow) itsSketchPad.paintDirtyList();
  }


  //--------------------------------------------------------
  //	DeleteObject
  //	delete one object routine
  //--------------------------------------------------------
  public void DeleteObject(ErmesObject theObject, boolean paintNow) {
    DeleteGraphicObject(theObject, paintNow);
    if (theObject.itsFtsObject != null) theObject.itsFtsObject.delete();
  }
  
  public void DeleteGraphicObject(ErmesObject theObject, boolean paintNow) {
    DeleteObjectConnections(theObject, false);
    //removes theObject from the selected elements list	
    if(theObject.NeedPropertyHandler())
      if(theObject.GetFtsObject()!=null) theObject.GetFtsObject().removeWatch(theObject);
    itsSketchPad.itsSelectedList.removeElement(theObject);
    //removes theObject from the element list (delete)
    itsSketchPad.itsElements.removeElement(theObject);
    itsSketchPad.RemoveInOutlets(theObject);
    itsSketchPad.markSketchAsDirty();
    
    if(theObject instanceof ErmesObjExternal)
      if (((ErmesObjExternal)theObject).itsSubWindow!= null) 
	((ErmesObjExternal)theObject).itsSubWindow.dispose();
    if(theObject instanceof ErmesObjPatcher) {
      ErmesObjPatcher aPatcher = (ErmesObjPatcher)theObject;
      if (aPatcher.itsSubWindow!= null) aPatcher.itsSubWindow.Close(true);
    }
    if (paintNow) itsSketchPad.paintDirtyList();
  }
  
  //--------------------------------------------------------
  //	DeleteSelected
  //	delete routine
  //--------------------------------------------------------
  public void DeleteSelected() {
    ErmesObject aObject;
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;

    if(!DeleteInOutletConnections(false)){
      while (!itsSketchPad.itsSelectedList.isEmpty()) {
	aObject = (ErmesObject) itsSketchPad.itsSelectedList.firstElement();
	DeleteObject(aObject, false);
      }
      
      while(!itsSketchPad.itsSelectedConnections.isEmpty()){
	int i=0;
	aConnection = (ErmesConnection) itsSketchPad.itsSelectedConnections.firstElement();
	DeleteConnection(aConnection, false);
      }
    }
    
    itsSketchPad.GetSketchWindow().DeselectionUpdateMenu();
    itsSketchPad.ToSave();
    itsSketchPad.paintDirtyList();
  }
    
  public boolean DeleteInOutletConnections(boolean paintNow){
    ErmesObjInOutlet aInOutlet;
    boolean ret = false;

    if(itsSketchPad.itsConnectingLetList.size()==0){
      if(itsSketchPad.itsConnectingLet!=null){
	DeleteThisInOutletConn(itsSketchPad.itsConnectingLet, false);
	itsSketchPad.ResetConnect();
	ret = true;
      }
    }
    else{
      for (Enumeration e = itsSketchPad.itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	DeleteThisInOutletConn(aInOutlet, false);
      }
      itsSketchPad.ResetConnect();
      ret = true;
    }
    if (paintNow) itsSketchPad.paintDirtyList();
    return ret;
  }

  public void DeleteThisInOutletConn(ErmesObjInOutlet theInOutlet, boolean paintNow){
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;
    
      while (theInOutlet.GetConnections().size() != 0) {
	aConnection = (ErmesConnection)theInOutlet.GetConnections().elementAt(0);
	DeleteConnection(aConnection, false);
      }
      
      theInOutlet.GetConnections().removeAllElements();
      theInOutlet.ChangeState(false, false, false);
      if (paintNow) itsSketchPad.paintDirtyList();
  }

  //--------------------------------------------------------
  //	deselectObjects
  //	deselect all the objects of a given list
  //--------------------------------------------------------
  
  void deselectObjects(Vector theObjects, boolean paintNow){
    ErmesObject aObject;
    for (Enumeration e = theObjects.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Deselect(false);
    }
    if (paintNow) {
      itsSketchPad.paintDirtyList();
    }
  }
  //--------------------------------------------------------
  //	deselectConnections
  //	deselect all the connections of a given list
  //--------------------------------------------------------

  void deselectConnections(Vector theConnections, boolean paintNow){
    ErmesConnection aConnection;
    for (Enumeration e = theConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Deselect(false);
    }
    if (paintNow) {
      itsSketchPad.paintDirtyList();
    }
  }


  /**
   * Deselect everything selected in the sketch 
   * (objects, connections, current selected in/outlet).
   * This function handles the Focus change and updates the menus.
   */
  public void deselectAll(boolean paintNow) {
    if (itsSketchPad.GetEditField() != null && itsSketchPad.GetEditField().HasFocus()) {
      itsSketchPad.GetEditField().transferFocus();
    }
    if(itsSketchPad.editStatus == ErmesSketchPad.EDITING_OBJECT){
      itsSketchPad.GetEditField().LostFocus();
    }

    if(itsSketchPad.itsSelectedList.size() != 0) 
      itsSketchPad.GetSketchWindow().DeselectionUpdateMenu();

    deselectCurrentInOutlet(false);
    deselectObjects(itsSketchPad.itsSelectedList, false);
    deselectConnections(itsSketchPad.itsSelectedConnections, false);
    
    itsSketchPad.itsSelectedList.removeAllElements();
    itsSketchPad.itsSelectedConnections.removeAllElements();

    if (paintNow) {
      itsSketchPad.paintDirtyList();
    }
    if (itsSketchPad.GetEditField() != null && itsSketchPad.GetEditField().HasFocus()) {
      itsSketchPad.GetEditField().transferFocus();
    }
    if(itsSketchPad.editStatus == ErmesSketchPad.EDITING_OBJECT){
      itsSketchPad.GetEditField().LostFocus();
    }
  }

  public void deselectCurrentInOutlet(boolean paintNow){
    ErmesObjInOutlet aInOutlet;
    if(itsSketchPad.itsConnectingLet!=null) 
      itsSketchPad.itsConnectingLet.ChangeState(false, itsSketchPad.itsConnectingLet.connected, false);
    if(itsSketchPad.itsConnectingLetList.size()!=0){
      for (Enumeration e = itsSketchPad.itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	aInOutlet.ChangeState(false, aInOutlet.GetConnected(), false);
      }
    }
    itsSketchPad.ResetConnect();
    if (paintNow) itsSketchPad.paintDirtyList();
  }

  
  //--------------------------------------------------------
  //	GetDrawableRect
  //--------------------------------------------------------
  Rectangle GetDrawableRect(Rectangle originalRect, Dimension drawingArea) {
    int x = originalRect.x;
    int y = originalRect.y;
    int width = originalRect.width;
    int height = originalRect.height;
    
    //Make sure rectangle width and height are positive.
    if (width < 0) {
      width = 0 - width;
      x = x - width + 1;
      if (x < 0) {
	width += x;
	x = 0;
      }
    }
    if (height < 0) {
      height = 0 - height;
      y = y - height + 1;
      if (y < 0) {
	height += y;
	y = 0;
      }
    }
    
    //The rectangle shouldn't extend past the drawing area.
    if ((x + width) > drawingArea.width) {
      width = drawingArea.width - x;
    }
    if ((y + height) > drawingArea.height) {
      height = drawingArea.height - y;
    }
    
    return new Rectangle(x, y, width, height);
  }
  
  //--------------------------------------------------------
  //	IsHorizontal
  //--------------------------------------------------------
  public boolean IsHorizontal(Rectangle theRect)
  {
    if(theRect.height <= 3)
      return true;
    else
      return false;
  }
  
  //--------------------------------------------------------
  //	IsInConnection
  //--------------------------------------------------------
  public boolean IsInConnection(int x, int y){
    ErmesConnection aConnection;
    Rectangle aRect;
    for(Enumeration e = itsSketchPad.itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection)e.nextElement();
      if(aConnection.IsNearToPoint(x, y)) {
	itsSketchPad.itsCurrentConnection = aConnection;
	return true;
      }
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	IsInInOutLet
  //--------------------------------------------------------
  public boolean IsInInOutLet(int x, int y){
    ErmesObject aObject;
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    Rectangle aRect;

    for(Enumeration e = itsSketchPad.itsInletList.elements(); e.hasMoreElements();) {
      aInlet = (ErmesObjInlet)e.nextElement();
      aRect = aInlet.Bounds();
      if(aRect.contains(x,y)) {
	itsSketchPad.itsCurrentInOutlet = aInlet;
	return true;
      }
    }
    //for(Enumeration e2 = aObject.GetOutletList().elements(); e2.hasMoreElements();) {
    for(Enumeration e2 = itsSketchPad.itsOutletList.elements(); e2.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet)e2.nextElement();
      aRect = aOutlet.Bounds();
      if(aRect.contains(x,y)) {
	itsSketchPad.itsCurrentInOutlet = aOutlet;
	return true;
      }
    }
    //  }
    return false;
  }
  
  //--------------------------------------------------------
  //	IsInObject
  //--------------------------------------------------------
  public boolean IsInObject(int x, int y){
    ErmesObject aObject;
    Rectangle aRect;
    for(int i = itsSketchPad.itsElements.size()-1; i>=0;i--){//???
      aObject = (ErmesObject)itsSketchPad.itsElements.elementAt(i);//????
      aRect = aObject.Bounds();
      if(aRect.contains(x,y)) {
	itsSketchPad.itsCurrentObject = aObject;
	return true;
      }
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	MoveElements
  //	Move the selected elements
  //--------------------------------------------------------
  public void MoveElements(int theDeltaH, int theDeltaV){
    ErmesObject aObject;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      aObject.MoveBy(theDeltaH, theDeltaV);
    }
  }



  //--------------------------------------------------------
  //	NormalizedRect
  //--------------------------------------------------------
  public Rectangle NormalizedRect(Rectangle theRect) {
    
    normalizedRect.x = (theRect.width < 0)? theRect.x + theRect.width: theRect.x;
    normalizedRect.y = (theRect.height < 0)? theRect.y +theRect.height: theRect.y;
    normalizedRect.width = (theRect.width < 0)? - theRect.width: theRect.width;
    normalizedRect.height = (theRect.height < 0)? - theRect.height: theRect.height;
    
    return normalizedRect;
  }
  
  //--------------------------------------------------------
  //	SearchFtsName
  //  corrispondence between fts names and ermes names (new and old format...)
  //--------------------------------------------------------
  public Class SearchFtsName(FtsObject fo)
  {
    String theName = fo.getClassName();

    if (itsSketchPad.nameTable.containsKey(theName))
      return (Class) itsSketchPad.nameTable.get(theName);
    else {
      return ircam.jmax.editors.ermes.ErmesObjExternal.class;
    }
  }
  
  //--------------------------------------------------------
  //	SetSnapToGrid
  //	set the snap to grid flag and snap the existing objects
  //--------------------------------------------------------
  public void SetSnapToGrid(){
    ErmesObject aObject;
    Point aPoint;
    int aDeltaH, aDeltaV;
    itsSketchPad.doSnapToGrid = !itsSketchPad.doSnapToGrid;
    if(itsSketchPad.doSnapToGrid){
      for(int i = 0; i<itsSketchPad.itsElements.size();i++){
	aObject = (ErmesObject) itsSketchPad.itsElements.elementAt(i);
	aPoint = SnapToGrid(aObject.itsX, aObject.itsY);
	aDeltaH = aPoint.x - aObject.itsX;
	aDeltaV = aPoint.y - aObject.itsY ;
	aObject.MoveBy(aDeltaH, aDeltaV);
      }
    }
    itsSketchPad.repaint();
  }
	
  //--------------------------------------------------------
  //	SnapToGrid
  //--------------------------------------------------------
  public Point SnapToGrid(int x, int y){
    Point aPoint;
    int quot = (int)java.lang.Math.floor(x/itsSketchPad.snapGrid.width);
    int res = (x - quot*itsSketchPad.snapGrid.width);
    if(res>itsSketchPad.snapGrid.width/2)
      x = (quot+1)*itsSketchPad.snapGrid.width;
    else
      x = quot*itsSketchPad.snapGrid.width;
    quot = (int)java.lang.Math.floor(y/itsSketchPad.snapGrid.height);
    res = (y - quot*itsSketchPad.snapGrid.height);
    if(res>itsSketchPad.snapGrid.height/2)
      y = (quot+1)*itsSketchPad.snapGrid.height;
    else
      y = quot*itsSketchPad.snapGrid.height;
    return aPoint = new Point(x, y);
  }
  
  //--------------------------------------------------------
  //	StartMoveInclusionRect
  //--------------------------------------------------------
  public Rectangle StartMoveInclusionRect(){
    ErmesRegion aRegion = new ErmesRegion();
    ErmesObject aObject;
    Rectangle aRect;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(int i=0; i<itsSketchPad.itsSelectedList.size(); i++){
      aObject = (ErmesObject) itsSketchPad.itsSelectedList.elementAt(i);
      //aRect = aObject.Bounds();
      aRegion.addArea(aObject);
    }
    return aRegion.MinimumRect();
  }
	
  //--------------------------------------------------------
  //	TraceConnection
  //--------------------------------------------------------
  
  public ErmesConnection TraceConnection(ErmesObjOutlet theSourceOutlet, ErmesObjInlet theDestInlet, boolean paintNow){
    ErmesConnection aConnection = new ErmesConnection(itsSketchPad, theDestInlet, theSourceOutlet);
    theSourceOutlet.AddConnection(aConnection);
    theDestInlet.AddConnection(aConnection); 
    itsSketchPad.itsConnections.addElement(aConnection);
    itsSketchPad.ToSave();
    if (paintNow) aConnection.DoublePaint();
    else itsSketchPad.addToDirtyConnections(aConnection);
    return aConnection;
  }
}





