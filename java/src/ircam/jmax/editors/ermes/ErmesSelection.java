
package ircam.jmax.editors.ermes;

import java.util.*;
import ircam.jmax.fts.*;

/**
 * A class representing a selection in Ermes, with its associated FtsSelection
 * object.
 * Some implementation choices:
 * 1) the class does not derives from Vector (the simplest solution) because
 * of the insertElementAt() problem: public this function is infact "final".
 * 2) This class is then a wrapper (redefinition) of a set of methods of Vector
 */
public class ErmesSelection {
  public Vector itsObjects = new Vector();
  public Vector itsConnections = new Vector();
  //WARN!!+ FtsSelection itsFtsSelection = new FtsSelection();

  public ErmesSelection(FtsObject theFtsObject) {
    //WARN!!+itsFtsSelection = theFtsSelection;
  }

  public ErmesSelection(/*WARN!! to be removed*/) {
    //itsFtsSelection = theSelection; 
  }

  public void addObject(Object theObject) {
    itsObjects.addElement(theObject);
    //WARN!!+itsFtsSelection.addObject(((ErmesObject)theObject).itsFtsObject);
  }

  public void addConnection(Object theConnection) {
    itsConnections.addElement(theConnection);
    //WARN!!+itsFtsSelection.addConnection(((ErmesConnection)theConnection).itsFtsConnection);
  }

  public void removeObject(Object theObject) {
    itsObjects.removeElement(theObject);
    //WARN!!+itsFtsSelection.removeObject(((ErmesObject)theObject).itsFtsObject);
  }

  public void removeConnection(Object theConnection) {
    itsConnections.removeElement(theConnection);
    //WARN!!+itsFtsSelection.removeConnection(((ErmesConnection)theConnection).itsFtsConnection);
  }

  public void removeAllElements() {
    itsObjects.removeAllElements();
    itsConnections.removeAllElements();
    //WARN!!+itsFtsSelection.clean();
  }

}


