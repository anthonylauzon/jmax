
package ircam.jmax.utils;

import java.util.*;

public class ListIterator implements Enumeration2{
  List itsList;
  ListObject itsPointer;

  public ListIterator (List l) {
    itsList = l;
    itsPointer = l.head;
  }

  public boolean hasMoreElements() {
    return itsPointer != null;
  }

  public Object nextElement() {
    ListObject temp = itsPointer;

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.next;
    return temp.itsObject;
  }

  public Object previousElement() {

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.previous;
    return itsPointer.itsObject;
  }

  public void reset() {
   itsPointer = itsList.head; 
  }
}
