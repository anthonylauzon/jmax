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

  final void addConnection(ErmesConnection connection)
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

	if (object.getBounds().intersects( rect))
	  ErmesSelection.patcherSelection.select( object);
      }	
    
    SelectConnections();
  }

  // This one don't do redraws ... should it ?

  void selectAll()
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

    // Needed in very heavy charge situations

    Toolkit.getDefaultToolkit().sync();
  }		

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   EFFEMERAL PAINTING  (Annotations)                        //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////


  void showErrorDescriptions()
  {
    Object values[] = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      ((ErmesObject) values[i]).showErrorDescription();
  }
}


