package ircam.jmax.editors.patcher;

import java.awt.*; 
import java.util.*;

import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;

//
// A class representing a selection in Ermes, with its associated FtsSelection
// object.
// The Class implement all the operations availables on the selection.
//


public class ErmesSelection
{
  static public ErmesSelection patcherSelection =  new ErmesSelection();

  private MaxVector objects = new MaxVector();
  private MaxVector connections = new MaxVector();
  private ErmesSketchPad owner;

  public ErmesSelection() 
  {
  }

  public void select(ErmesObject object) 
  {
    if (object.getSketchPad() != owner)
      setOwner(object.getSketchPad());

    if (! objects.contains( object))
      {
	objects.addElement( object);
	Fts.getSelection().addObject( object.getFtsObject());
	object.setSelected(true);
      }

    owner.selectionChanged();
  }

  public void select( ErmesConnection connection) 
  {
    if (connection.itsSketchPad != owner)
      setOwner(connection.itsSketchPad);

    if (! connections.contains( connection))
      {
	connections.addElement( connection);
	Fts.getSelection().addConnection( connection.itsFtsConnection);
	connection.setSelected(true);
      }

    owner.selectionChanged();
  }

  public void deselect( ErmesObject object) 
  {
    if (objects.contains( object))
      {
	object.setSelected(false);
	objects.removeElement( object);
	Fts.getSelection().removeObject( object.getFtsObject());
	owner.selectionChanged();
      }
  }

  public void deselect( ErmesConnection connection) 
  {
    if (connections.contains( connection))
      {
	connection.setSelected(false);
	connections.removeElement( connection);
	Fts.getSelection().removeConnection( connection.itsFtsConnection);
	owner.selectionChanged();
      }
  }

  public boolean isSelected(ErmesObject object)
  {
    return objects.contains( object);
  }


  public boolean isSelected(ErmesConnection connection)
  {
    return connections.contains( connection);
  }

  public boolean isSingleton()
  {
    return objects.size() == 1;
  }

  public ErmesObject getSingleton()
  {
    return (ErmesObject) objects.elementAt(0);
  }
  
  public void deselectAll() 
  {
    ErmesObject object;

    for ( Enumeration e = objects.elements() ; e.hasMoreElements(); ) 
      {
	object = ( ErmesObject) e.nextElement();
	object.setSelected(false);
      }

    ErmesConnection connection;

    for ( Enumeration e = connections.elements() ; e.hasMoreElements(); ) 
      {
	connection = (ErmesConnection) e.nextElement();
	connection.setSelected(false);
      }

    objects.removeAllElements();
    connections.removeAllElements();
    Fts.getSelection().clean();

    owner.selectionChanged();
  }

  public void redraw() 
  {
    ErmesObject object;

    for ( Enumeration e = objects.elements() ; e.hasMoreElements(); ) 
      {
	object = ( ErmesObject) e.nextElement();
	object.redraw();
      }

    ErmesConnection connection;

    for ( Enumeration e = connections.elements() ; e.hasMoreElements(); ) 
      {
	connection = (ErmesConnection) e.nextElement();
	connection.redraw();
      }
  }

  public boolean isEmpty() 
  {
    return objects.isEmpty() && connections.isEmpty();
  }

  public boolean hasObjects() 
  {
    return (! objects.isEmpty());
  }

  public boolean hasObjectsIn(ErmesSketchPad owner)
  {
    return ((this.owner == owner) && (! objects.isEmpty()));
  }

  public void setOwner(ErmesSketchPad owner)
  {
    if ((owner != null) && (this.owner != owner))
      {
	if (! isEmpty())
	  {
	    deselectAll();
	    owner.redraw();
	  }

	owner.selectionChanged();
      }

    this.owner = owner;
  }

  ErmesSketchPad getOwner()
  {
    return owner;
  }

  public boolean ownedBy(ErmesSketchPad owner)
  {
    return (this.owner == owner);
  }

  // Operations on selection

  void deleteAll()
  {
    while (! connections.isEmpty())
      ((ErmesConnection) connections.elementAt( 0)).delete();
    
    while (! objects.isEmpty()) 
      ((ErmesObject) objects.elementAt( 0)).delete();

    owner.selectionChanged();
  }

  //--------------------------------------------------------
  //	moveAll
  //	Move the selected elements
  //--------------------------------------------------------

