package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import java.lang.*;
import java.io.*;
import java.util.*;


/**
 * A concrete implementation of the ExplodeDataModel.
 * It handles the datas coming from a remote explode in FTS
 */
public class ExplodeRemoteData extends FtsRemoteData implements ExplodeDataModel
{
  /* Events are stored in an array; the array is larger than
     needed to allow insertions, and reallocated by need.
     Search is done with a binary search, while insertion/deleting
     need moving objects around; then, we assume that lookup
     is more frequent than editing (true, every repaint imply
     a look up) */


  /**
   * constructor.
   */
  public ExplodeRemoteData()
  {
    super();
      
    listeners = new Vector();
  }

  /* Private methods */

  private final int getIndexAfter(int time)
  {
    if (events_fill_p == 0) 
      return EMPTY_COLLECTION;
    
    else if (events[events_fill_p - 1].getTime()<= time)  
      return NO_SUCH_EVENT;
    
    
    int min = 0;
    int max = events_fill_p - 1;
    
    while (max > min+1)
      {
	int med = (max + min) / 2;
	
	if (events[med].getTime() <= time) 
	  min = med;
	else 
	  max = med;
      }
    
    return max;
  
  }


  /**
   * utility function to make the event vector bigger
   */
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

  /**
   * utility function to create a new place at the given index
   */
  private final void makeRoomAt(int index)
  {
    if (events_fill_p >= events_size) 
      reallocateEvents();

    for (int i = events_fill_p; i> index; i--) 
      events[i] = events[i-1];
    
    events_fill_p++;
  }

  /**
   * deletes the place at the given index
   */
  private final void deleteRoomAt(int index)
  {
    for (int i = index;  i < events_fill_p; i++)
      events[i] = events[i + 1];

    events_fill_p--;
  }
			 

  /* the ExplodeDataModel interface... */
  /**
   * how many events in the data base?
   */
  public int length()
  {
    return events_fill_p;
  }

  /**
   * an utility class to efficiently implement the getEvents()
   * call
   */
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

  /**
   * returns an enumeration of all the events
   */
  public Enumeration getEvents()
  {
    return new ExplodeEnumeration();
  }


  /**
   * adds an event in the data base
   */

  public void addEvent(ScrEvent event)
  {
    int index;

    index = getIndexAfter(event.getTime());

    if (index == EMPTY_COLLECTION) index = 0;
    else if (index == NO_SUCH_EVENT) index = events_fill_p;
    makeRoomAt(index);
    events[index] = event;
    
    Object args[] = new Object[6];
    args[0] = new Integer(index);
    args[1] = new Integer(event.getTime());
    args[2] = new Integer(event.getPitch());
    args[3] = new Integer(event.getVelocity());
    args[4] = new Integer(event.getDuration());
    args[5] = new Integer(event.getChannel());

    remoteCall(REMOTE_ADD, args);

    notifyObjectAdded(event);
  }


  /**
   * remove an event from the data base
   */

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

	    notifyObjectDeleted(event);

