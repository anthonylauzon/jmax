package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import java.lang.*;
import java.io.*;
import java.util.*;

public class ExplodeRemoteData extends FtsRemoteData implements ExplodeDataModel{

  public ExplodeRemoteData()
    {
      super();

      System.err.println( "instanciated " + this.getClass().getName());

      itsEvents = new Vector();
      listeners = new Vector();
    }

  /* the ExplodeDataModel interface... */
  public int length()
  {
    return itsEvents.size();
  }

  public Enumeration getEvents() {
    return itsEvents.elements();
  }

  public void addEvent(ScrEvent theEvent) {
    /*should insert in the real (remote) explode
      itsEvents.addElement(theEvent);
      notifyListeners();*/
  }

  public void removeEvent(ScrEvent theEvent) {
    /* see previous comment
      itsEvents.removeElement(theEvent);
      notifyListeners();*/
  }

 private void notifyListeners() {
    ExplodeDataListener el;
    
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
      el = (ExplodeDataListener) e.nextElement();
      el.dataChanged(null);
    }
  }

  public void addListener(ExplodeDataListener theListener) {
    listeners.addElement(theListener);
  }

  public void removeListener(ExplodeDataListener theListener) {
    listeners.removeElement(theListener);
  }


  public ScrEvent getEventAt(int index) {
    return (ScrEvent) itsEvents.elementAt(index);
  }

  public int indexOfFirstEventAfter(int time) {
    //this implementation requires that the events are ordered by starting time
    ScrEvent temp;
    for (int i = 0; i<length(); i++) {
      temp = getEventAt(i);
      if (temp.getTime() >= time) return i;
    }
    System.err.println("=------------------can't find an event after"+time);
    return 0;//can't find. Did you give a wrong value?
  }

  /* a method inherited from FtsRemoteData */
  public final void call( int key, Object args[])
    {
      switch( key) {
      case 1:
	itsEvents.removeAllElements();
	break;
      case 2:
	itsEvents.addElement( new ScrEvent( ((Integer) args[0]).intValue(), 
					    ((Integer) args[1]).intValue(), 
					    ((Integer) args[2]).intValue(), 
					    ((Integer) args[3]).intValue(), 
					    ((Integer) args[4]).intValue()));
	break;
      default:
	break;
      }
    }

  private Vector itsEvents;
  private Vector listeners;

  /* The MaxData interface */


  /** Get the a name for this data, for UI purposes only */

  public String getName()
  {
    return "explode";
  }
}

