
package ircam.jmax.editors.explode;

import java.util.*;

public class ExplodeSelection implements SelectionHandler {

  public ExplodeSelection(ExplodeDataModel theModel, Adapter theAdapter) {
    itsModel = theModel;
    itsAdapter = theAdapter;
    selected = new Hashtable();
  }

  public void select(Object obj) {
    selected.put(obj, obj);
  }

  public void deSelect(Object obj) {
    selected.remove(obj);
  }

  public boolean isInSelection(Object obj) {

    return selected.containsKey(obj);
  }

  public Enumeration getSelected() {
    return selected.elements();
  }

  public void deselectAll() {
    selected.clear();
  }

  public void selectAll() {
    for (Enumeration e = itsModel.getEvents(); e.hasMoreElements();) {
      select(e.nextElement());
    }
  }

  /**
   * ExplodeSelection make an assumption on the x-y mapping:
   * x is mapped to time, y to pitch. This method, maybe, should not be
   * in the generic SelectionHandler interface.
   */
  public void selectArea(int x, int y, int w, int h) {
    int startTime = itsAdapter.getInvX(x);
    int endTime = itsAdapter.getInvX(x+w);
    int upperPitch = itsAdapter.getInvY(y);
    int lowerPitch = itsAdapter.getInvY(y+h);

    int startIndex = itsModel.indexOfLastEventEndingBefore(startTime)+1;
    int endIndex = itsModel.indexOfFirstEventStartingAfter(endTime)-1;

    ScrEvent aScrEvent;
    for (int i = startIndex; i<= endIndex; i++) {
      aScrEvent = itsModel.getEventAt(i);
      if (aScrEvent.getPitch() >=lowerPitch && aScrEvent.getPitch() <= upperPitch)
	select(itsModel.getEventAt(i));
    }
  }


  // fields
  ExplodeDataModel itsModel;
  Adapter itsAdapter;
  Hashtable selected;
}


