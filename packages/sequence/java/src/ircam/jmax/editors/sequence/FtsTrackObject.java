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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;

/**
 * A general-purpose TrackDataModel, this class
 * offers an implementation based on a variable-length 
 * array.
 * @see ircam.jmax.editors.sequence.track.TrackDataModel*/

public class FtsTrackObject extends FtsObject implements TrackDataModel, ClipableData, ClipboardOwner
{

    /**
     * Create an AbstractSequence and initialize the type vector
     * with the given type.
     */
    public FtsTrackObject(Fts fts, String name, ValueInfo info)
    {
	super(fts, null, null, "seqtrack", "seqtrack");
	
	listeners = new MaxVector();

	this.name = name;
	//sequenceData = sequence;
	
	/* prepare the flavors for the clipboard */
	if (flavors == null)
	    flavors = new DataFlavor[1];
	flavors[0] = sequenceFlavor;

	infos.addElement(info);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

    /**
     * Fts callback: add a TrackEvent(first arg) in a track (second arg). 
     * 
     */
    public void addEvent(int nArgs , FtsAtom args[])
    {
	//String trackName = args[0].getString();
	TrackEvent evt = (TrackEvent)(args[0].getObject());
	//TrackDataModel model = getTrackByName(trackName).getTrackDataModel();

	// starts an undoable transition
	//((UndoableData)model).beginUpdate();
    
	//model.addEvent(evt);
	addEvent(evt);
    
	// ends the undoable transition
	//((UndoableData)model).endUpdate();
  }


  public void requestEventCreation(float time, String type, int nArgs, Object args[])
  {
    sendArgs[0].setFloat(time); 
    sendArgs[1].setString(type);
      
    for(int i=0; i<nArgs; i++)
      sendArgs[2+i].setValue(args[i]);

    sendMessage(FtsObject.systemInlet, "event_new", 2+nArgs, sendArgs);
  }
  
    
    /**
     * Create an AbstractSequence with an empty type vector 
     */
    /*public FtsTrackObject(Fts fts)
      {
      super(fts, null, null, "seqtrack", "seqtrack");
      listeners = new MaxVector();
      
      // prepare the flavors for the clipboard 
      if (flavors == null)
      flavors = new DataFlavor[1];
      flavors[0] = sequenceFlavor;
      
      }*/

  
    
    /**
     * Create an AbstractSequence and initialize the type vector
     * with the given enumeration of types.
     */
    /*public AbstractSequence( Enumeration types)
      {
      super();
      listeners = new MaxVector();
	
      // prepare the flavors for the clipboard 
      if (flavors == null)
      flavors = new DataFlavor[1];
      flavors[0] = sequenceFlavor;
      
      if (types == null)
      return;

      while(types.hasMoreElements())
      {
      infos.addElement(types.nextElement());
      }
      
      }*/

    /**
     * how many events in the database?
     */
    public int length()
    {
	return events_fill_p;
    }
    
    
    /**
     * returns an enumeration of all the events
     */
    public Enumeration getEvents()
    {
	return new SequenceEnumeration();
    }
    
    
    /**
     * returns a given event
     */
    public TrackEvent getEventAt(int index)
    {
	return events[index];
    }
    
    /**
     * return the index of the given event, if it exists, or the error constants
     * NO_SUCH_EVENT, EMPTY_COLLECTION */
    public int indexOf(TrackEvent event)
    {
	int index = getFirstEventAt(event.getTime());
	if (index == NO_SUCH_EVENT || index == EMPTY_COLLECTION)
	    {
		return index;
	    }
	
	for(;getEventAt(index) != event; index++)
	    {
		if (index >= events_fill_p) return NO_SUCH_EVENT;
	    }
	
	return index;
    }
    
    public Enumeration intersectionSearch(double start, double end)
    {
	return new Intersection(start,end);
    }
    
    public Enumeration inclusionSearch(double start, double end)
    {
	return new Inclusion(start, end);
    }
    
    public int getFirstEventAt(double time)
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
     * adds an event in the database
     */
    public void addEvent(TrackEvent event)
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
		
	notifyObjectAdded(event, index);
	
	/*if (isInGroup())     
	  {
	  postEdit(new UndoableAdd(event));
	  }*/
    }

    /**
     * generic change of an event in the database.
     * Call this function to signal the parameters changing of the event, except
     * the initial time and the duration parameters. Use moveEvent and resizeEvent for that.
     */
    public void changeEvent(TrackEvent event)
    {
	int index;
	
	index = indexOf(event);
	
	if (index == NO_SUCH_EVENT || index == EMPTY_COLLECTION)
	    return;

	//????????????????
	
	notifyObjectChanged(event);
    }
    
