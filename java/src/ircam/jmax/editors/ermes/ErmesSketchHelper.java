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
    
    ErmesConnection aConnection = new ErmesConnection(fromObj, toObj, itsSketchPad, fromOutlet, 
					      toInlet, fc, itsSketchPad.doAutorouting);
    aConnection.update(fc);
    return aConnection;
  }
	
  //--------------------------------------------------------
  //	AddObject
  //  adding an object of the given class name in the given location
  //--------------------------------------------------------
  ErmesObject AddObject(FtsGraphicDescription theFtsDescription, String theName, FtsObject theFtsObject) {

    ErmesObject aObject = null;	//wasting time...
    Rectangle aRect;
    ErmesObjOutlet aOutlet;
    int i;
    
    if(itsSketchPad.doSnapToGrid){
      Point aPoint = SnapToGrid(theFtsDescription.x, theFtsDescription.y);
      theFtsDescription.x = aPoint.x;
      theFtsDescription.y = aPoint.y;
    }
    try {
      //there was an error "aObject may not have been initialized"
      aObject = (ErmesObject) Class.forName(theName).newInstance();
      aObject.Init(itsSketchPad,/* x, y, width, height*/theFtsDescription, theFtsObject);
    } catch(ClassNotFoundException e) {i = 0;}
    catch(IllegalAccessException e) {i = 1;}
    catch(InstantiationException e) {i = 2;}
    finally {
      itsSketchPad.itsElements.addElement(aObject);
      if (!itsSketchPad.itsToolBar.locked) itsSketchPad.editStatus = itsSketchPad.DOING_NOTHING;	
      aRect = new Rectangle(aObject.currentRect.x, aObject.currentRect.y, 
			    aObject.currentRect.width, aObject.currentRect.height);
      aRect.grow(3,6);
      itsSketchPad.itsElementRgn.Add(aRect);
      for (Enumeration e = aObject.GetOutletList().elements(); e.hasMoreElements();) {
	aOutlet = (ErmesObjOutlet)e.nextElement();
	itsSketchPad.itsConnectionSetList.addElement(aOutlet.GetConnectionSet());
      }
    }
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
  public void DeleteObjectConnections(ErmesObject theObject){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(Enumeration e = theObject.GetInletList().elements() ; e.hasMoreElements() ;) {
      aInlet = (ErmesObjInlet) e.nextElement();
      while (!aInlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aInlet.GetConnections().firstElement();
	aOutlet = aConnection.GetOutlet();
	itsSketchPad.itsConnections.removeElement(aConnection);
	aInlet.GetConnections().removeElement(aConnection);
	aOutlet.GetConnections().removeElement(aConnection);
	if(aOutlet.GetConnections().size()==0) {
	  aOutlet.SetConnected(false);
	}
	if(!aConnection.GetErrorState()){
	  itsSketchPad.RemoveConnRgn(aConnection);
	  aConnection.GetConnectionSet().Remove(aConnection);
	}
	if(aConnection.GetSelected()) itsSketchPad.itsSelectedConnections.removeElement(aConnection);
	aConnection.Delete();//remove all the segments from the sketchpad components and finalize 
      }
    }
    for(Enumeration e1 = theObject.GetOutletList().elements() ; e1.hasMoreElements() ;) {
      aOutlet = (ErmesObjOutlet) e1.nextElement();
      while (!aOutlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aOutlet.GetConnections().firstElement();
	aInlet = aConnection.GetInlet();
	itsSketchPad.itsConnections.removeElement(aConnection);
	aInlet.GetConnections().removeElement(aConnection);
	aOutlet.GetConnections().removeElement(aConnection);
	if(aInlet.GetConnections().size()==0) {
	  aInlet.SetConnected(false);
	}
	if(!aConnection.GetErrorState()){
	  itsSketchPad.RemoveConnRgn(aConnection);
	  aConnection.GetConnectionSet().Remove(aConnection);
	}
	if(aConnection.GetSelected()) itsSketchPad.itsSelectedConnections.removeElement(aConnection);
	aConnection.Delete();//remove all the segments from the sketchpad components and finalize 
      }
    }
    itsSketchPad.repaint();
  }
  
  //--------------------------------------------------------
  //	DeleteConnectionByInOut
  //	delete one connection routine
  //--------------------------------------------------------
  
  public void DeleteConnectionByInOut(ErmesObject srcObj, int srcOut, ErmesObject destObj, int destIn)
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
      DeleteConnection(aConnection);
      itsSketchPad.repaint();
    }
  }


  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one object routine
  //--------------------------------------------------------
  public void DeleteConnection(ErmesConnection theConnection) {
    ErmesObjOutlet aOutlet = theConnection.GetOutlet();
    ErmesObjInlet aInlet = theConnection.GetInlet();
    
    itsSketchPad.itsSelectedConnections.removeElement(theConnection);
    itsSketchPad.itsConnections.removeElement(theConnection);
    aOutlet.GetConnections().removeElement(theConnection);
    aInlet.GetConnections().removeElement(theConnection);
    if(aInlet.GetConnections().size()==0) aInlet.SetConnected(false);
    if(aOutlet.GetConnections().size()==0) aOutlet.SetConnected(false);
    if(theConnection.itsAutorouted){
      itsSketchPad.RemoveConnRgn(theConnection);
      theConnection.GetConnectionSet().Remove(theConnection);
    }
    if (theConnection.itsFtsConnection != null) theConnection.itsFtsConnection.delete();	//delete from FTS
  }


  //--------------------------------------------------------
  //	DeleteObject
  //	delete one object routine
  //--------------------------------------------------------
  public void DeleteObject(ErmesObject theObject) {
    itsSketchPad.RemoveElementRgn(theObject);
    DeleteObjectConnections(theObject);
    //removes theObject from the selected elements list	
    itsSketchPad.itsSelectedList.removeElement(theObject);
    //removes theObject from the element list (delete)
    itsSketchPad.itsElements.removeElement(theObject);
    //removes theObject from the container (sketchpad)
    //remove(theObject);	//from the sketchpad components

    if(theObject instanceof ErmesObjExternal)
      if (((ErmesObjExternal)theObject).itsSubWindow!= null) ((ErmesObjExternal)theObject).itsSubWindow.dispose();
    if(theObject instanceof ErmesObjPatcher) {
      itsSketchPad.itsPatcherElements.removeElement(theObject);
      ErmesObjPatcher aPatcher = (ErmesObjPatcher)theObject;
      if(((ErmesSketchWindow)itsSketchPad.itsSketchWindow).GetProjectEntry()!=null){
	if(MaxApplication.getApplication().GetProjectWindow().GetProject().
	   HaveAEntry(aPatcher.GetName()+aPatcher.GetPath())){
	  ProjectEntry aEntry = MaxApplication.getApplication().GetProjectWindow().
	    GetProject().GetTheEntry(aPatcher.GetName()+aPatcher.GetPath());
	  aEntry.DecAbstractionNumber();
	  if(aEntry.GetAbstractionNumber()==0) 
	    MaxApplication.getApplication().GetProjectWindow().GetProject().RemoveFromProject(aEntry);
	}
      }
    }
    if (theObject.itsFtsObject != null) theObject.itsFtsObject.delete();//a Delete() must be implemented in EObject
    //theObject.finalize();	//Oh-Oh... we HOPE!
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

    if(!DeleteInOutletConnections()){
      while (!itsSketchPad.itsSelectedList.isEmpty()) {
	aObject = (ErmesObject) itsSketchPad.itsSelectedList.firstElement();
	DeleteObject(aObject);
      }
      while(!itsSketchPad.itsSelectedConnections.isEmpty()){
	aConnection = (ErmesConnection) itsSketchPad.itsSelectedConnections.firstElement();
	DeleteConnection(aConnection);
      }
    }
    itsSketchPad.ToSave();
    itsSketchPad.repaint();
  }
    
  public boolean DeleteInOutletConnections(){
    ErmesObjInOutlet aInOutlet;
    if(itsSketchPad.itsConnectingLetList.size()==0){
      if(itsSketchPad.itsConnectingLet!=null){
	DeleteThisInOutletConn(itsSketchPad.itsConnectingLet);
	return true;
      }
    }
    else{
      for (Enumeration e = itsSketchPad.itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	DeleteThisInOutletConn(aInOutlet);
      }
      return true;
    }
    return false;
  }

  public void DeleteThisInOutletConn(ErmesObjInOutlet theInOutlet){
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;
    if(theInOutlet.IsInlet()){
      for (Enumeration e = theInOutlet.GetConnections().elements(); e.hasMoreElements();) {
	aConnection = (ErmesConnection)e.nextElement();
	itsSketchPad.itsConnections.removeElement(aConnection);
	aOutlet = aConnection.GetOutlet();
	aOutlet.GetConnections().removeElement(aConnection);
	if(aOutlet.GetConnections().size()==0) aOutlet.ChangeState(false, false);
	else  aOutlet.ChangeState(false, true);
	if(aConnection.itsAutorouted){
	  itsSketchPad.RemoveConnRgn(aConnection);
	  aConnection.GetConnectionSet().Remove(aConnection);
	}
	if (aConnection.itsFtsConnection != null) aConnection.itsFtsConnection.delete();
      }
    }
    else{
      for (Enumeration e = theInOutlet.GetConnections().elements(); e.hasMoreElements();) {
	aConnection = (ErmesConnection)e.nextElement();
	itsSketchPad.itsConnections.removeElement(aConnection);
	aInlet = aConnection.GetInlet();
	aInlet.GetConnections().removeElement(aConnection);
	if(aInlet.GetConnections().size()==0) aInlet.ChangeState(false, false);
	else aInlet.ChangeState(false, true);
	if(aConnection.itsAutorouted)
	  itsSketchPad.RemoveConnRgn(aConnection);
      }
     ((ErmesObjOutlet)theInOutlet).GetConnectionSet().RemoveAllConnections();//svuota la lista
    }
    theInOutlet.GetConnections().removeAllElements();
    theInOutlet.ChangeState(false, false);
    itsSketchPad.ResetConnect();
  }

  //--------------------------------------------------------
  //	DeselectAll
  //	deselect all the objects AND CONNECTIONS currently selected
  //--------------------------------------------------------
  void DeselectAll() {
    ErmesObject aObject;
    ErmesConnection aConnection; 
    if (itsSketchPad.GetEditField() != null && itsSketchPad.GetEditField().HasFocus()) {
      itsSketchPad.GetEditField().nextFocus();
    }
    if(itsSketchPad.editStatus == ErmesSketchPad.EDITING_OBJECT){
      itsSketchPad.GetEditField().LostFocus();
    }
    
    DeselectInOutlet();

    if (itsSketchPad.itsSelectedList.size() == 0 && itsSketchPad.itsSelectedConnections.size() ==0 ) return;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Deselect();
      aObject.Paint(itsSketchPad.offGraphics);
    }
    itsSketchPad.itsSelectedList.removeAllElements();
    
    if(itsSketchPad.itsConnectingLet!=null) {
      itsSketchPad.itsConnectingLet.ChangeState(false, itsSketchPad.itsConnectingLet.GetConnected());
    }
    
    for (Enumeration e = itsSketchPad.itsSelectedConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Deselect();
      aConnection.Update(itsSketchPad.GetOffGraphics());
      aConnection.Paint(itsSketchPad.GetOffGraphics());		     
    }
    itsSketchPad.itsSelectedConnections.removeAllElements();
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());
  }
  
  public void DeselectObjAndConn(){
    ErmesObject aObject;
    ErmesConnection aConnection; 
    if (itsSketchPad.GetEditField() != null && itsSketchPad.GetEditField().HasFocus()) {
      itsSketchPad.GetEditField().nextFocus();
    }
    if(itsSketchPad.editStatus == ErmesSketchPad.EDITING_OBJECT){
      itsSketchPad.GetEditField().LostFocus();
    }
    if (itsSketchPad.itsSelectedList.size() == 0 && itsSketchPad.itsSelectedConnections.size() ==0 ) return;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Deselect();
      aObject.Paint(itsSketchPad.offGraphics);
    }
    itsSketchPad.itsSelectedList.removeAllElements();
    
    if(itsSketchPad.itsConnectingLet!=null) {
      itsSketchPad.itsConnectingLet.ChangeState(false, itsSketchPad.itsConnectingLet.GetConnected());
    }
    
    for (Enumeration e = itsSketchPad.itsSelectedConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Deselect();
      //bug 1aConnection.Repaint();
      aConnection.Update(itsSketchPad.GetOffGraphics());
      aConnection.Paint(itsSketchPad.GetOffGraphics());		     
    }
    itsSketchPad.itsSelectedConnections.removeAllElements();
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());
    itsSketchPad.editStatus = itsSketchPad.DOING_NOTHING;
  }


  void DeselectAll(ErmesConnection theConnection) {
    ErmesObject aObject;
    ErmesConnection aConnection; 
    if (itsSketchPad.GetEditField() != null && itsSketchPad.GetEditField().HasFocus()) {
      itsSketchPad.GetEditField().nextFocus();
    }
    if(itsSketchPad.editStatus == ErmesSketchPad.EDITING_OBJECT){
      itsSketchPad.GetEditField().LostFocus();
    }

    DeselectInOutlet();

    if (itsSketchPad.itsSelectedList.size() == 0 && itsSketchPad.itsSelectedConnections.size() ==0 ) return;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Deselect();
      aObject.Paint(itsSketchPad.offGraphics);
    }
    itsSketchPad.itsSelectedList.removeAllElements();
    
    if(itsSketchPad.itsConnectingLet!=null) {
      itsSketchPad.itsConnectingLet.ChangeState(false, itsSketchPad.itsConnectingLet.GetConnected());
    }
		
    for (Enumeration e = itsSketchPad.itsSelectedConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      if(aConnection != theConnection){
	aConnection.Deselect();
	//bug 1aConnection.Repaint();
	aConnection.Update(itsSketchPad.GetOffGraphics());
	aConnection.Paint(itsSketchPad.GetOffGraphics());
      }		     
    }
    itsSketchPad.itsSelectedConnections.removeAllElements();
    itsSketchPad.itsSelectedConnections.addElement(theConnection);
    itsSketchPad.CopyTheOffScreen(itsSketchPad.getGraphics());
  }

  public void DeselectInOutlet(){
    ErmesObjInOutlet aInOutlet;
    if(itsSketchPad.itsConnectingLet!=null) 
      itsSketchPad.itsConnectingLet.ChangeState(false, itsSketchPad.itsConnectingLet.connected);
    if(itsSketchPad.itsConnectingLetList.size()!=0){
      for (Enumeration e = itsSketchPad.itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	aInOutlet.ChangeState(false, aInOutlet.connected);
      }
    }
    itsSketchPad.ResetConnect();
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
    for(Enumeration e = itsSketchPad.itsElements.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      for(Enumeration e1 = aObject.GetInletList().elements(); e1.hasMoreElements();) {
	aInlet = (ErmesObjInlet)e1.nextElement();
	aRect = aInlet.Bounds();
	if(aRect.inside(x,y)) {
	  itsSketchPad.itsCurrentInOutlet = aInlet;
	  return true;
	}
      }
      for(Enumeration e2 = aObject.GetOutletList().elements(); e2.hasMoreElements();) {
	aOutlet = (ErmesObjOutlet)e2.nextElement();
	aRect = aOutlet.Bounds();
	if(aRect.inside(x,y)) {
	  itsSketchPad.itsCurrentInOutlet = aOutlet;
	  return true;
	}
      }
    }
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
      if(aRect.inside(x,y)) {
	itsSketchPad.itsCurrentObject = aObject;
	return true;
      }
    }
    return false;
  }
  
  //--------------------------------------------------------
  //	IsMovable
  //	say if is possible to drag a segment
  //--------------------------------------------------------
  public boolean IsMovable(ErmesConnSegment theSegment){
    ErmesConnSegment aSegment;
    boolean trovato = false;
    ErmesConnection aConnection = itsSketchPad.itsSelectedSegment.GetConnection();
    if(!aConnection.GetErrorState()){
      if(theSegment.preferredSize().width<3){//is vertical
	int i;
	for(i=0; ((i<aConnection.GetVSegmentList().size())&&!trovato); i++){
	  aSegment = (ErmesConnSegment) aConnection.GetVSegmentList().elementAt(i);
	  if(aSegment == theSegment) trovato = true; 
	}
	if((i!=1)&&(i!=aConnection.GetVSegmentList().size())) return true;
	else return false;
      }
      return true;	
    }
    else return false;
  }
  
  //--------------------------------------------------------
  //	MoveCircles
  //	move connectionSet circles
  //--------------------------------------------------------
  public void MoveCircles(ErmesConnectionSet theConnectionSet, int theDeltaH, int theDeltaV){
    ErmesConnCircle aCircle;
    for (Enumeration e = theConnectionSet.GetCircles().elements(); e.hasMoreElements() ;){
      aCircle = (ErmesConnCircle) e.nextElement();
      aCircle.MoveBy(theDeltaH,theDeltaV);
    }
  }
  
  //--------------------------------------------------------
  //	MoveDraggedSegment
  //	Move the selected elements
  //--------------------------------------------------------
  public void MoveDraggedSegment(int theDeltaH, int theDeltaV){
    Rectangle aRect = itsSketchPad.itsSelectedSegment.Bounds();
    ErmesConnection aConnection = itsSketchPad.itsSelectedSegment.GetConnection();
    if(IsHorizontal(aRect)){
      //RemoveSegmRgn(true, itsSelectedSegment);
      aConnection.UpdateAllSegments(itsSketchPad.itsSelectedSegment, true, theDeltaV);
      itsSketchPad.itsSelectedSegment.MoveBy(0, theDeltaV);
    }
    else{
      //RemoveSegmRgn(false, itsSelectedSegment);
      aConnection.UpdateAllSegments(itsSketchPad.itsSelectedSegment, false, theDeltaH);
      itsSketchPad.itsSelectedSegment.MoveBy(theDeltaH, 0);
    }
    aConnection.GetConnectionSet().UpdateCircles();
    aConnection.CalcNewPoints();
  }
  
  //--------------------------------------------------------
  //	MoveElements
  //	Move the selected elements
  //--------------------------------------------------------
  public void MoveElements(int theDeltaH, int theDeltaV){
    ErmesObject aObject;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      itsSketchPad.RemoveElementRgn(aObject);
      aObject.MoveBy(theDeltaH, theDeltaV);
    }
  }

 //--------------------------------------------------------
  //	MoveElemListConnections
  //	Move or re-route connections of a list of objects
  //--------------------------------------------------------
  public void MoveElementListConnections(Vector theList, int theDeltaH, int theDeltaV){
    ErmesObject aObject;
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    boolean toUpdating = false;
    Vector UpdatedLinesList = new Vector();
    Vector UpdatingConnSet = new Vector();
    for (Enumeration e = theList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      for (Enumeration e1 = aObject.GetInletList().elements() ; e1.hasMoreElements() ;) {
	aInlet = (ErmesObjInlet) e1.nextElement();
	for (Enumeration e2 = aInlet.GetConnections().elements() ; e2.hasMoreElements() ;) {
	  aConnection = (ErmesConnection) e2.nextElement();
	  if(!UpdatedLinesList.contains(aConnection)){
	    if(aConnection.itsAutorouted){//only for the autorouted lines
	      if(!aConnection.GetErrorState()) {
		itsSketchPad.RemoveConnRgn(aConnection);
		aConnection.GetConnectionSet().RemoveRgn(aConnection);
	      }
	      if((theList.contains(aConnection.GetOutlet().GetOwner()))&&(aConnection.IsToMoving(theDeltaH, theDeltaV))&&
		 (!aConnection.GetErrorState()))
		aConnection.MoveConnection(theDeltaH, theDeltaV);
	      else{
		aConnection.Delete();
		aConnection.PrepareToRouting();
		aConnection.AutoRouting();
		if(!UpdatingConnSet.contains(aConnection.GetConnectionSet()))
		  UpdatingConnSet.addElement(aConnection.GetConnectionSet());
	      }
	      if(!aConnection.GetErrorState()){ 
		itsSketchPad.SaveConnectionRgn(aConnection);
		aConnection.GetConnectionSet().SaveRgn(aConnection);
	      }
	    }
	    else aConnection.PrepareToRouting();
	    UpdatedLinesList.addElement(aConnection);
	  }					
	}
      }
      for (Enumeration e3 = aObject.GetOutletList().elements() ; e3.hasMoreElements() ;) {
	aOutlet = (ErmesObjOutlet) e3.nextElement();
	toUpdating = false;
	for (Enumeration e4 = aOutlet.GetConnections().elements() ; e4.hasMoreElements() ;) {
	  aConnection = (ErmesConnection) e4.nextElement();
	  if(!UpdatedLinesList.contains(aConnection)){
	    if(aConnection.itsAutorouted){
	      if(!aConnection.GetErrorState()) {
		itsSketchPad.RemoveConnRgn(aConnection);
		aConnection.GetConnectionSet().RemoveRgn(aConnection);
	      }
	      if((theList.contains(aConnection.GetInlet().GetOwner()))&&
		 (aConnection.IsToMoving(theDeltaH, theDeltaV))&&(!aConnection.GetErrorState()))
		aConnection.MoveConnection(theDeltaH, theDeltaV);	
	      else{
		aConnection.Delete();
		aConnection.PrepareToRouting();
		aConnection.AutoRouting();
		toUpdating = true;
	      }
	      if(!aConnection.GetErrorState()) {
		itsSketchPad.SaveConnectionRgn(aConnection);
		aConnection.GetConnectionSet().SaveRgn(aConnection);
	      }
	    }
	    else aConnection.PrepareToRouting();
	    UpdatedLinesList.addElement(aConnection);
	  }
	}
	if(toUpdating){
	  if(!UpdatingConnSet.contains(aOutlet.GetConnectionSet()))
	    UpdatingConnSet.addElement(aOutlet.GetConnectionSet());
	}
	else {
	  if(aOutlet.GetConnectionSet().GetCount()!=0)
	    MoveCircles(aOutlet.GetConnectionSet(), theDeltaH, theDeltaV);
	}
      }
    }
    UpdateConnectionSet(UpdatingConnSet);				
  }
  //--------------------------------------------------------
  //	MoveElemConnections
  //	Move or re-route the moved elements connections
  //--------------------------------------------------------
  public void MoveElemConnections(int theDeltaH, int theDeltaV){
    ErmesObject aObject;
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    boolean toUpdating = false;
    Vector UpdatedLinesList = new Vector();
    Vector UpdatingConnSet = new Vector();
    for (Enumeration e = itsSketchPad.itsSelectedList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      for (Enumeration e1 = aObject.GetInletList().elements() ; e1.hasMoreElements() ;) {
	aInlet = (ErmesObjInlet) e1.nextElement();
	for (Enumeration e2 = aInlet.GetConnections().elements() ; e2.hasMoreElements() ;) {
	  aConnection = (ErmesConnection) e2.nextElement();
	  if(!UpdatedLinesList.contains(aConnection)){
	    if(aConnection.itsAutorouted){//only for the autorouted lines
	      if(!aConnection.GetErrorState()) {
		itsSketchPad.RemoveConnRgn(aConnection);
		aConnection.GetConnectionSet().RemoveRgn(aConnection);
		if((itsSketchPad.itsSelectedList.contains(aConnection.GetOutlet().GetOwner()))&&
		   (aConnection.IsToMoving(theDeltaH, theDeltaV)))
		  aConnection.MoveConnection(theDeltaH, theDeltaV);
		else{
		  aConnection.Delete();
		  aConnection.PrepareToRouting();
		  aConnection.AutoRouting();
		  if(!UpdatingConnSet.contains(aConnection.GetConnectionSet()))
		    UpdatingConnSet.addElement(aConnection.GetConnectionSet());
		}
		itsSketchPad.SaveConnectionRgn(aConnection);
		aConnection.GetConnectionSet().SaveRgn(aConnection);
	      }
	      else{
		aConnection.Delete();
		aConnection.PrepareToRouting();
		aConnection.AutoRouting();
		if(!UpdatingConnSet.contains(aConnection.GetConnectionSet()))
		  UpdatingConnSet.addElement(aConnection.GetConnectionSet());
	      }
	    }
	    else aConnection.PrepareToRouting();
	    UpdatedLinesList.addElement(aConnection);
	  }					
	}
      }
      for (Enumeration e3 = aObject.GetOutletList().elements() ; e3.hasMoreElements() ;) {
	aOutlet = (ErmesObjOutlet) e3.nextElement();
	toUpdating = false;
	for (Enumeration e4 = aOutlet.GetConnections().elements() ; e4.hasMoreElements() ;) {
	  aConnection = (ErmesConnection) e4.nextElement();
	  if(!UpdatedLinesList.contains(aConnection)){
	    if(aConnection.itsAutorouted){
	      if(!aConnection.GetErrorState()) {
		itsSketchPad.RemoveConnRgn(aConnection);
		aConnection.GetConnectionSet().RemoveRgn(aConnection);
		if((itsSketchPad.itsSelectedList.contains(aConnection.GetInlet().GetOwner()))&&
		   (aConnection.IsToMoving(theDeltaH, theDeltaV)))
		  aConnection.MoveConnection(theDeltaH, theDeltaV);	
		else{
		  aConnection.Delete();
		  aConnection.PrepareToRouting();
		  aConnection.AutoRouting();
		  toUpdating = true;
		}
		itsSketchPad.SaveConnectionRgn(aConnection);
		aConnection.GetConnectionSet().SaveRgn(aConnection);
	      }
	      else{
		aConnection.Delete();
		aConnection.PrepareToRouting();
		aConnection.AutoRouting();
		toUpdating = true;
	      }
	    }
	    else aConnection.PrepareToRouting();
	    UpdatedLinesList.addElement(aConnection);
	  }
	}
	if(toUpdating){
	  if(!UpdatingConnSet.contains(aOutlet.GetConnectionSet()))
	    UpdatingConnSet.addElement(aOutlet.GetConnectionSet());
	}
	else {
	  if(aOutlet.GetConnectionSet().GetCount()!=0)
	    MoveCircles(aOutlet.GetConnectionSet(), theDeltaH, theDeltaV);
	}
      }
    }
    UpdateConnectionSet(UpdatingConnSet);				
  }
  
  //--------------------------------------------------------
  //	NormalizedRect
  //--------------------------------------------------------
  public Rectangle NormalizedRect(Rectangle theRect) {
    Rectangle aNewRect = new Rectangle();
    
    aNewRect.x = (theRect.width < 0)? theRect.x + theRect.width: theRect.x;
    aNewRect.y = (theRect.height < 0)? theRect.y +theRect.height: theRect.y;
    aNewRect.width = (theRect.width < 0)? - theRect.width: theRect.width;
    aNewRect.height = (theRect.height < 0)? - theRect.height: theRect.height;
    
    return aNewRect;
  }
  
  //--------------------------------------------------------
  //	RemoveSegmRgn
  //	remove the segments region (prec e succ)
  //--------------------------------------------------------
  public void RemoveSegmRgn(boolean theHor, ErmesConnSegment theSegment){
    int i;
    boolean trovato = false;
    ErmesConnSegment aSegment1, aSegment2;
    Rectangle aRect1, aRect2;
    ErmesConnection aConnection = theSegment.GetConnection();
    ErmesConnectionSet aConnectionSet = aConnection.GetConnectionSet();
    aRect1 = theSegment.Bounds();
    if(theHor){
      itsSketchPad.itsHSegmRgn.Remove(aRect1);
      aConnectionSet.GetHSegmentRgn().Remove(aRect1);
      for(i=0; ((i<aConnection.GetHSegmentList().size())&&(trovato==false)); i++){
	aSegment1 = (ErmesConnSegment )(aConnection.GetHSegmentList().elementAt(i));
	if(aSegment1 == theSegment)
	  trovato = true;
      }
      aSegment1 = (ErmesConnSegment)(aConnection.GetVSegmentList().elementAt(i-1));
      aSegment2 = (ErmesConnSegment)(aConnection.GetVSegmentList().elementAt(i));
      aRect1 = aSegment1.Bounds();
      aRect2 = aSegment2.Bounds();
      itsSketchPad.itsVSegmRgn.Remove(aRect1);
      aConnectionSet.GetVSegmentRgn().Remove(aRect1);
      itsSketchPad.itsVSegmRgn.Remove(aRect2);
      aConnectionSet.GetVSegmentRgn().Remove(aRect2);
    }
    else{
      itsSketchPad.itsVSegmRgn.Remove(aRect1);
      aConnectionSet.GetVSegmentRgn().Remove(aRect1);
      for(i=0; ((i<aConnection.GetVSegmentList().size())&&(trovato==false)); i++){
	aSegment1 = (ErmesConnSegment )(aConnection.GetVSegmentList().elementAt(i));
	if(aSegment1 == theSegment)
	  trovato = true;
      }
      aSegment1 = (ErmesConnSegment )(aConnection.GetHSegmentList().elementAt(i-2));
      aSegment2 = (ErmesConnSegment )(aConnection.GetHSegmentList().elementAt(i-1));
      aRect1 = aSegment1.Bounds();
      aRect2 = aSegment2.Bounds();
      itsSketchPad.itsHSegmRgn.Remove(aRect1);
      aConnectionSet.GetHSegmentRgn().Remove(aRect1);
      itsSketchPad.itsHSegmRgn.Remove(aRect2);
      aConnectionSet.GetHSegmentRgn().Remove(aRect1);
    }
  }
  
  //--------------------------------------------------------
  //	ReroutingAllConnections
  //	redo all existing connections autorouting
  //  used only in SetSnapToGrid function
  //--------------------------------------------------------
  public void ReroutingAllConnections(){
    ErmesObject aObject;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(int i=0; i<itsSketchPad.itsConnections.size();i++){
      aConnection = (ErmesConnection) itsSketchPad.itsConnections.elementAt(i);
      if(!aConnection.GetErrorState()) itsSketchPad.RemoveConnRgn(aConnection);
      aConnection.Delete();
      aConnection.PrepareToRouting();
      aConnection.AutoRouting();
      if(!aConnection.GetErrorState()) {
	itsSketchPad.SaveConnectionRgn(aConnection);
	aConnection.GetConnectionSet().SaveRgn(aConnection);
      }
    }
    for(int j=0; j<itsSketchPad.itsElements.size();j++){
      aObject = (ErmesObject) itsSketchPad.itsElements.elementAt(j);
      for(int k=0; k<aObject.GetOutletList().size(); k++){
	aOutlet = (ErmesObjOutlet) aObject.GetOutletList().elementAt(k);
	aOutlet.GetConnectionSet().UpdateCircles();
      }
    }
  }
  
  //--------------------------------------------------------
  //	SaveElementRgn
  //	save all selected elements regions
  //--------------------------------------------------------
  void SaveElementRgn(){
    Rectangle aRect;
    ErmesObject aObject;
    for (Enumeration e = itsSketchPad.itsSelectedList.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aRect = aObject.Bounds();
      aRect.grow(3,6);
      itsSketchPad.itsElementRgn.Add(aRect);
    }		
  }
  
  //??	void SaveSegmRgn(ErmesConnSegment theSegment)
  //--------------------------------------------------------
  //	SearchFtsName
  //  corrispondence between fts names and ermes names (new and old format...)
  //--------------------------------------------------------
  public String SearchFtsName(String theName) {
    if (theName.startsWith("i_")) {	//a new name
      for (int i=0; i<ErmesToolBar.NUM_BUTTONS; i++)
	if (itsSketchPad.newFtsNames[i].equals(theName)) return itsSketchPad.objectNames[i];
    }
    else { //an old name, probably coming from an import...
      for (int i=0; i<ErmesToolBar.NUM_BUTTONS; i++)
	if (itsSketchPad.ftsNames[i].equals(theName)) return itsSketchPad.objectNames[i];
    }		
    ErrorDialog aErr = new ErrorDialog(itsSketchPad.itsSketchWindow, theName + " Not known in ermes");
    aErr.move(50, 50);
    aErr.show();
    
    return ("ermes.ErmesObjComment");
  }
  
  public static String SearchErmesName(String theName) {	//returns the name (new format) 
    for (int i=0; i<ErmesToolBar.NUM_BUTTONS; i++)
      if (ErmesSketchPad.objectNames[i].equals(theName)) return ErmesSketchPad.newFtsNames[i];
    return ("i_object"); //generic object: actually this is an error, we didn't find the name
  }
  //??	public void SetSelectedSegment(ErmesConnSegment theSegment)
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
	itsSketchPad.RemoveElementRgn(aObject);
	aObject.MoveBy(aDeltaH, aDeltaV);
	itsSketchPad.SaveOneElementRgn(aObject);
      }
      ReroutingAllConnections();
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
    ErmesConnSegment aSegment;
    ErmesConnection aConnection;
    for(int i=0; i<itsSketchPad.itsSelectedList.size(); i++){
      aObject = (ErmesObject) itsSketchPad.itsSelectedList.elementAt(i);
      aRect = aObject.Bounds();
      aRegion.Add(aRect);
      for(int j = 0; j<aObject.GetOutletList().size(); j++){
	aOutlet = (ErmesObjOutlet) aObject.GetOutletList().elementAt(j);
	for(int k=0; k<aOutlet.GetConnections().size(); k++){
	  aConnection = (ErmesConnection) aOutlet.GetConnections().elementAt(k);
	  //aggiunge il rettangolo solo se entrambi gli estremi sono in lista selezioanti
	  if(itsSketchPad.itsSelectedList.contains(aConnection.GetInlet().GetOwner())){
	    for(int z=0; z< aConnection.GetHSegmentList().size();z++){
	      aSegment = (ErmesConnSegment) aConnection.GetHSegmentList().elementAt(z);
	      aRect = aSegment.Bounds();
	      aRegion.Add(aRect);
	    }
	    for(int z=0; z< aConnection.GetVSegmentList().size();z++){
	      aSegment = (ErmesConnSegment) aConnection.GetVSegmentList().elementAt(z);
	      aRect = aSegment.Bounds();
	      aRegion.Add(aRect);
	    }
	  }
	}
      }
    }
    return aRegion.MinimumRect();
  }
	
  //--------------------------------------------------------
  //	TraceConnection
  //--------------------------------------------------------
  
  public ErmesConnection TraceConnection(ErmesObjOutlet theSourceOutlet, ErmesObjInlet theDestInlet){
    ErmesConnection aConnection = new ErmesConnection(itsSketchPad, theDestInlet, theSourceOutlet, itsSketchPad.doAutorouting);
    theSourceOutlet.AddConnection(aConnection);
    theDestInlet.AddConnection(aConnection); 
    if((aConnection.itsAutorouted)&&(!aConnection.GetErrorState())){
      itsSketchPad.SaveConnectionRgn(aConnection);
      theSourceOutlet.itsConnectionSet.Add(aConnection);
    }
    itsSketchPad.itsConnections.addElement(aConnection);
    itsSketchPad.ToSave();
    itsSketchPad.repaint();
    return aConnection;
  }
  
  //--------------------------------------------------------
  //	UpdateConnectionSet
  //	update connectionSet circles
  //--------------------------------------------------------
  public void UpdateConnectionSet(Vector theConnSetList){
    ErmesConnectionSet aConnectionSet;
    for (Enumeration e = theConnSetList.elements() ; e.hasMoreElements() ;) {
      aConnectionSet = (ErmesConnectionSet) e.nextElement();
      if(aConnectionSet.GetCount()!=0)
	aConnectionSet.UpdateCircles();
    }
  }
}





