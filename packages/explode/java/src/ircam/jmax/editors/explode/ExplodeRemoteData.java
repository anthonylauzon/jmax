package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import java.lang.*;
import java.io.*;
import java.util.*;

public class ExplodeRemoteData extends FtsRemoteData implements ExplodeDataModel
{
  /** Key for remote call add */

  static final int REMOTE_ADD   = 1;
  static final int REMOTE_REMOVE = 2;

  /* Events are stored in an array; the array is larger than
     needed to allow insertions, and reallocated by need.
     Search is done with a binary search, while insertion/deleting
     need moving objects around; then, we assume that lookup
     is more frequent than editing (true, every repain imply
     a look up) */

  int events_size   = 256;	// 
  int events_fill_p  = 0;	// next available position
  ScrEvent events[] = new ScrEvent[256];
  private Vector listeners;

  public ExplodeRemoteData()
  {
    super();
      
    listeners = new Vector();
  }

  /* Private methods */

  private final int getIndexAfter(int time)
  {
    if (events_fill_p == 0)
      return 0;
    else if (events[events_fill_p - 1].getTime() <= time)
      return events_fill_p;
    else
      {
	int min = 0;
	int max = events_fill_p - 1;
	
	while (max > min + 1)
	  {
	    int med = (max + min) / 2;
	    
	    if (events[med].getTime() <= time) 
	      min = med;
	    else 
	      max = med;
	  }

	return max;
      }
  }


  private final void reallocateEvents()
  {
    int new_size;
    ScrEvent new_events[];
   
    new_size = (events_size * 3)/2;
    new_events = new ScrEvent[new_size];

    for (int i = 0; i < events_size; i++)
      new_events[i] = events[i];

    events = new_events;
    events_size = new_size;
  }

  private final void makeRoomAt(int index)
  {
    if (events_fill_p >= events_size) 
      reallocateEvents();

    for (int i = events_fill_p; i> index; i--) 
      events[i] = events[i-1];
    
    events_fill_p++;
  }

  private final void deleteRoomAt(int index)
  {
    for (int i = index;  i < events_fill_p; i++)
      events[i] = events[i + 1];

    events_fill_p--;
  }
			 

  /* the ExplodeDataModel interface... */

  public int length()
  {
    return events_fill_p;
  }

  private class ExplodeEnumeration implements Enumeration
  {
    int p;

    public boolean hasMoreElements()
    {
      return p < events_fill_p;
    }

    public Object nextElement()
    {
      return events[p++];
    }
  }

  public Enumeration getEvents()
  {
    return new ExplodeEnumeration();
  }

  public void addEvent(ScrEvent event)
  {
    int index;

    index = getIndexAfter(event.getTime());

    makeRoomAt(index);
    events[index] = event;
    
    Object args[] = new Object[6];
    args[0] = new Integer(index);
    args[1] = new Integer(event.getTime());
    args[2] = new Integer(event.getPitch());
    args[3] = new Integer(event.getVelocity());
    args[4] = new Integer(event.getDuration());
    args[5] = new Integer(event.getSomething());

    remoteCall(REMOTE_ADD, args);

    notifyListeners();
  }

  public void removeEvent(ScrEvent event)
  {
    int removeIndex;

    // Linear search: a binary search would not
    // work because we can have multiple events
    // with the same key (time).

    for (removeIndex = 0 ; removeIndex < events_fill_p; removeIndex++)
      {
	if (events[removeIndex] == event)
	  {
	    deleteRoomAt(removeIndex);

	    // Send the remove command to fts

	    Object args[] = new Object[1];
	    args[0] = new Integer(removeIndex);
	    remoteCall(REMOTE_REMOVE, args);

	    notifyListeners();
	  }
      }
  }

 private void notifyListeners()
  {
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
    return events[index];
  }

  public int indexOfFirstEventStartingAfter(int time) {
    int index;

    index = getIndexAfter(time);

    if (index == events_fill_p)
      return -1;
    else
      return index;
  }

  public int indexOfLastEventEndingBefore(int time) {
    int index;

    index = getIndexAfter(time);
    
    for (;index>=0;index--) {
      if (events[index].getTime()+events[index].getDuration() < time)
	break;
    }
    return index;

  }

  private class ExplodeLivingEnumeration implements Enumeration {
    
    int index;
    int time;
    
    private ExplodeLivingEnumeration(int theTime) {
      time = theTime;
      index = indexOfLastEventEndingBefore(time);
      while(events[index].getTime()+events[index].getDuration() < time ) 
	index++;
    }

    public boolean hasMoreElements() {
      return events[index].getTime() <= time;
    }
    
    public Object nextElement() {
      return events[index++];
    }
    
  }
  
  public Enumeration eventsLivingAt(int time) {
    return new ExplodeLivingEnumeration(time);
  }

  /* a method inherited from FtsRemoteData */

  public final void call( int key, Object args[])
    {
      switch( key) {
      case 1:
	// Make the gc happy

	for (int i = 0; i < events_fill_p; i++)
	  events[i] = null;

	events_fill_p = 0;
	break;
      case 2:
	// FTS always send the events in order (???)

	if (events_fill_p >= events_size)
	  reallocateEvents();
	events[events_fill_p++] = new ScrEvent( ((Integer) args[0]).intValue(), 
						((Integer) args[1]).intValue(), 
						((Integer) args[2]).intValue(), 
						((Integer) args[3]).intValue(), 
						((Integer) args[4]).intValue());
	
	break;
      default:
	break;
      }
    }



  /* The MaxData interface */


  /** Get the a name for this data, for UI purposes only */

  public String getName()
  {
    return "explode";
  }
}