    /**
     * move an event in the database
     */
    public void moveEvent(TrackEvent event, double newTime)
    {
	int index = indexOf(event);     // Find the event
	int newIndex = getIndexAfter(newTime); //Find where to place it
	
	if (newIndex == NO_SUCH_EVENT) newIndex = events_fill_p-1;
	else if (event.getTime() <= newTime) newIndex -=1;
	
	
	if (index == NO_SUCH_EVENT)
	    {
		System.err.println("no such event error");
		return; //this is a severe error: should rise an exception instead
	    }
	
	if (index == EMPTY_COLLECTION)
	    index = 0;
	event.setTime(newTime);
	
	// inform FTS (to be implemented)
	
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
	notifyObjectMoved(event, index, newIndex);
    }
    
    
    /**
     * deletes an event from the database
     */
    public void removeEvent(TrackEvent event)
    {
	int removeIndex;
	
	removeIndex = indexOf(event);
	removeEventAt(removeIndex);

	sendArgs[0].setObject(event);
	//((FtsSequenceObject)sequenceData).sendMessage(FtsObject.systemInlet, "event_remove", 1, sendArgs);
	sendMessage(FtsObject.systemInlet, "event_remove", 1, sendArgs);
    }
    

    public void removeAllEvents()
    {
	// help a little the garbage collector
	for (int i = 0; i<events_fill_p; i++)
	    events[i] = null;

	events_fill_p = 0;
	infos = new MaxVector();
    }

    private void removeEventAt(int removeIndex)
    {
	TrackEvent event = getEventAt(removeIndex);
	if (removeIndex == NO_SUCH_EVENT || removeIndex == EMPTY_COLLECTION)
	    return;
	
	/*if (isInGroup())
	  postEdit(new UndoableDelete(event));*/
	
	deleteRoomAt(removeIndex);
	
	// Send the remove command to fts (to be implemented)

	notifyObjectDeleted(event, removeIndex);
    }



    /**
     * utility to notify the data base change to all the listeners
     */
    
