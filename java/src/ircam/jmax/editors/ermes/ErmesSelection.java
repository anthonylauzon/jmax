
package ircam.jmax.editors.frobber;

import java.util.*;

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
public class ErmesSelection {
  public MaxVector itsObjects = new MaxVector();
  public MaxVector itsConnections = new MaxVector();

  public ErmesSelection() 
  {
  }

  public void addObject( Object theObject) 
  {
    itsObjects.addElement( theObject);
    Fts.getSelection().addObject( ((ErmesObject)theObject).itsFtsObject);
  }

  public void addConnection( Object theConnection) 
  {
    itsConnections.addElement( theConnection);
    Fts.getSelection().addConnection( ((ErmesConnection)theConnection).itsFtsConnection);
  }

  public void removeObject( Object theObject) 
  {
    itsObjects.removeElement( theObject);
    Fts.getSelection().removeObject( ((ErmesObject)theObject).itsFtsObject);
  }

  public void removeConnection( Object theConnection) 
  {
    itsConnections.removeElement( theConnection);
    Fts.getSelection().removeConnection( ((ErmesConnection)theConnection).itsFtsConnection);
  }

  public void removeAllElements() 
  {
    itsObjects.removeAllElements();
    itsConnections.removeAllElements();
    Fts.getSelection().clean();
  }

  public boolean isEmpty() 
  {
    return itsObjects.isEmpty() && itsConnections.isEmpty();
  }
}

