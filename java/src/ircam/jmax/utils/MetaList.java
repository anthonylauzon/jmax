
package ircam.jmax.utils;

/**
 * A special list of objects contained in another list
 */
public class MetaList extends List {
  ListObject refersTo;
  List itsBiggerList;

  public MetaList(List theList) {
    itsBiggerList = theList;
  }

  public void addElement(Object theObject) {
    ListObject aListObject = itsBiggerList.addAndReturn(theObject);
    super.addElement(aListObject);
  }

  public void removeElement(Object theObject) {
    ListObject aListObject = super.search(theObject); //lookup in the little list...
    itsBiggerList.removeListObject(aListObject);
    super.removeListObject(aListObject);
  }

}
