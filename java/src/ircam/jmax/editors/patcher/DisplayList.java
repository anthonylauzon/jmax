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

/** This class represent the display list of the patcher editor.
 *  It keep the data base of graphic objects, handle the paiting,
 *  and handle all the geometric requests, like which object this point
 *  belong to ?
 * It also handle the selection.
 */


public class DisplayList
{
  ErmesSketchPad sketch;

  final static int NO_DRAG        = 0;
  final static int DRAG_RECTANGLE = 1;
  final static int DRAG_LINE      = 2;

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

  void addObject(ErmesObject object)
  {
    displayObjects.addElement( object);
  }

  public void removeObject( ErmesObject object)
  {
    displayObjects.removeElement( object);
  }

  ErmesObject getErmesObjectFor(FtsObject obj)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof ErmesObject)
	{
	  ErmesObject object = (ErmesObject) values[i];

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
      if (values[i] instanceof ErmesObject)
	((ErmesObject) values[i]).dispose();
  }

  // Generic operation on objects in the display List

  public void applyToObjects(ObjectAction action)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for (int i = 0; i < size; i++)
      if (values[i] instanceof ErmesObject)
	action.processObject((ErmesObject) values[i]);
  }


  public void objectToFront(ErmesObject object)
  {
    removeObject(object);
    addObject(object);
    sortDisplayList();
  }

  /* Connections */

  final public void addConnection(ErmesConnection connection)
  {
    displayObjects.addElement(connection);
    sortDisplayList();
  }

  void removeConnection( ErmesConnection connection)
  {
    displayObjects.removeElement( connection);
  }

  ErmesConnection getErmesConnectionFor(FtsConnection c)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof ErmesConnection)
	{
	  ErmesConnection connection = (ErmesConnection) values[i];

	  if (connection.itsFtsConnection == c)
	    return connection;
	}

    return null;
  }

  public void redrawConnectionsFor(ErmesObject obj)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for ( int i = 0; i < size; i++)
      if (values[i] instanceof ErmesConnection)
	{
	  ErmesConnection connection = (ErmesConnection) values[i];

	  if ((connection.getSourceObject() == obj) || 
	      (connection.getDestObject() == obj))
	    connection.redraw();
	}
  }

  // Generic operation on objects in the display List

  public void applyToConnections(ConnectionAction action)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    for (int i = 0; i < size; i++)
      if (values[i] instanceof ErmesConnection)
	action.processConnection((ErmesConnection) values[i]);
  }


  // Magic sorting for objects and connections
  // essentially, keep objects are they are
  // and move the connections

  private void sortDisplayList()
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();


    // First, assign progressive layer number to objects

    int layer = 0;
    for (int i = 0; i < size; i++)
      if (values[i] instanceof ErmesObject)
	((ErmesObject) values[i]).setLayer(layer++);

    // Then, sort 

    for (int i = 0; i < size; i++)
      for (int j = 0; j < i; j++)
	if (after(values[j], values[i]))
	  {
	    Object v;

	    v = values[j];
	    values[j] = values[i];
	    values[i] = v;
	  }
  }

  private final boolean after(Object do1, Object do2)
  {
    if (do1 instanceof ErmesObject)
      {
	ErmesObject object1 = (ErmesObject) do1;

	if (do2 instanceof ErmesObject)
	  {
	    ErmesObject object2 = (ErmesObject) do2;

	    return (object1.getLayer() > object2.getLayer());
	  }
	else if (do2 instanceof ErmesConnection)
	  {
	    ErmesConnection connection2 = (ErmesConnection) do2;

	    return (object1.getLayer() > connection2.getSourceObject().getLayer() ||
		    object1.getLayer() > connection2.getDestObject().getLayer());
	  }
	else
	  return false;
      }
    else if (do1 instanceof ErmesConnection)
      {
	ErmesConnection connection1 = (ErmesConnection) do1;

	if (do2 instanceof ErmesObject)
	  {
	    ErmesObject object2 = (ErmesObject) do2;

	    return (object2.getLayer() <= connection1.getSourceObject().getLayer() &&
		    object2.getLayer() <= connection1.getDestObject().getLayer());

	  }
	else if (do2 instanceof ErmesConnection)
	  {
	    ErmesConnection connection2 = (ErmesConnection) do2;

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

  // Find an display object; for the moment quite an hack

  public DisplayObject getDisplayObjectAt(int x, int y)
  {
    Object values[] = displayObjects.getObjectArray();
    int size = displayObjects.size();

    /* Look in the reverse order from outside to inside,
       to be consistent with the graphic look */

    for ( int i = (size - 1); i >= 0; i--)
      {
	if (values[i] instanceof ErmesObject)
	  {
	    ErmesObject object = (ErmesObject) values[i];

	    if (object.getBounds().contains( x,y))
	      {
		SensibilityArea area = object.findSensibilityArea( x, y);

		if (area == null)
		  return object;
		else
		  return area;
	      }
	  }
	else if (values[i] instanceof ErmesConnection)
	  {
	    ErmesConnection  connection = (ErmesConnection) values[i];

	    if (connection.IsNearToPoint( x, y))
	      return connection;
	  }
      }

    return null;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   PAINTING                                            //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  private void paintList( MaxVector theList, Graphics g, Rectangle clip)
  {



  }


  public void paint( Graphics g)
  {
    Object[] values = displayObjects.getObjectArray();
    int size = displayObjects.size();

    // First, paint the background

    Rectangle clip;
    
    clip = g.getClipBounds();

    g.setColor(sketch.getBackground());
    g.fillRect(clip.x, clip.y, clip.width, clip.height);

    // Second, paint the display objects in the right order

    for ( int i = 0; i < size; i++)
      {
	ErmesDrawable object = (ErmesDrawable) values[i];

	if (clip.intersects(object.getBounds()))
	  object.Paint( g);
      }

    // Finally, draw the Drag/effermeral thingies

    switch (dragMode)
      {
      case DRAG_RECTANGLE:
	if (clip.intersects(dragRectangle))
	  {
	    g.setColor( Color.black);
	    g.drawRect( dragRectangle.x, dragRectangle.y, dragRectangle.width, dragRectangle.height);
	  }
	break;

      case DRAG_LINE:
	if (clip.intersects(dragRectangle))
	  {
	    g.setColor( Color.black);
	    g.drawLine( lineStart.x, lineStart.y, lineEnd.x, lineEnd.y);
	  }
	break;

      case NO_DRAG:
	break;
      }

    // Needed in very heavy charge situations ?

    Toolkit.getDefaultToolkit().sync();
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
    public void processConnection(ErmesConnection connection)
      {
	if ( (connection.getSourceObject().isSelected())
	     && (connection.getDestObject().isSelected()))
	  {
	    ErmesSelection.patcherSelection.select( connection);
	  }
      }
  };


  static class ObjectSelecter implements  ObjectAction
  {
    boolean toggle = false;
    Rectangle rect;

    public void processObject(ErmesObject object)
      {
	if (object.getBounds().intersects( rect))
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
    public void processObject(ErmesObject object)
      {
	ErmesSelection.patcherSelection.select(object);
      }
  };

  ConnectionAction allConnectionSelecter =
  new ConnectionAction()
  {
    public void processConnection(ErmesConnection connection)
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
    
    public void processObject(ErmesObject object)
    {
      Rectangle bounds = object.getBounds();

      SwingUtilities.computeUnion(bounds.x, bounds.y, bounds.width, bounds.height, r);      
    }
  }

  BoundCollector boundCollector = new BoundCollector();

  Rectangle getBounds(Rectangle r)
  {
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
    
    public void processObject(ErmesObject object)
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

  public void noDrag()
  {
    dragMode = NO_DRAG;
  }

  public void redrawDragRectangle()
  {
    sketch.repaint(dragRectangle.x, dragRectangle.y, dragRectangle.width + 1, dragRectangle.height + 1);
  }

  public void redrawDragLine()
  {
    sketch.repaint(dragRectangle.x, dragRectangle.y, dragRectangle.width + 1, dragRectangle.height + 1);
  }
}


