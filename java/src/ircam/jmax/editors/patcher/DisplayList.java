//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher;

import java.awt.*; 
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;

import javax.swing.*; 

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

/** This class represent the display list of the patcher editor.
 *  It keep the data base of graphic objects, handle the paiting,
 *  and handle all the geometric requests, like which object this point
 *  belong to ?
 * It also handle the selection.
 */


public class DisplayList 
{
  ErmesSketchPad sketch;

  final static int NO_DRAG             = 0;
  final static int DRAG_RECTANGLE      = 1;
  final static int DRAG_LINE           = 2;

  int dragMode = NO_DRAG;
  Rectangle dragRectangle = new Rectangle();
  Point     lineStart     = new Point();
  Point     lineEnd       = new Point();

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Constructors                                             //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  DisplayList(ErmesSketchPad sketch)
  {
    this.sketch = sketch;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   DISPLAY OBJECT DATABASE HANDLING                         //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private MaxVector displayObjects = new MaxVector(); 

  /* Objects */

  void add(GraphicObject object)
  {
    displayObjects.addElement( object);
  }

  private void addToBeginning(GraphicObject object)
  {
    displayObjects.insertElementAt( object, 0);
  }

  public void remove(GraphicObject object)
  {
    displayObjects.removeElement( object);
  }

  GraphicObject getGraphicObjectFor(FtsObject obj)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicObject)
	{
	  GraphicObject object = (GraphicObject) values[i];

	  if (object.getFtsObject() == obj)
	    return object;
	}

