package ircam.jmax.utils;

public class ListObject {
  public Object itsObject;
  public ListObject next;
  public ListObject previous;
  
  public ListObject(Object theObject) {
    itsObject = theObject;
    next = null;
    previous = null;
  }
}
