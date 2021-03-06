//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.util.*;
import java.io.*;
import java.awt.*; 
import java.awt.datatransfer.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;
import ircam.jmax.editors.patcher.menus.*;

//
// A class representing a selection in Ermes, with its associated FtsSelection
// object.
// The Class implement all the operations availables on the selection.
//

public class ErmesSelection implements Transferable
{
  static public ErmesSelection patcherSelection =  new ErmesSelection();

  private MaxVector objects = new MaxVector();
  private MaxVector connections = new MaxVector();
  private ErmesSketchPad owner;

  FtsSelection ftsSelection;

  public static FtsSelection getFtsSelection()
  {
    return patcherSelection.ftsSelection;
  }

  public static DataFlavor patcherSelectionFlavor = new DataFlavor(ircam.jmax.fts.FtsSelection.class, "PatcherSelection");
  public static DataFlavor flavors[] = { patcherSelectionFlavor };

  public Object getTransferData(DataFlavor flavor)
  {
    return ftsSelection;
  } 

  public DataFlavor[]  getTransferDataFlavors() 
  {
    return flavors;
  }

  public boolean isDataFlavorSupported(DataFlavor flavor) 
  {
    return flavor.equals(patcherSelectionFlavor);
  } 

  public ErmesSelection() 
  {
    if (flavors == null)
      flavors = new DataFlavor[1];

    flavors[0] = patcherSelectionFlavor;
  
    try
      {
	ftsSelection = new FtsSelection();
      }
    catch(IOException e)
      {
	System.err.println("[ErmesSelection]: Error in FtsSelection creation!");
	e.printStackTrace();
      }
  }

  public void select( DisplayObject object)
  {
    if( object instanceof GraphicConnection)
	   select( (GraphicConnection)object);
    else
	   select( (GraphicObject)object);
  }

  public void select( GraphicObject object) 
  {
    if (object.getSketchPad() != owner)
      setOwner(object.getSketchPad());

    if (! objects.contains( object))
      {
	objects.addElement( object);
	ftsSelection.add( object.getFtsObject());
	object.setSelected(true);
	selectionChanged();
      }
  }

  public void select( GraphicConnection connection) 
  {
    if (connection.getSketchPad() != owner)
      setOwner(connection.getSketchPad());

    if (! connections.contains( connection))
      {
	connections.addElement( connection);
	ftsSelection.add( connection.getFtsConnection());
	connection.setSelected(true);
      }
  }

  public void add( DisplayObject object)
  {
    if( object instanceof GraphicConnection)
      add( (GraphicConnection)object);
    else
      add( (GraphicObject)object);
  }

  public void add( GraphicObject object)
  {
    if (! objects.contains( object))
      {
	objects.addElement( object);
	ftsSelection.add( object.getFtsObject());
      }
  }

  public void add( GraphicConnection connection)
  {
    if (! connections.contains( connection))
      {
	connections.addElement( connection);
	ftsSelection.add( connection.getFtsConnection());
      }
  }

  public void deselect( GraphicObject object) 
  {
    if (objects.contains( object))
      {
	object.setSelected(false);
	objects.removeElement( object);
	ftsSelection.remove( object.getFtsObject());
	selectionChanged();
      }
  }

  public void deselect( GraphicConnection connection) 
  {
    if (connections.contains( connection))
      {
	connection.setSelected(false);
	connections.removeElement( connection);
	ftsSelection.remove( connection.getFtsConnection());
      }
  }

  public boolean isSelected(GraphicObject object)
  {
    return objects.contains( object);
  }


  public boolean isSelected(GraphicConnection connection)
  {
    return connections.contains( connection);
  }

  public boolean isSingleton()
  {
    return objects.size() == 1;
  }

  public GraphicObject getSingleton()
  {
    return (GraphicObject) objects.elementAt(0);
  }

  public Enumeration getSelectedObjects()
  {
    return objects.elements();
  }
  public Enumeration getSelectedConnections()
  {
    return connections.elements();
  }

  public int getSelectedObjectsSize()
  {
    return objects.size();
  }
  