	    return;
	  }
      }
  }


  /**
   *  Signal FTS that an object is changed 
   *  but that the time is still the same
   */

  public void changeEvent(ScrEvent event)
  {
    int index;

    for (index = 0 ; index < events_fill_p; index++)
      {
	if (events[index] == event)
	  {
	    // Send the change command to fts

	    Object args[] = new Object[6];

	    args[0] = new Integer(index);
	    args[1] = new Integer(event.getTime());
	    args[2] = new Integer(event.getPitch());
	    args[3] = new Integer(event.getVelocity());
	    args[4] = new Integer(event.getDuration());
	    args[5] = new Integer(event.getChannel());

	    remoteCall(REMOTE_CHANGE, args);

	    notifyObjectChanged(event);

	    return;
	  }
      }
  }

  /**
   *  Signal FTS that an object is moved, and move it
   * in the data base; moving means changing the "index"
   * value, i.e. its time.
   */

  public void moveEvent(ScrEvent event)
  {
  }



  /**
   * utility to notify the data base change to all the listeners
   */

  private void notifyObjectAdded(Object spec)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((ExplodeDataListener) e.nextElement()).objectAdded(spec);
  }

  private void notifyObjectDeleted(Object spec)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((ExplodeDataListener) e.nextElement()).objectDeleted(spec);
  }

  private void notifyObjectChanged(Object spec)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
      ((ExplodeDataListener) e.nextElement()).objectChanged(spec);
  }


  /**
   * require to be notified when data change
   */
  public void addListener(ExplodeDataListener theListener) 
  {
    listeners.addElement(theListener);
  }
  

  /**
   * remove the listener
   */
  public void removeListener(ExplodeDataListener theListener) 
  {
    listeners.removeElement(theListener);
  }


  /**
   * access the event at the given index
   */
  public ScrEvent getEventAt(int index) 
  {
    return events[index];
  }
  

  /**
   * access the first event whose starting time is 
   * after a given time
   */
  public int indexOfFirstEventStartingAfter(int time) 
  {
    int index;
    
    index = getIndexAfter(time);
    
    if (index == events_fill_p)
      return -1;
    else
      return index;
  }


  /**
   * access the first event whose ENDING time is 
   * after a given time
   */
  public int indexOfFirstEventEndingAfter(int time)
  {

    return indexOfLastEventEndingBefore(time) + 1;
  }

  /**
   * access the last event whose ENDING time is 
   * before a given time.
   */
  public int indexOfLastEventEndingBefore(int time) 
  {

    int index = getIndexAfter(time);
    
    // ENDING times are not ordered...
    while(index >= 0 && events[index].getTime()+ events[index].getDuration() > time)
      index --;

    return index;    
  }

  /**
   * access the last event whose starting time is 
   * before a given time
   */
  public int indexOfLastEventStartingBefore(int time) 
  {
    int index;

    index = getIndexAfter(time);
    if (index == NO_SUCH_EVENT) return events_fill_p-1;

    for (;index>=0;index--) 
      {
	if (events[index].getTime() < time)
	  break;
      }

    return index;    
  }


  /**
   * utility class to efficiently implement the eventsLivingAt call
   */
  private class ExplodeLivingEnumeration implements Enumeration {
    
    int index;
    int time;
    
    private ExplodeLivingEnumeration(int theTime) 
    {
      time = theTime;
      index = indexOfLastEventEndingBefore(time);
      while(events[index].getTime()+events[index].getDuration() < time ) 
	index++;
    }

    public boolean hasMoreElements() 
    {
      return events[index].getTime() <= time;
    }
    
    public Object nextElement() 
    {
      return events[index++];
    }
    
  }
  
  /**
   * the enumeration of all the events active in a given moment
   */
  public Enumeration eventsLivingAt(int time) 
  {  
    return new ExplodeLivingEnumeration(time);
  }

  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsMessage msg)
    {
      switch( key) {
      case REMOTE_CLEAN:
	// Make the gc happy

	for (int i = 0; i < events_fill_p; i++)
	  events[i] = null;

	events_fill_p = 0;
	break;
      case REMOTE_APPEND:
	// FTS always send the events in order (???)

	if (events_fill_p >= events_size)
	  reallocateEvents();
	events[events_fill_p++] = new ScrEvent( ((Integer) msg.getArgument(2)).intValue(), 
						((Integer) msg.getArgument(3)).intValue(), 
						((Integer) msg.getArgument(4)).intValue(), 
						((Integer) msg.getArgument(5)).intValue(), 
						((Integer) msg.getArgument(6)).intValue());
	break;
      default:
	break;
      }
    }


  /* The MaxData interface */


  /** 
   * Get the a name for this data, for UI purposes only 
   */

  public String getName()
  {
    return "explode";
  }

  //----- Fields
  /** Key for remote call add */

  static final int REMOTE_CLEAN  = 1;
  static final int REMOTE_APPEND = 2;
  static final int REMOTE_ADD    = 1;
  static final int REMOTE_REMOVE = 2;
  static final int REMOTE_CHANGE = 3;

  static final int EMPTY_COLLECTION = -1;
  static final int NO_SUCH_EVENT = -2;

  int events_size   = 256;	// 
  int events_fill_p  = 0;	// next available position
  ScrEvent events[] = new ScrEvent[256];
  private Vector listeners;
}