  void moveAll( int dh, int dv)
  {
    ErmesObject object;

    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	object = (ErmesObject) values[i];

	object.redraw();
	object.moveBy(dh, dv);
	object.redraw();
      }
  }

  void resizeAll( int dh, int dv)
  {
    ErmesObject object;

    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	object = (ErmesObject) values[i];

	object.redraw();
	object.resizeBy( dh, dv);
	object.redraw();
      }
  }


  void resizeToMaxWidth()
  {
    
    int max = 0;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject) e.nextElement();
	if (object.getWidth() > max) 
	  max = object.getWidth();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements();) 
      {
	ErmesObject object = (ErmesObject) e.nextElement();
	object.redraw();
	object.resizeBy( max-object.getWidth(), 0);
	object.redraw();
      }
  }

  void resizeToMaxHeight()
  {
    int max = 0;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject) e.nextElement();

	if (object.getHeight() > max)
	  max = object.getHeight();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); ) 
      {
	ErmesObject object = (ErmesObject) e.nextElement();
	object.redraw();
	object.resizeBy( 0, max - object.getHeight());
	object.redraw();
      }
  }

  void alignTop()
  {
    int value = minY();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	object.redraw();
	object.moveBy( 0, value - object.getY());
	object.redraw();
      }
  }

  void alignBottom()
  {
    int value = maxY();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	object.redraw();
	object.moveBy( 0, value - (object.getY()+object.getHeight()));
	object.redraw();
      }
  }

  void alignLeft()
  {
    int value = minX();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	object.redraw();
	object.moveBy( value - object.getX(), 0);
	object.redraw();
      }
  }

  void alignRight()
  {
    int value = maxX();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	object.redraw();
	object.moveBy( value - (object.getX() + object.getWidth()), 0);
	object.redraw();
      }
  }

  // Add the proper throw

  void setFontName( String theFontName)
  {
    Font font;

    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++) 
      {
	ErmesObject object = (ErmesObject) values[i];

	object.redraw();
	object.setFontName( theFontName);
	object.redraw();
      }
  }

  // Add the proper throw exception

  void setFontSize( int fontSize) 
  {
    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) values[i];

	object.redraw();
	object.setFontSize(fontSize);
	object.redraw();
      }
  }

  void inspect() 
  {
    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for ( int i = 0; i < size; i++) 
      ((ErmesObject) values[i]).inspect();
  }


  boolean openHelpPatches()
  {
    ErmesObject object;
      
    for (Enumeration en = objects.elements(); en.hasMoreElements(); )
      {
	object = (ErmesObject) en.nextElement();
	
	if (! FtsHelpPatchTable.openHelpPatch( object.getFtsObject()))
	  return false;
      }

    return true;
  }

  void showErrorDescriptions()
  {
    for ( Enumeration e = objects.elements(); e.hasMoreElements();)
      ((ErmesObject) e.nextElement()).showErrorDescription();
  }

  // Queries about selection geometry

  int minY()
  {
    int min = Integer.MAX_VALUE;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	if (min >= object.getY())
	  min = object.getY();
      }

    return min;
  }

  int minX()
  {
    int min = Integer.MAX_VALUE;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject object = (ErmesObject)e.nextElement();

	if (min >= object.getX())
	  min = object.getX();
      }

    return min;
  }
  
  int maxY()
  {
    int max = Integer.MAX_VALUE;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject	object = (ErmesObject)e.nextElement();
	if (max < object.getY() + object.getHeight()) 
	  max = object.getY() + object.getHeight();
      }

    return max;
  }

  int maxX()
  {
    int max = Integer.MAX_VALUE;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	ErmesObject	object = (ErmesObject)e.nextElement();
	if (max < object.getX() + object.getWidth()) 
	  max = object.getX() + object.getWidth();
      }

    return max;
  }

  // The bounds of the selection as a rectangle

  Rectangle getBounds()
  {
    int top, left, bottom, right;
    ErmesObject object;

    left = Integer.MAX_VALUE;
    top = Integer.MAX_VALUE;
    bottom = 0;
    right = 0;

    for( int i = 0; i < objects.size(); i++)
      {
	object = (ErmesObject) objects.elementAt( i);
	
	Rectangle aRect = object.getBounds();

	if (aRect.x < left)
	  left = aRect.x;

	if (aRect.y < top)
	  top = aRect.y;

	if ( aRect.y + aRect.height > bottom)
	  bottom = aRect.y + aRect.height;

	if ( aRect.x  + aRect.width > right)
	  right = aRect.x + aRect.width;
      }

    return new Rectangle( left, top, right-left, bottom-top);
  }
}



