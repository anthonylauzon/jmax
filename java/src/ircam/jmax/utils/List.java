
package ircam.jmax.utils;
import java.util.*;
import ircam.jmax.editors.ermes.ErmesArea; //debug only. Please remove soon

public class List {
  ListObject head;
  ListObject tail;
  ListIterator itsIterator;

  public List() {
    itsIterator = new ListIterator(this);
  }


  public void addElement(Object theObject) {
    ListObject aListObject = new ListObject(theObject);
    addListObject(aListObject);
  }
  
  ListObject addAndReturn(Object theObject) {
    ListObject aListObject = new ListObject(theObject);
    addListObject(aListObject);
    return aListObject;
  }

  void addListObject(ListObject aListObject) {
    
    if (head == null) {
      aListObject.next = null;
      aListObject.previous = null;
      head = aListObject;
      tail = aListObject;
    }
    else  {
      head.previous = aListObject;
      aListObject.next = head;
      aListObject.previous = null;
      head = aListObject;
    }
  }

  public void removeElement(Object theObject) {
    ListObject aListObject = search(theObject);
    removeListObject(aListObject);
  }

  void removeListObject(ListObject aListObject) {
    ListObject before = null;
    ListObject after = null;
    
    if (aListObject != null) {
      before = aListObject.previous;
      after = aListObject.next;
    }
    else {
      return;
    }
    
    if (after == null) {
      if (before == null) {
	head = null;
	tail = null;
      }
      else {
	before.next = null;
	tail = before;
      }
    }
    else if (before == null) {
      head = after;
      after.previous = null;
    }
    else {
      before.next = after;
      after.previous = before;
    }
  }
  
  public boolean isInList(Object theObject) {
    return (search(theObject) != null);
  }

  public int size() {
    int i=0;
    for(Enumeration li = elements(); li.hasMoreElements();i++){
      li.nextElement();
    }
    return i;
  }

  public Object next() {
    return head.itsObject;
  }

  public Object firstInserted() {
    return tail.itsObject;
  }

  public ListIterator elements() {
    itsIterator.reset();
    return itsIterator;
  }

  /*  public boolean hasMoreElements() {
      return head != null;
  }*/

  public boolean isEmpty() {
    return head == null;
  }

  public void removeAllElements() {
    head = null;
    System.gc(); //not to leave too much garbage around
  }

  ListObject search(Object theObject) {
    ListObject temp = head;

    while (temp != null) {
      if (temp.itsObject.equals(theObject)) break;
      else temp = temp.next;
    }
    return temp;
  }
  
  void printTheList() {
    //bla bla
  }
}




