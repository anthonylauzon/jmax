
package ircam.jmax.utils;

import java.util.*;

public class MetaListIterator extends ListIterator{

  public MetaListIterator(List l) {
    super(l);
  }

  public Object nextElement() {
    ListObject temp = itsPointer;

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.next;
    return ((ListObject)(temp.itsObject)).itsObject;
  }

  public Object previousElement() {

    if (itsPointer == null) return null;//what's going on here?
    itsPointer = itsPointer.previous;
    return ((ListObject)itsPointer.itsObject).itsObject;
  }

}
