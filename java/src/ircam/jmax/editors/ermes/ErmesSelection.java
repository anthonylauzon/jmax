
package ircam.jmax.editors.ermes;

import java.util.*;
import java.awt.datatransfer.*;

import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

//
// A class representing a selection in Ermes, with its associated FtsSelection
// object.
// Some implementation choices:
// 1) the class does not derives from MaxVector (the simplest solution) because
// of the insertElementAt() problem: public this function is infact "final".
// 2) This class is then a wrapper (redefinition) of a set of methods of Vector
//
public class ErmesSelection implements Transferable
{
  public MaxVector itsObjects = new MaxVector();
  public MaxVector itsConnections = new MaxVector();
  public ErmesSketchPad itsOwner;

  public static DataFlavor patcherSelection = new DataFlavor(ircam.jmax.fts.FtsSelection.class, "PatcherSelection");
  public static DataFlavor flavors[];

  public Object getTransferData(DataFlavor flavor)
  {
    return Fts.getSelection();
  } 

  public DataFlavor[]  getTransferDataFlavors() 
  {
    return flavors;
  }

  public boolean isDataFlavorSupported(DataFlavor flavor) 
  {
    return flavor.equals(patcherSelection);
  } 

  public ErmesSelection() 
  {
    if (flavors == null) flavors = new DataFlavor[1];
    flavors[0] = patcherSelection;
  }

  public void select(ErmesObject object) 
  {
    if (object.itsSketchPad != itsOwner)
      setOwner(object.itsSketchPad);

    if (! itsObjects.contains( object))
      {
	itsObjects.addElement( object);
	Fts.getSelection().addObject( object.itsFtsObject);
	object.Select();
      }
  }

  public void select( ErmesConnection connection) 
  {
    if (connection.itsSketchPad != itsOwner)
      setOwner(connection.itsSketchPad);

    if (! itsConnections.contains( connection))
      {
	itsConnections.addElement( connection);
	Fts.getSelection().addConnection( connection.itsFtsConnection);
	connection.Select();
      }
  }

  public void deselect( ErmesObject object) 
  {
    if (itsObjects.contains( object))
      {
	object.Deselect();
	itsObjects.removeElement( object);
	Fts.getSelection().removeObject( object.itsFtsObject);
      }
  }

  public void deselect( ErmesConnection connection) 
  {
    if (itsConnections.contains( connection))
      {
	connection.Deselect();
	itsConnections.removeElement( connection);
	Fts.getSelection().removeConnection( connection.itsFtsConnection);
      }
  }

  public boolean isSelected(ErmesObject object)
  {
    return itsObjects.contains( object);
  }


  public boolean isSelected(ErmesConnection connection)
  {
    return itsConnections.contains( connection);
  }


  public void deselectAll() 
  {
    if (itsObjects.size() != 0) 
      itsOwner.GetSketchWindow().DeselectionUpdateMenu();

    ErmesObject aObject;

    for ( Enumeration e = itsObjects.elements() ; e.hasMoreElements(); ) 
      {
	aObject = ( ErmesObject) e.nextElement();
	aObject.Deselect();
      }

    ErmesConnection aConnection;

    for ( Enumeration e = itsConnections.elements() ; e.hasMoreElements(); ) 
      {
	aConnection = (ErmesConnection) e.nextElement();
	aConnection.Deselect();
      }

    itsObjects.removeAllElements();
    itsConnections.removeAllElements();
    Fts.getSelection().clean();
  }

  public boolean isEmpty() 
  {
    return itsObjects.isEmpty() && itsConnections.isEmpty();
  }


  public void setOwner(ErmesSketchPad owner)
  {
    if ((itsOwner != null) && (owner != itsOwner))
      {
	deselectAll();
	itsOwner.repaint();
      }

    itsOwner = owner;
  }

  public  ErmesSketchPad getOwner()
  {
    return itsOwner;
  }
}