    return null;
  }

  void disposeAllObjects()
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicObject)
	((GraphicObject) values[i]).dispose();
  }

  // Generic operation on objects in the display List

  public void applyToObjects(ObjectAction action)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for (int i = 0; i < size; i++)
      if (values[i] instanceof GraphicObject)
	action.processObject((GraphicObject) values[i]);
  }


  public void objectToFront(GraphicObject object)
  {
    remove(object);
    add(object);
    reassignLayers();
    sortDisplayList();
  }

  public void objectToBack(GraphicObject object)
  {
    remove(object);
    addToBeginning(object);
    reassignLayers();
    sortDisplayList();
  }

  public void objectsToFront(Object[] objects, int size)
  {
    GraphicObject object; 

    sortVector(objects, size);
    for (int i=0; i<size; i++)
      {
	object = (GraphicObject) objects[i];
	remove(object);
	add(object);
      }
    reassignLayers();
    sortDisplayList();
    sketch.repaint();
  }

  public void objectsToBack(Object[] objects, int size)
  {
    GraphicObject object;

    sortVector(objects, size);
    for (int i=size-1; i>=0; i--)
      {
	object = (GraphicObject) objects[i];
	remove(object);
	addToBeginning(object);
      }
    reassignLayers();
    sortDisplayList();
    sketch.repaint();
  }

  /* Connections */

  final public void add(GraphicConnection connection)
  {
    displayObjects.addElement(connection);
  }

  public void remove(GraphicConnection connection)
  {
    displayObjects.removeElement( connection);
  }

  GraphicConnection getGraphicConnectionFor(FtsConnection c)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  if (connection.getFtsConnection() == c)
	    return connection;
	}

    return null;
  }

  public boolean thisConnectionExist(GraphicObject src, int out, GraphicObject dst, int in)
  {
    GraphicConnection connection;
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();
    
    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  connection = (GraphicConnection) values[i];

	  if ((connection.getSourceObject() == src) &&
	      (connection.getOutlet()==out) && 
	      (connection.getDestObject() == dst) &&
	      (connection.getInlet()==in))
	    return true;
	}
    return false;
  }

  public void redrawConnectionsFor(GraphicObject obj)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  if ((connection.getSourceObject() == obj) || 
	      (connection.getDestObject() == obj))
	    connection.redraw();
	}
  }

  public void updateConnectionsFor(GraphicObject obj)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  if ((connection.getSourceObject() == obj) || 
	      (connection.getDestObject() == obj))
	    connection.updateDimensions();
	}
  }

  public void deleteConnectionsForOutlet(GraphicObject obj, int pos)
  {
    MaxVector toDelete = new MaxVector();
    Object[] values;
    int size;

    values = displayObjects.getObjectArray();
    size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  if ((connection.getSourceObject() == obj) &&
	      (connection.getOutlet() == pos))
	    toDelete.addElement(connection);
	}

    values = toDelete.getObjectArray();
    size = toDelete.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  connection.redraw();
	  connection.delete();
	}
  }

  public void deleteConnectionsForInlet(GraphicObject obj, int pos)
  {
    MaxVector toDelete = new MaxVector();
    Object[] values;
    int size;

    values = displayObjects.getObjectArray();
    size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  if ((connection.getDestObject() == obj) &&
	      (connection.getInlet() == pos))
	    toDelete.addElement(connection);
	}

    values = toDelete.getObjectArray();
    size = toDelete.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	{
	  GraphicConnection connection = (GraphicConnection) values[i];

	  connection.redraw();
	  connection.delete();
	}
  }

  // Generic operation on objects in the display List

  public void applyToConnections(ConnectionAction action)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for (int i = 0; i < size; i++)
      if (values[i] instanceof GraphicConnection)
	action.processConnection((GraphicConnection) values[i]);
  }

  
  // Magic sorting for objects and connections
  // essentially, keep objects are they are
  // and move the connections
  // Must be called by hand in the proper case, beacause calling
  // it automatically will call it just too many time in situations
  // like creating a patch and pasting ...

  public void reassignLayers()
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();
    int layer = 0;

    for (int i = 0; i < size; i++)
      if (values[i] instanceof GraphicObject)
	((GraphicObject) values[i]).setLayer(layer++);
  }

  void sortVector(Object[] values, int size){
    for (int i = 0; i < size; i++)
      for (int j = 0; j < i; j++)
	if (isAfter(values[j], values[i]))
	  {
	    Object v;
	    
	    v = values[j];
	    values[j] = values[i];
	    values[i] = v;
	  }
  }

  public void sortDisplayList()
  {
    sortVector(displayObjects.getObjectArray(), displayObjects.size());
  }

  private final boolean isAfter(Object do1, Object do2)
  {
    if (do1 instanceof GraphicObject)
      {
	GraphicObject object1 = (GraphicObject) do1;

	if (do2 instanceof GraphicObject)
	  {
	    GraphicObject object2 = (GraphicObject) do2;

	    return (object1.getLayer() > object2.getLayer());
	  }
	else if (do2 instanceof GraphicConnection)
	  {
	    GraphicConnection connection2 = (GraphicConnection) do2;

	    return (object1.getLayer() >= connection2.getSourceObject().getLayer() ||
		    object1.getLayer() >= connection2.getDestObject().getLayer());
	  }
	else
	  return false;
      }
    else if (do1 instanceof GraphicConnection)
      {
	GraphicConnection connection1 = (GraphicConnection) do1;

	if (do2 instanceof GraphicObject)
	  {
	    GraphicObject object2 = (GraphicObject) do2;

	    return (object2.getLayer() < connection1.getSourceObject().getLayer() &&
		    object2.getLayer() < connection1.getDestObject().getLayer());

	  }
	else if (do2 instanceof GraphicConnection)
	  {
	    GraphicConnection connection2 = (GraphicConnection) do2;

	    return ((connection1.getSourceObject().getLayer() > connection2.getDestObject().getLayer() ||
		     connection1.getSourceObject().getLayer() > connection2.getSourceObject().getLayer()) && 
		    (connection1.getDestObject().getLayer() > connection2.getDestObject().getLayer() ||
		     connection1.getDestObject().getLayer() > connection2.getSourceObject().getLayer()));
	  }
	else
	  return false;
      }
    else
      return false;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   DISPLAY OBJECT BASIC GEOMETRICAL QUERIS                  //
  //                    AND DISPLAY LIST MANIPULATIONS                          //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  public SensibilityArea getSensibilityAreaAt(int x, int y)
  {
    Object values[] = displayObjects.getObjectArray();
    int size = displayObjects.size();
    SensibilityArea candidateArea = null;
    /* Look in the reverse order from outside to inside,
       to be consistent with the graphic look */

    for ( int i = (size - 1); i >= 0; i--)
      {
	DisplayObject object = (DisplayObject) values[i];
	SensibilityArea area = object.getSensibilityAreaAt( x, y);

	if (area != null)
	  {
	    if (area.isTransparent())
	      {
		if (candidateArea == null)
		  candidateArea = area;
		else
		  {
		    if (candidateArea.getCost() > area.getCost())
		      {
			candidateArea.dispose();
			candidateArea = area;
		      }
		  }
	      }
	    else return area;
	  }
      }

    // If nothing returned until now, return the candidate area if any

    return candidateArea;
  }

  //colled during connection and in runMode
  public SensibilityArea getObjectSensibilityAreaAt(int x, int y)
  {
    Object values[] = displayObjects.getObjectArray();
    int size = displayObjects.size();
    SensibilityArea candidateArea = null;
    /* Look in the reverse order from outside to inside,
       to be consistent with the graphic look */

    for ( int i = (size - 1); i >= 0; i--)
      {
	DisplayObject object = (DisplayObject) values[i];
	 
	if(object instanceof GraphicObject){ 
	  
	  SensibilityArea area = object.getSensibilityAreaAt( x, y);

	  if (area != null)
	    {
	      if (area.isTransparent())
		{
		  if (candidateArea == null)
		    candidateArea = area;
		  else
		    {
		      if (candidateArea.getCost() > area.getCost())
			{
			  candidateArea.dispose();
			  candidateArea = area;
			}
		    }
		}
	      else{
		return area;
	      }
	    }
	}
      }
    // If nothing returned until now, return the candidate area if any
    return candidateArea;
  }

  ////////////////////////////////////////////////////////////
  //
  //                  UPDATING 
  //
  ///////////////////////////////////////////////////////////
  MaxVector updateObjects = new MaxVector();

  public void addToUpdate(GraphicObject object){
    updateObjects.addElement(object);
  }
    
  public void resetUpdateObjects(){
    updateObjects.removeAllElements();
  }
    
  public Enumeration getUpdateObjects(){
    return updateObjects.elements();
  }

  void updatePaint(Graphics g){
    Object[] values = updateObjects.getObjectArray();
    int size = updateObjects.size();
    for ( int i = 0; i < size; i++)
      ((DisplayObject) values[i]).updatePaint(g);
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   PAINTING                                            //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  boolean doneOnce = false;
  static Container ic = new Panel();

  public void paint( Graphics g)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    // Very First, if this is the first paint for the window,
    // we do a fake paint with the textRenderer (doesn't work
    // before its first paint) and then we update the dimension
    // of all the editable objects; 
    // and, of course, update for the connections.

    if (! doneOnce)
      {
	doneOnce = true;
 
	for ( int i = 0; i < size; i++)
	  {
	    DisplayObject object = (DisplayObject) values[i];

	    if (object instanceof Editable)
	      {
		((Editable)object).drawContent(g);
		((Editable)object).updateDimensionsNoConnections(); // HACK ? Yes !
	      }

	  }

	for ( int i = 0; i < size; i++)
	  {
	    DisplayObject object = (DisplayObject) values[i];

	    if (object instanceof GraphicConnection)
	      ((GraphicConnection)object).updateDimensions();
	  }
      }

    // First, paint the background

    Rectangle clip  = g.getClipBounds();

    g.setColor(sketch.isLocked() ?
	       Settings.sharedInstance().getLockBackgroundColor() : 
	       Settings.sharedInstance().getEditBackgroundColor());

    g.fillRect(clip.x, clip.y, clip.width, clip.height);

    // Second, paint the display objects in the right order

    for ( int i = 0; i < size; i++)
      {
	DisplayObject object = (DisplayObject) values[i];

	if (object.intersects(clip))
	  object.paint( g);
      }

    // Finally, draw the Drag/effermeral thingies
    // Leave the clipping do the clip work; if the drag thingies are
    // active are probabily always repaint.

    switch (dragMode)
      {
      case DRAG_RECTANGLE:
	g.setColor( Color.black);
	g.drawRect( dragRectangle.x, dragRectangle.y, dragRectangle.width, dragRectangle.height);
	break;

      case DRAG_LINE:
	g.setColor( Color.black);
	g.drawLine( lineStart.x, lineStart.y, lineEnd.x, lineEnd.y);
	break;

      case NO_DRAG:
	break;
      }
  }		

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   SELECTION HANDLING                                       //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  /* The following code use a apply style of programming with static
     objects, instantiated once, to avoid:
     1- depending on the display list data structure, that is going to change
        fast.
     2- Avoid allocating new objects for all the operations, including enumeration
        or object/connection Actions.
	*/

  ConnectionAction connectionSelecter =
  new ConnectionAction()
  {
    public void processConnection(GraphicConnection connection)
      {
	if ((connection.getSourceObject().isSelected()) && (connection.getDestObject().isSelected()))
	  ErmesSelection.patcherSelection.select( connection);
	else
	  ErmesSelection.patcherSelection.deselect( connection);
      }
  };


  static class ObjectSelecter implements  ObjectAction
  {
    boolean toggle = false;
    Rectangle rect;

    public void processObject(GraphicObject object)
      {
	if (object.coreIntersects( rect))
	  {
	    if (object.isSelected())
	      {
		if (toggle)
		  {
		    ErmesSelection.patcherSelection.deselect( object);
		    object.redraw();
		  }
	      }
	    else
	      {
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }
	  }
	else
	  {
	    if (object.isSelected())
	      {
		if (! toggle)
		  {
		    ErmesSelection.patcherSelection.deselect( object);
		    object.redraw();
		  }
	      }
	  }
      }

    void setToggling(boolean v)
    {
      toggle = v;
    }

    void setRectangle(Rectangle rect)
    {
      this.rect = rect;
    }
  }

  ObjectSelecter objectSelecter = new ObjectSelecter();

  public void selectExactly(Rectangle rect)
  {
    objectSelecter.setRectangle(rect);
    objectSelecter.setToggling(false);
    applyToObjects(objectSelecter);
    applyToConnections(connectionSelecter);
  }

  public void toggleSelect(Rectangle rect)
  {
    objectSelecter.setRectangle(rect);
    objectSelecter.setToggling(true);
    applyToObjects(objectSelecter);
    applyToConnections(connectionSelecter);
  }


  ObjectAction allObjectSelecter =
  new ObjectAction()
  {
    public void processObject(GraphicObject object)
      {
	ErmesSelection.patcherSelection.select(object);
      }
  };

  ConnectionAction allConnectionSelecter =
  new ConnectionAction()
  {
    public void processConnection(GraphicConnection connection)
      {
	ErmesSelection.patcherSelection.select(connection);
      }
  };


  // This one don't do redraws ... should it ?

  public void selectAll()
  {
    applyToObjects(allObjectSelecter);
    applyToConnections(allConnectionSelecter);
  }


  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   GEOMETRICAL COMPUTATIONS                                 //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  /** Compute the rectangle including all the objects, on a rectagle passed as object */

  static class BoundCollector implements ObjectAction
  {
    Rectangle r;

    void setRectangle(Rectangle r)
    {
      this.r = r;
    }
    
    public void processObject(GraphicObject object)
    {
      object.rectangleUnion(r);
    }
  }

  BoundCollector boundCollector = new BoundCollector();

  public Rectangle getBounds(Rectangle r)
  {
    // Note that the bounds always include the 0.0 corner
    // this is done on purpose, it is not a bug.

    r.x = 0;
    r.y = 0;
    r.height = 0;
    r.width = 0;

    boundCollector.setRectangle(r);
    applyToObjects(boundCollector);

    return r;
  }

  /** Move all the objects to positive coordinates, and put in the rectangle
   *  passed as argument, a dimension that is suitable for a container for the patch.
   *  Return true if a coordinate change has been done, false otherwise.
   */

  static class ObjectMover implements ObjectAction
  {
    int dx;
    int dy;

    void setDeltaX(int v)
    {
      dx = v;
    }

    void setDeltaY(int v)
    {
      dy = v;
    }
    
    public void processObject(GraphicObject object)
    {
      object.moveBy(dx, dy);
    }
  }

  ObjectMover objectMover = new ObjectMover();

  void moveAllBy( int dx, int dy)
  {
    objectMover.setDeltaX(dx);
    objectMover.setDeltaY(dy);
    applyToObjects(objectMover);
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   TRANSIENT OBJECT DISPLAY                                 //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  // The Display List also handle a additional DRAG rectangle,
  // on top of the other stuff; the idea is to use it for select and resize
  // applications; 

  /** Set the display list drag rectagle; accept a non normalized rectangle,
    i.e. one with negative width or height, and flip it correctly
    */

  public void setDragRectangle(int x, int y, int width, int height)
  {
    if (height < 0)
      {
	height = (-1) * height;
	y = y - height;
      }

    if (width < 0)
      {
	width = (-1) * width;
	x = x - width;
      }
 
    dragRectangle.x = x;
    dragRectangle.y = y;
    dragRectangle.width = width;
    dragRectangle.height = height;
  }

  public void setDragLine(int x, int y, int x2, int y2)
  {
    // Set the drag rectangle to the rectangle corresponding to
    // the line, in order to be able to use intersect with the
    // clipping area during repaint.

    if (x < x2)
      {
	dragRectangle.x = x;
	dragRectangle.width = x2 - x;
      }
    else
      {
	dragRectangle.x = x2;
	dragRectangle.width = x - x2;
      }

    if (y < y2)
      {
	dragRectangle.y = y;
	dragRectangle.height = y2 - y;
      }
    else
      {
	dragRectangle.y = y2;
	dragRectangle.height = y - y2;
      }

    // Store the two ending points 

    lineStart.x = x;
    lineStart.y = y;

    lineEnd.x = x2;
    lineEnd.y = y2;
  }


  public Rectangle getDragRectangle()
  {
    return dragRectangle;
  }

  public void dragRectangle()
  {
    dragMode = DRAG_RECTANGLE;
  }

  public void dragLine()
  {
    dragMode = DRAG_LINE;
  }

  public boolean isDragLine()
  {
    return (dragMode == DRAG_LINE);
  }

  public void noDrag()
  {
    dragMode = NO_DRAG;
  }

  public void redrawDragRectangle()
  {
    /* If the rectangle is 'big', we issue four repaint,
       corresponding to the four side of the rectangle
       */

    
    if ((dragRectangle.width > 50) && (dragRectangle.height > 50))
      {
	sketch.repaint(dragRectangle.x, dragRectangle.y, 1, dragRectangle.height + 1);
	sketch.repaint(dragRectangle.x, dragRectangle.y, dragRectangle.width + 1, 1);
	sketch.repaint(dragRectangle.x, dragRectangle.y + dragRectangle.height, dragRectangle.width + 1, 1);
	sketch.repaint(dragRectangle.x + dragRectangle.width, dragRectangle.y, 1, dragRectangle.height + 1);
      }
    else
      sketch.repaint(dragRectangle.x, dragRectangle.y, dragRectangle.width + 1, dragRectangle.height + 1);
  }

  public void redrawDragLine()
  {
    sketch.repaint(dragRectangle.x, dragRectangle.y, dragRectangle.width + 1, dragRectangle.height + 1);
  }
}