    private void notifyObjectAdded(Object spec, int index)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectAdded(spec, index);
    }
    
    private void notifyObjectDeleted(Object spec, int oldIndex)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectDeleted(spec, oldIndex);
    }
    
    private void notifyObjectChanged(Object spec)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectChanged(spec);
    }
    
    private void notifyObjectMoved(Object spec, int oldIndex, int newIndex)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectMoved(spec, oldIndex, newIndex);
    }
    
    /**
     * requires to be notified when the database changes
     */
    public void addListener(TrackDataListener theListener)
    {
	listeners.addElement(theListener);
    }
    
    /**
     * removes the listener
     */
    public void removeListener(TrackDataListener theListener)
    {
	listeners.removeElement(theListener);
    }


    // ClipableData functionalities
    public void cut()
    {
	
	if (SequenceSelection.getCurrent().getModel() != this) return;
	
	copy();
	
	// ... and remove 
	
	//beginUpdate(); //cut is undoable
	
	for (Enumeration e = SequenceSelection.getCurrent().getSelected(); e.hasMoreElements();)
	    {
		removeEvent((TrackEvent) e.nextElement());
	    }
	
	//endUpdate();
    }
    
    public void copy()
    {
	if (SequenceSelection.getCurrent().getModel() != this) 
	    return;
	SequenceSelection.getCurrent().prepareACopy();
	MaxApplication.systemClipboard.setContents(SequenceSelection.getCurrent(), this);
    }  
    
    
    public void paste()
    {
	if (SequenceSelection.getCurrent().getModel() != this) 
	    return;
	
	Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);
	Enumeration objectsToPaste = null;
	
	if (clipboardContent != null && clipboardContent.isDataFlavorSupported(SequenceDataFlavor.getInstance()))
	    {
		try {
		    objectsToPaste = (Enumeration) clipboardContent.getTransferData(SequenceDataFlavor.getInstance());
		} catch (UnsupportedFlavorException ufe)
		    {
			// this should never happen, but...
			System.err.println("Clipboard error in paste: content does not support "+SequenceDataFlavor.getInstance().getHumanPresentableName());
		    } 
		catch (IOException ioe)
		    {
			System.err.println("Clipboard error in paste: content is no more an "+SequenceDataFlavor.getInstance().getHumanPresentableName());
		    }
		
	    }
	
	if (objectsToPaste != null)
	    {
		TrackEvent event;
		TrackEvent event1;
		
		//beginUpdate();  //the paste is undoable
		SequenceSelection.getCurrent().deselectAll();
		
		try {
		    while (objectsToPaste.hasMoreElements())
			{
			    event = (TrackEvent) objectsToPaste.nextElement();
			    event1 = event.duplicate();
			    addEvent(event1);
			    SequenceSelection.getCurrent().select(event1);
			}
		    
		}
		catch (Exception e) {}
		
		//endUpdate();
		
	    }
    }
    
    /** ClipboardOwner interface */
    
    public void lostOwnership(Clipboard clipboard,
			      Transferable contents)
    {
	//SequenceSelection.discardTheCopy();
    }
    
    //-------------------------------------------------------
    
    /* Private methods */
    
    final int getIndexAfter(/*int*/double time)
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
	
	if (time < events[min].getTime())
	    return min;
	else if (time > events[max].getTime())
	    return max+1;
	else return max;
	
    }
    
    
    /**
     * utility function to make the event vector bigger
     */
    protected final void reallocateEvents()
    {
	int new_size;
	TrackEvent temp_events[];
	
	new_size = (events_size * 3)/2;
	temp_events = new TrackEvent[new_size];
	
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
    
    /**
     * an utility class to efficiently implement the getEvents()
     * call
     */
    
    private class SequenceEnumeration implements Enumeration
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


    /*  FtsRemoteData. To be implemented. Look at ExplodeRemoteData for a 
     * complete example. */
    
    public void call( int key, FtsStream stream)
	throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
    {
	return;

    }

    /**
     * an utility class to implement the intersection with a range */
    class Intersection implements Enumeration {
	Intersection(double start, double end)
	{
	    endTime = end;
	    startTime = start;
	    
	    index = 0;
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
	    if (length() == 0) return null;
	    TrackEvent e;
	    
	    while (index < length() && events[index].getTime() <= endTime)
		{
		    e = events[index++];
		    if (e.getTime() >= startTime ||
			e.getTime()+((Integer)e.getProperty("duration")).intValue() >= startTime )
			{
			    return e;
			}
		}
	    return null;
	}
	
	
	//--- Intersection Fields
	double endTime;
	double startTime;
	int index;
	Object nextObject = null;
    }    


    /**
     * AN utility class to return an enumeration of all the events
     * in a temporal range */
    class Inclusion implements Enumeration {
	
	Inclusion(double start, double end)
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
	    TrackEvent e;
	    
	    while(index < endIndex)
		{
		    e = events[index++];
		    if (e.getTime()+((Integer)e.getProperty("duration")).intValue() <=endTime)
		    {
			    return e;
			}
		}
	    return null;
	}
	
	
	
	//--- Inclusion internal class fields
	double endTime;
	int index;
	int endIndex;
	Object nextObject = null;
    }


    /**
     * Move all the events of the given model in this MultiSequence, and
     * remove them from the original one. After this operation, the old
     * model is empty, but its content can be get back using the unmergeModel() call.
     */
    /*public void mergeModel(TrackDataModel model)
      {
      for (Enumeration e = model.getEvents(); e.hasMoreElements();)
      {
      
      addEvent((TrackEvent) e.nextElement());
      }
      
      for (Enumeration e = model.getTypes(); e.hasMoreElements();)
      infos.addElement(e.nextElement());
      
      model.removeAllEvents();
      }*/


    /**
     * Fill the given TrackDataModel with all the event of type info
     * contained in this MultiSequence. This function could be used to
     * reverse the effect of a mergeModel call. 
     * The resulting models can have a different content if the MultiSequence
     * object have been edited in the meanwhile.
     */
    /*public void unmergeModel(TrackDataModel model, ValueInfo info)
      {
      if (!infos.contains(info))
      return; 
      // no elements of type info are present.
      
      infos.removeElement(info);
      
      TrackEvent temp;
      for (Enumeration e = getEvents(); e.hasMoreElements();)
      {
      temp = (TrackEvent) e.nextElement();
      if (temp.getValue().getValueInfo().equals(info))
      {
      model.addEvent(temp);
      }
      }

      }*/

    /**
     * Returns an enumeration of all the ValueInfo merged in this model
     */
    public Enumeration getTypes()
    {
	return infos.elements();
    }

    public ValueInfo getTypeAt(int i)
    {
	return (ValueInfo) infos.elementAt(i);
    }

    /**
     * Returns true if this models currently contains events of the given type */
    public boolean containsType(ValueInfo info)
    {
	return infos.contains(info);
    }


    /**
     * Returns the number of types */
    public int getNumTypes()
    {
	return infos.size();
    }

    public String getName()
    {
	return name;
    }

    //---  AbstractSequence fields
    
    int events_size   = 256;	// 
    int events_fill_p  = 0;	// next available position
    TrackEvent events[] = new TrackEvent[256];
    private MaxVector listeners;
    private MaxVector tempVector = new MaxVector();
    MaxVector infos = new MaxVector();
    //SequenceDataModel sequenceData;
    private String name;
    public static DataFlavor flavors[];
    public static DataFlavor sequenceFlavor = new DataFlavor(ircam.jmax.editors.sequence.SequenceSelection.class, "SequenceSelection");

    static FtsAtom[] sendArgs = new FtsAtom[128];
    static
    {
	for(int i=0; i<128; i++)
	    sendArgs[i]= new FtsAtom();
    }
}








