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
  Point     lineStart      = new Point();
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
  //                   OBJECT HANDLING                                          //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private MaxVector objects = new MaxVector(); // ==> objects


  //--------------------------------------------------------
  //	AddObject
  //  adding an object of the given class name in the given location
  //--------------------------------------------------------

  void addObject(ErmesObject object)
  {
    objects.addElement( object);
  }
  
  ErmesObject getErmesObjectFor(FtsObject obj)
  {
    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	if (object.getFtsObject() == obj)
	  return object;
      }

    return null;
  }

  //--------------------------------------------------------
  //	ChangeObjectPrecedence
  //--------------------------------------------------------
  // (***fd) is probably messed, because the object should be put
  // at the beginning of the vector, not at the end
  
  final void ChangeObjectPrecedence( ErmesObject object)
  {
    // Need the new paint structure to work well,
    // because of a bad interaction with the dirty list.
    //objects.removeElement( object);
    // objects.insertElementAt( object, 0);
  }
  

  //--------------------------------------------------------
  //	DeleteObject
  //	delete one object routine
  //--------------------------------------------------------

  public void removeObject( ErmesObject object)
  {
    objects.removeElement( object);
  }

  //--------------------------------------------------------
  //	getObjectContaining
  //--------------------------------------------------------

  ErmesObject getObjectContaining( int x, int y)
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	if (object.getBounds().contains( x,y))
	  {
	    return object;
	  }
      }

    return null;
  }
  

  void disposeAllObjects()
  {
    Object[] values = objects.getObjectArray();
    int size       = objects.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	object.dispose();
      }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   CONNECTIONS HANDLING                                          //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  MaxVector connections = new MaxVector();


  ErmesConnection getErmesConnectionFor(FtsConnection c)
  {
    Object[] values = connections.getObjectArray();
    int size = connections.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesConnection connection = (ErmesConnection) values[i];

	if (connection.itsFtsConnection == c)
	  return connection;
      }

    return null;
  }

  public void redrawConnectionsFor(ErmesObject obj)
  {
    Object[] values = connections.getObjectArray();
    int size = connections.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesConnection connection = (ErmesConnection) values[i];

	if ((connection.getSourceObject() == obj) || 
	    (connection.getDestObject() == obj))
	  connection.redraw();
      }
  }

  void SelectConnections()
  {
    ErmesConnection connection;

    for ( Enumeration en = connections.elements() ; en.hasMoreElements(); )
      {
	connection = (ErmesConnection) en.nextElement();

	if ( (connection.getSourceObject().isSelected())
	     && (connection.getDestObject().isSelected()))
	  {
	    ErmesSelection.patcherSelection.select( connection);
	  }
      }
  }

  //--------------------------------------------------------
  //	AddConnection
  //  adding a connection between two objects
  //--------------------------------------------------------

  final public void addConnection(ErmesConnection connection)
  {
    connections.addElement(connection);
  }
	
  //--------------------------------------------------------
  //	removeConnection
  //	delete one connection routine
  //--------------------------------------------------------

  void removeConnection( ErmesConnection connection)
  {
    connections.removeElement( connection);
  }

  //--------------------------------------------------------
  //	getConnectionNear
  //--------------------------------------------------------

  // This function substitute IsInConnection; predicates should
  // not have side effect; by returning directly the connection
  // found we use one member variable less, and the code is more
  // readeble, because all the effects are explicit

  ErmesConnection getConnectionNear( int x, int y)
  {
    ErmesConnection connection;
    Rectangle aRect;

    for( Enumeration e = connections.elements(); e.hasMoreElements();)
      {
	connection = (ErmesConnection)e.nextElement();
	if (connection.IsNearToPoint( x, y))
	  return connection;
      }

    return null;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   SELECTION HANDLING                                       //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  void select(Rectangle rect)
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	if (object.getBounds().intersects( rect) && (! object.isSelected()))
	  {
	    ErmesSelection.patcherSelection.select( object);
	    object.redraw();
	  }
      }	
    
    SelectConnections();
  }

  public void toggleSelect(Rectangle rect)
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	if (object.getBounds().intersects( rect))
	  if (object.isSelected())
	    {
	      ErmesSelection.patcherSelection.deselect( object);
	      object.redraw();
	    }
	  else
	    {
	      ErmesSelection.patcherSelection.select( object);
	      object.redraw();
	    }
      }	
    
    SelectConnections();
  }

  public void selectExactly(Rectangle rect)
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	if (object.getBounds().intersects( rect))
	  {
	    if (! object.isSelected())
	      {
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }
	  }
	else
	  {
	    if (object.isSelected())
	      {
		ErmesSelection.patcherSelection.deselect( object);
		object.redraw();
	      }
	  }
      }	
    
    SelectConnections();
  }

  // This one don't do redraws ... should it ?

  public void selectAll()
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      ErmesSelection.patcherSelection.select((ErmesObject) values[i]);

    for ( Enumeration e = connections.elements() ; e.hasMoreElements(); )
      ErmesSelection.patcherSelection.select( (ErmesConnection) e.nextElement());
  }


  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   PAINTING  DLING                                          //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private void paintList( MaxVector theList, Graphics g, Rectangle clip)
  {
    Object[] values = theList.getObjectArray();
    int size = theList.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesDrawable object = (ErmesDrawable) values[i];

	if (clip.intersects(object.getBounds()))
	  object.Paint( g);
      }
  }


  public void paint( Graphics g)
  {
    // First, paint the background

    Rectangle clip;
    
    clip = g.getClipBounds();

    g.setColor(sketch.getBackground());
    g.fillRect(clip.x, clip.y, clip.width, clip.height);

    if (sketch.isLocked())
      {
	// Locked mode: connections BEFORE objects

	paintList( connections, g, clip); 
	paintList( objects, g, clip);
      }
    else
      {
	// Edit mode: objects BEFORE connections

	paintList( objects, g, clip);
	paintList( connections, g, clip);
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

    // Needed in very heavy charge situations

    Toolkit.getDefaultToolkit().sync();
  }		


  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   GEOMETRICAL COMPUTATIONS                                 //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  /** Compute the rectangle including all the objects, on a rectagle passed as object */

  Rectangle getBounds(Rectangle r)
  {
    r.x = 0;
    r.y = 0;
    r.height = 0;
    r.width = 0;

    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      {
	Rectangle bounds = ((ErmesObject) values[i]).getBounds();

	SwingUtilities.computeUnion(bounds.x, bounds.y, bounds.width, bounds.height, r);
      }

    return r;
  }

  /** Move all the objects to positive coordinates, and put in the rectangle
   *  passed as argument, a dimension that is suitable for a container for the patch.
   *  Return true if a coordinate change has been done, false otherwise.
   */

  void moveAllBy( int dx, int dy)
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();
    
    for ( int i = 0; i < size; i++)
      ((ErmesObject) values[i]).moveBy(dx, dy);
  }

  // Generic operation on objects in the display List

  public void apply(ObjectAction action)
  {
    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	action.processObject(object);
      }
  }

  // Find an display object; for the moment quite an hack, because displayObject
  // is not really implemented as such

  public DisplayObject getDisplayObjectAt(int x, int y)
  {
    ErmesObject object;

    // First, look for the object

    object = getObjectContaining( x, y);

    if (object != null)
      {
	// Found, verify if we got a sensibility area

	SensibilityArea area = object.findSensibilityArea( x, y);

	if (area == null)
	  return object;
	else
	  return area;
      }
    else
      {
	// Return the connection, or null

	return getConnectionNear(x, y);
      }
  }


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


