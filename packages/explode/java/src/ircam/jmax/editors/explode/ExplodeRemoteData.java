package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

import java.lang.*;
import java.io.*;
import java.util.*;


/**
 * A concrete implementation of the ExplodeDataModel.
 * It handles the datas coming from a remote explode in FTS
 */
// implementation notes: the structure used is an ordered array of ScrEvents.
// A better structure for look-up (ex. TDTree) would require the use of a
// mean of communicate the identity of an event between the editor and the explode not
// based on indexes.
public class ExplodeRemoteData extends FtsRemoteUndoableData implements ExplodeDataModel
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
      
    listeners = new MaxVector();
  }

  /* Private methods */

  final int getIndexAfter(int time)
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
    
    if (events[min].getTime() > time)
      return min;
    else return max;
  
  }

  public final int getFirstEventAt(int time)
  {
    if (events_fill_p == 0) 
	return EMPTY_COLLECTION;
    
    else if (events[events_fill_p - 1].getTime()< time)  
	return NO_SUCH_EVENT;
    
    
    int min = 0;
    int max = events_fill_p - 1;
    int med = 0;

    while (max > min+1)
      {
	med = (max + min) / 2;

	if (events[med].getTime() >= time)
	  max = med;
	else 
	  min = med;
      }
    
    if (events[min].getTime() == time) return min;
    else if (events[max].getTime() == time) return max;
    else return NO_SUCH_EVENT;
  }

  /**
   * utility function to make the event vector bigger
   */
  protected final void reallocateEvents()
  {
    int new_size;
    ScrEvent temp_events[];

    new_size = (events_size * 3)/2;
    temp_events = new ScrEvent[new_size];

    for (int i = 0; i < events_size; i++)
      temp_events[i] = events[i];

    events = temp_events;
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
   * an utility class to implement the intersection with a range */
  class Intersection implements Enumeration {
    Intersection(int start, int end)
    {
      endTime = end;
      startTime = start;
      
      index = getIndexAfter(endTime) ;
      if (index == NO_SUCH_EVENT) index = events_fill_p -1;

    } 

    public boolean hasMoreElements()
    {
      nextObject = findNext();

      return nextObject != null;
    }

    public Object nextElement()
    {

      return nextObject;
    }

    private Object findNext()
    {
      ScrEvent e;

      while(index >=0)
	{
	  e = events[index--];
	  if (e.getTime()+e.getDuration() >= startTime)
	    {
	      return e;
	    }
	}
      return null;
    }


    //--- Fields
    int endTime;
    int startTime;
    int index;
    Object nextObject = null;
  }

  /**
   * returns an enumeration of all the events that intersect a given range */

  public Enumeration intersectionSearch(int start, int end)
  {
    return new Intersection(start,end);
  }

  class Inclusion implements Enumeration {
    
    Inclusion(int start, int end)
    {
      this.endTime = end;
      index = getIndexAfter(start);
      endIndex = getIndexAfter(end);
    } 

    public boolean hasMoreElements()
    {
      nextObject = findNext();
      return nextObject != null;
    }

    public Object nextElement()
    {
      return nextObject;
    }

    private Object findNext()
    {
      ScrEvent e;
      
      while(index < endIndex)
	{
	  e = events[index++];
	  if (e.getTime()+e.getDuration() <=endTime)
	    {
	      return e;
	    }
	}
      return null;
    }



    //--- Fields
    int endTime;
    int index;
    int endIndex;
    Object nextObject = null;
  }

  /**
   * Returns an enumeration of the events completely included
   * in the given range. Note that this function is MUCH more
   * efficient that the intersectionSearch, and should be used
   * when possible */
  public Enumeration inclusionSearch(int start, int end)
  {
    return new Inclusion(start, end);
  }

  /**
   * adds an event in the data base
   */

  public void addEvent(ScrEvent event)
  {
    int index;

    event.setDataModel(this);

    index = getIndexAfter(event.getTime());

    if (index == EMPTY_COLLECTION)
      index = 0;
    else if (index == NO_SUCH_EVENT)
      index = events_fill_p;

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
    
    if (isInGroup())     
      postEdit(new UndoableAdd(event));
    
  }

  private int linearSearch(ScrEvent e)
  {
    for (int i = 0; i < events_fill_p; i++)
      {
	if (getEventAt(i) == e) return i;
      }
    return NO_SUCH_EVENT;
  }

  private int binarySearch(ScrEvent e)
  {
    int index = getFirstEventAt(e.getTime());
    if (index == NO_SUCH_EVENT || index == EMPTY_COLLECTION)
      {
	return index;
      }
    
    for(;getEventAt(index) != e; index++)
      {
	if (index >= events_fill_p) return NO_SUCH_EVENT;
      }
    
    return index;
  }
  
  /**
   * remove an event from the data base
   */

  public void removeEvent(ScrEvent event)
  {
    int removeIndex;

    removeIndex = binarySearch(event);
    if (removeIndex == NO_SUCH_EVENT || removeIndex == EMPTY_COLLECTION)
      return;
    
    if (isInGroup())
      postEdit(new UndoableDelete(event));
    
    deleteRoomAt(removeIndex);
    
    // Send the remove command to fts
    
    Object args[] = new Object[1];
    args[0] = new Integer(removeIndex);
    remoteCall(REMOTE_REMOVE, args);
	    
    notifyObjectDeleted(event);
    
  }


  /**
   *  Signal FTS that an object is changed 
   *  but that the time is still the same
   * Note that FTS will decide on its own
   * if an object should be moved or not;
   * i.e. the remote call for this function
   * and the next is the same.
   */

  public void changeEvent(ScrEvent event)
  {
    int index;

    index = binarySearch(event);

    if (index == NO_SUCH_EVENT || index == EMPTY_COLLECTION)
      return;

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

  }


  /**
   *  Signal FTS that an object is to be moved, and move it
   * in the data base; moving means changing the "index"
   * value, i.e. its time.
   */

  public void moveEvent(ScrEvent event, int newTime)
  {
    int index = binarySearch(event);     // Find the event
    int newIndex = getIndexAfter(newTime); //Find where to place it
    
    if (newIndex == NO_SUCH_EVENT) newIndex = events_fill_p-1;
    else if (event.getTime() < newTime) newIndex -=1;
    

    if (index == NO_SUCH_EVENT)
      {
	System.err.println("no such event error");
	for (int i = 0; i < length(); i++)
	  {
	    System.err.println("#"+i+" t "+getEventAt(i).getTime()+" p "+getEventAt(i).getPitch());
	  }
	return; //should rise an exception instead
      }

    if (index == EMPTY_COLLECTION)
      index = 0;
    event.setTime(newTime);
    
    // inform FTS
    
    Object args[] = new Object[6];
    
    args[0] = new Integer(index);
    args[1] = new Integer(event.getTime());
    args[2] = new Integer(event.getPitch());
    args[3] = new Integer(event.getVelocity());
    args[4] = new Integer(event.getDuration());
    args[5] = new Integer(event.getChannel());
    
    remoteCall(REMOTE_CHANGE, args);
    
    // rearranges the events in the DB 
    if (index < newIndex) 
      {
	for (int i = index; i < newIndex; i++)
	  {
	    events[i] = events[i+1];
	  }
      }
    else
      {
	for (int i = index; i > newIndex; i--)
	  {
	    events[i] = events[i-1];
	  }
	events[newIndex] = event;
      }
    
    events[newIndex] = event;
    notifyObjectChanged(event);
    
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
  

  /* a method inherited from FtsRemoteData */

  public void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key) {
    case REMOTE_LOAD_START:
      {
	// Clean the explode before a new loading (Needed ?? )
	
	for (int i = 0; i < events_fill_p; i++)
	  events[i] = null;
	
	events_fill_p = 0;
      }
    break;

    case REMOTE_LOAD_APPEND:
      {
	// add events at the end; used during loading
	
	if (events_fill_p >= events_size)
	  reallocateEvents();
	
	events[events_fill_p++] = new ScrEvent(this,
					       stream.getNextIntArgument(),
					       stream.getNextIntArgument(),
					       stream.getNextIntArgument(),
					       stream.getNextIntArgument(),
					       stream.getNextIntArgument());
      }
    break;
    
    case REMOTE_LOAD_END:
      {
	}
    break;
    
    case REMOTE_CLEAN:
      {
	// Clean the explode, after a record message
	
	beginUpdate();
	for (int i = 0; i < events_fill_p; i++)
	  {
	    postEdit(new UndoableDelete(events[i]));
	    notifyObjectDeleted( events[i]);
	    events[i] = null;
	  }
	
	events_fill_p = 0;
	endUpdate();
      }
    break;
    
    case REMOTE_APPEND:
      {
	// add a note, after an FTS computation.
	int index;
	
	beginUpdate();
	if (events_fill_p >= events_size)
	  reallocateEvents();
	
	index = events_fill_p++;
	events[index] = new ScrEvent(this,
					  stream.getNextIntArgument(),
					  stream.getNextIntArgument(),
					  stream.getNextIntArgument(),
					  stream.getNextIntArgument(),
					  stream.getNextIntArgument());
	
	postEdit(new UndoableAdd(events[index]));
	endUpdate();
	notifyObjectAdded(events[index]);
      }
    break;
    
    default:
      break;
    }
  }

  
  private void sort(ScrEvent[] elements)
  {
    sort(elements, 0, length()-1);
  }

  /**
   * bubble sort, betweem two index of the given vector.
   * This function is usefull when we know in advance that
   * a subset of the vector is already ordered (es. in moveEvent) */
  private void sort(ScrEvent[] elements, int first, int last)
  {
    int i;
    boolean doneSomething = true;
    ScrEvent temp;
    
    while(doneSomething)
      {
	doneSomething = false;
	
	for (i = first; i < last-1; i++, last -= 1)
	  {
	    if(elements[i] == null || elements[i].getTime() >   
	       elements[i+1].getTime() )
	      {
		temp = elements[i];
		elements[i] = elements[i+1];
		elements[i+1] = temp;
		doneSomething = true;
	      }
	  }
      }
  }

  //----- Fields
  /** Key for remote call add */
  
  static final int REMOTE_LOAD_START  = 1;
  static final int REMOTE_LOAD_APPEND = 2;
  static final int REMOTE_LOAD_END    = 3;
  static final int REMOTE_CLEAN  = 4;
  static final int REMOTE_APPEND = 5;
  static final int REMOTE_ADD    = 6;
  static final int REMOTE_REMOVE = 7;
  static final int REMOTE_CHANGE = 8;

  static final int EMPTY_COLLECTION = -1;
  static final int NO_SUCH_EVENT = -2;

  int events_size   = 256;	// 
  int events_fill_p  = 0;	// next available position
  ScrEvent events[] = new ScrEvent[256];
  private MaxVector listeners;
  private MaxVector tempVector = new MaxVector();
}


