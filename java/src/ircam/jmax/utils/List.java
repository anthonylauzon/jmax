//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.utils;

import java.util.*;

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