  public void deselectAll() 
  {
    for (Enumeration e = objects.elements() ; e.hasMoreElements(); ) 
      ((GraphicObject) e.nextElement()).setSelected(false);

    for (Enumeration e = connections.elements() ; e.hasMoreElements(); ) 
      ((GraphicConnection) e.nextElement()).setSelected(false);

    objects.removeAllElements();
    connections.removeAllElements();
    ftsSelection.clean();
	selectionChanged();
  }

  public void redraw() 
  {
    GraphicObject object;

    for ( Enumeration e = objects.elements() ; e.hasMoreElements(); ) 
      {
	object = ( GraphicObject) e.nextElement();
	object.redraw();
      }

    GraphicConnection connection;

    for ( Enumeration e = connections.elements() ; e.hasMoreElements(); ) 
      {
	connection = (GraphicConnection) e.nextElement();
	connection.redraw();
      }
  }

  public boolean isEmpty() 
  {
    return objects.isEmpty() && connections.isEmpty();
  }

  public boolean isObjectsEmpty() 
  {
    return objects.isEmpty();
  }

  public boolean hasObjects() 
  {
    return (! objects.isEmpty());
  }

  public boolean hasConnections() 
  {
    return (! connections.isEmpty());
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
      }
    
    this.owner = owner;
  }

  public ErmesSketchPad getOwner()
  {
    return owner;
  }

  public boolean ownedBy(ErmesSketchPad owner)
  {
    return (this.owner == owner);
  }

  // Operations on selection

  public void deleteAll()
  {
    getOwner().setUndo( "Remove", true, false);

    if( connections.size() > 0)
      {
	for ( Enumeration e = connections.elements() ; e.hasMoreElements(); ) 
	  ((GraphicConnection) e.nextElement()).delete();
	
	connections.removeAllElements();
      }
    
    if( objects.size() > 0)
      {
	getOwner().getFtsPatcher().requestDeleteObjects(objects.elements());
	
	for ( Enumeration e = objects.elements() ; e.hasMoreElements(); ) 
	  ((GraphicObject) e.nextElement()).delete();
	
	objects.removeAllElements();
      }
    
    ftsSelection.clean();
    
    if (owner != null)
      owner.getDisplayList().reassignLayers();
  }


  // Generic operation on objects in the selection

  public void apply(ObjectAction action)
  {
    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	GraphicObject object = (GraphicObject) values[i];

	action.processObject(object);
      }
  }

   private void selectionChanged()
	  {
		 if(getOwner() == null) return;
		 ErmesSketchWindow window;
		 if(getOwner().getEditorContainer() instanceof ErmesSketchWindow)
			window = (ErmesSketchWindow)getOwner().getEditorContainer();
		 else
			return;

		 EditMenu m = window.getEditMenu();

		 int numSelected = objects.size();

		 m.copyAction.setEnabled(numSelected > 0);
		 m.cutAction.setEnabled(numSelected > 0);
		 m.duplicateAction.setEnabled(numSelected > 0);
		 m.bringToFrontAction.setEnabled(numSelected > 0);
		 m.sendToBackAction.setEnabled(numSelected > 0);

		 boolean inspectable = false;
		 Enumeration anenum = objects.elements();
		 while(anenum.hasMoreElements())
		 {
			GraphicObject o = (GraphicObject)anenum.nextElement();
			inspectable = inspectable || o.isInspectable();
		 }
		 m.inspectAction.setEnabled(inspectable);

		 m.alignTopAction.setEnabled(numSelected > 1);
		 m.alignLeftAction.setEnabled(numSelected > 1);
		 m.alignBottomAction.setEnabled(numSelected > 1);
		 m.alignRightAction.setEnabled(numSelected > 1);
	  }


  //--------------------------------------------------------
  //	moveAll
  //	Move the selected elements
  //--------------------------------------------------------

  public void moveAllBy( int dx, int dy)
  {
    GraphicObject object;

    Object[] values = objects.getObjectArray();
    int size = objects.size();

    for (int i = 0; i < size; i++)
      {
	object = (GraphicObject) values[i];

	object.redraw();
	object.redrawConnections();
	object.moveBy(dx, dy);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void resizeToMaxWidth()
  {
    getOwner().setUndo( "Resize", false, false);

    int max = 0;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	if (object.getWidth() > max) 
	  max = object.getWidth();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements();) 
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	object.redraw();
	object.redrawConnections();
	object.setWidth(max);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void resizeToMaxHeight()
  {
    getOwner().setUndo( "Resize", false, false);

    int max = 0;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject) e.nextElement();

	if (object.getHeight() > max)
	  max = object.getHeight();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); ) 
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	object.redraw();
	object.redrawConnections();
	object.setHeight(max);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void resizeToMinWidth()
  {
    getOwner().setUndo( "Resize", false, false);

    int min = -1;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	
	if ( (min == -1) || object.getWidth() < min) 
	  min = object.getWidth();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements();) 
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	object.redraw();
	object.redrawConnections();
	object.setWidth( min);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void resizeToMinHeight()
  {
    getOwner().setUndo( "Resize", false, false);

    int min = -1;

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject) e.nextElement();

	if ((min == -1) || object.getHeight() < min)
	  min = object.getHeight();
      }

    for ( Enumeration e = objects.elements(); e.hasMoreElements(); ) 
      {
	GraphicObject object = (GraphicObject) e.nextElement();
	object.redraw();
	object.redrawConnections();
	object.setHeight(min);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void alignTop()
  {
    getOwner().setUndo( "Align", false, false);

    int value = minY();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

	object.redraw();
	object.redrawConnections();
	object.moveBy( 0, value - object.getY());
	object.redraw();
	object.redrawConnections();
      }
  }

  public void alignBottom()
  {
    getOwner().setUndo( "Align", false, false);

    int value = maxY();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

	object.redraw();
	object.redrawConnections();
	object.moveBy( 0, value - (object.getY()+object.getHeight()));
	object.redraw();
	object.redrawConnections();
      }
  }

  public void alignLeft()
  {
    getOwner().setUndo( "Align", false, false);

    int value = minX();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

	object.redraw();
	object.redrawConnections();
	object.moveBy( value - object.getX(), 0);
	object.redraw();
	object.redrawConnections();
      }
  }

  public void alignRight()
  {
    getOwner().setUndo( "Align", false, false);

    int value = maxX();

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

	object.redraw();
	object.redrawConnections();
	object.moveBy( value - (object.getX() + object.getWidth()), 0);
	object.redraw();
	object.redrawConnections();
      }
  }
 public void bringToFront()
  {
    if(hasObjects())
      owner.getDisplayList().objectsToFront(objects.getObjectArray(), objects.size());
  }

  public void sendToBack()
  {
    if(hasObjects())
      owner.getDisplayList().objectsToBack(objects.getObjectArray(), objects.size());
  }

  // Queries about selection geometry

  int minY()
  {
    int min = Integer.MAX_VALUE;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

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
	GraphicObject object = (GraphicObject)e.nextElement();

	if (min >= object.getX())
	  min = object.getX();
      }

    return min;
  }
  
  int maxY()
  {
    int max = 0;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject object = (GraphicObject)e.nextElement();

	if (max < object.getY() + object.getHeight()) 
	  max = object.getY() + object.getHeight();
      }

    return max;
  }

  int maxX()
  {
    int max = 0;

    for( Enumeration e = objects.elements(); e.hasMoreElements(); )
      {
	GraphicObject	object = (GraphicObject)e.nextElement();
	if (max < object.getX() + object.getWidth()) 
	  max = object.getX() + object.getWidth();
      }

    return max;
  }

  // The bounds of the selection as a rectangle
  // Still miss the connections (but should be ok like this)

  Rectangle bounds = new Rectangle();

  Rectangle getBounds()
  {
    if (hasObjects())
      {
	((GraphicObject) objects.elementAt(0)).getBounds(bounds);

	for( int i = 1; i < objects.size(); i++)
	  ((GraphicObject) objects.elementAt(i)).rectangleUnion(bounds);

	return bounds;
      }
    else
      return null;
  }
}











