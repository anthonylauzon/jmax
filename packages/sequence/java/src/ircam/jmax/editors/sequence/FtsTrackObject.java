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
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import java.lang.reflect.*;
import javax.swing.undo.*;
import javax.swing.*;
/**
 * A general-purpose TrackDataModel, this class
 * offers an implementation based on a variable-length 
 * array.
 * @see ircam.jmax.editors.sequence.track.TrackDataModel*/

public class FtsTrackObject extends FtsUndoableObject implements TrackDataModel, ClipableData, ClipboardOwner
{

    /**
     * Create an AbstractSequence and initialize the type vector
     * with the given type.
     */
    //public FtsTrackObject(Fts fts, String name, ValueInfo info)
  public FtsTrackObject(Fts fts, FtsObject parent, String variableName, String className, int nArgs, FtsAtom args[])
  {
      super(fts, null, null, "seqtrack", "seqtrack");

      this.info = ValueInfoTable.getValueInfo(args[0].getString());

      if(nArgs>1)
	  this.trackName = args[1].getString();
      else
	 this.trackName  = "untitled";

      listeners = new MaxVector();
      hhListeners = new MaxVector();
      stateListeners = new MaxVector();

      /* prepare the flavors for the clipboard */
      if (flavors == null)
	  flavors = new DataFlavor[1];
      flavors[0] = sequenceFlavor;
  }

    public void setUntitled()
    {
	trackName = "untitled";
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
	TrackEvent evt = (TrackEvent)(args[0].getObject());

	// beginUpdate is called in adderTool
    
	addEvent(evt);
    
	// ends the undoable transition
	endUpdate();

	setDirty();
    }
    public void addEvents(int nArgs , FtsAtom args[])
    {
	TrackEvent evt = null;
	int evtTime;
	int index = -1;

	//begin update is called in adderTool 
	for(int i=0; i<nArgs; i++)
	    addEvent((TrackEvent)(args[i].getObject()));

	// ends the undoable transition
	endUpdate();

	setDirty();
    }

  public void removeEvents(int nArgs , FtsAtom args[])
  {
    int removeIndex;
    TrackEvent event = null;

    for(int i=0; i<nArgs; i++)
      {
	event = (TrackEvent)(args[i].getObject());
	removeIndex = indexOf(event);
	deleteEventAt(removeIndex);
      }
    // ends the undoable transition
    endUpdate();

    setDirty();
  }

  public void clear(int nArgs , FtsAtom args[])
  {
    while(events_fill_p != 0)
	{
	    if (isInGroup())
		postEdit(new UndoableDelete(events[0]));
	    
	    deleteRoomAt(0);	    
	}
    notifyTrackCleared();

    endUpdate();
    setDirty();
  }

  public void moveEvents(int nArgs , FtsAtom args[])
  {
      /*TrackEvent evt;
	double time;

	for(int i=0; i<nArgs; i+=2)
	{
	evt  = (TrackEvent)(args[i].getObject());
	time = (double)(args[i+1].getFloat());
	evt.moveTo(time);
	}
	endUpdate();      
	setDirty();
      */     

      TrackEvent evt;
      int oldIndex, newIndex;
      double time;
      double maxTime = 0.0;
      TrackEvent maxEvent = null;
      int maxOldIndex = 0; int maxNewIndex = 0;
      for(int i=0; i<nArgs; i+=2)
	  {
	      evt  = (TrackEvent)(args[i].getObject());
	      oldIndex = indexOf(evt);
	      deleteRoomAt(oldIndex);
	      time = (double)(args[i+1].getFloat());

	      if (isInGroup())
		  postEdit(new UndoableMove(evt, time));
	      
	      evt.setTime(time);
	      
	      newIndex = getIndexAfter(time);	
	      if (newIndex == EMPTY_COLLECTION)
		  newIndex = 0;
	      else if (newIndex == NO_SUCH_EVENT)
		  newIndex = events_fill_p;
	
	      makeRoomAt(newIndex);
	      events[newIndex] = evt;

	      if(time >maxTime)
		  { 
		      maxTime = time;
		      maxEvent = evt;
		      maxOldIndex = oldIndex;
		      maxNewIndex = newIndex;
		  }
	      notifyObjectMoved(evt, oldIndex, newIndex);
	  }
      if(nArgs>0)
        notifyLastObjectMoved(maxEvent, maxOldIndex, maxNewIndex);

      endUpdate();      
      setDirty();
  }  

  public void setName(int nArgs , FtsAtom args[])
  {
    
      if(nArgs <= 0)
	  {
	      //here will be the sequence frame , not null but.....
	      JOptionPane.showMessageDialog(null,
					    "A track with this name already exist!\n Choose another name.",
					    "Warning",
					    JOptionPane.WARNING_MESSAGE); 
	      return;
	  }
      else
	  {
	      String name = args[0].getString();
    
	      if(trackName == name)
		  {
		      JOptionPane.showMessageDialog(null,
						    "A track with this name already exist!\n Please choose another name.",
						    "Warning",
						    JOptionPane.WARNING_MESSAGE); 
		      return;
		  }
	      
	      String old = trackName;
	      trackName = name;

	      notifyTrackNameChanged(old, trackName);
	      setDirty();
	  }
  }  


  public void lock(int nArgs , FtsAtom args[])
  {
      locked = true;
      notifyLock(true);
  }

  public void unlock(int nArgs , FtsAtom args[])
  {
      locked = false;
      notifyLock(false);
  }

  public void active(int nArgs[], FtsAtom args[])
  {
      boolean active = (args[0].getInt() == 1);
      notifyActive(active);
  }

  public void highlightEvents(int nArgs , FtsAtom args[])
  {
    int selIndex;
    TrackEvent event = null;
    MaxVector events = new MaxVector();

    for(int i=0; i<nArgs; i++)
    {
	event = (TrackEvent)(args[i].getObject());
	events.addElement(event);
    }
    
    double time = ((TrackEvent)args[0].getObject()).getTime();
    notifyHighlighting(events, time);
  }
  public void requestEventCreation(float time, String type, int nArgs, Object args[])
  {
    sendArgs[0].setFloat(time); 
    sendArgs[1].setString(type);
      
    for(int i=0; i<nArgs; i++)
      {
	if(args[i] instanceof Double)
	    sendArgs[2+i].setFloat(((Double)args[i]).floatValue());
	else
	  sendArgs[2+i].setValue(args[i]);
      }
    sendMessage(FtsObject.systemInlet, "addEvent", 2+nArgs, sendArgs);
  }
  
  public void requestEventCreationWithoutUpload(float time, String type, int nArgs, Object args[])
  {
    sendArgs[0].setFloat(time); 
    sendArgs[1].setString(type);
      
    for(int i=0; i<nArgs; i++)
      {
	if(args[i] instanceof Double)
	  sendArgs[2+i].setFloat(((Double)args[i]).floatValue());
	else
	  sendArgs[2+i].setValue(args[i]);
      }

    sendMessage(FtsObject.systemInlet, "makeEvent", 2+nArgs, sendArgs);
  }

  public void requestEventMove(TrackEvent evt, double newTime)
  {
    sendArgs[0].setObject(evt); 
    sendArgs[1].setDouble(newTime);
    sendMessage(FtsObject.systemInlet, "moveEvents", 2, sendArgs);
  }

  public void requestEventsMove(Enumeration events, int deltaX, Adapter a)
  {
      TrackEvent aEvent;
      int i = 0;
      for (Enumeration e = events; e.hasMoreElements();) 
	  {	  
	      aEvent = (TrackEvent) e.nextElement();		    
	      if(i<NUM_ARGS-1)
		  {
		      sendArgs[i].setObject(aEvent);
		      sendArgs[i+1].setDouble(a.getInvX(a.getX(aEvent)+deltaX));
		      i+=2;
		  }
	      else
		  {
		      sendMessage(FtsObject.systemInlet, "moveEvents", i, sendArgs);
		      i=0;
		      sendArgs[i].setObject(aEvent);
		      sendArgs[i+1].setDouble(a.getInvX(a.getX(aEvent)+deltaX));		      
		      i+=2;
		  }
	  }
      
      if(i!=0)
	  sendMessage(FtsObject.systemInlet, "moveEvents", i, sendArgs);
  }

  public void requestSetName(String newName)
  {
    sendArgs[0].setString(newName); 
    sendMessage(FtsObject.systemInlet, "setName", 1, sendArgs);
  }    

  public void requestClearTrack()
  {
    sendMessage(FtsObject.systemInlet, "clear", 0, null);
  }    

  public void export()
  {
      sendMessage(FtsObject.systemInlet, "export_midi_dialog", 0, null);
  }

  public void requestSetActive(boolean active)
  {
      sendArgs[0].setInt((active)? 1 : 0); 
      sendMessage(FtsObject.systemInlet, "active", 1, sendArgs);
  }

    /**
     * how many events in the database?
     */
    public int length()
    {
	return events_fill_p;
    }
    
    public double getMaximumTime()
    {
	double time;
	double max = 0;

	for (int i=0 ; i<events_fill_p;i++) 
	    {
		time = events[i].getTime()+((Double)events[i].getProperty("duration")).intValue();
		if(time>max) max = time;
	    }
	return max;
    }

    public boolean isLocked()
    {
	return locked;
    }
    
    /**
     * returns an enumeration of all the events
     */
    public Enumeration getEvents()
    {
	return new SequenceEnumeration();
    }
    
    public Enumeration getEvents(int startIndex , int endIndex)
    {
	return new SequenceEnumeration(startIndex, endIndex);
    }
    
    /**
     * returns a given event
     */
    public TrackEvent getEventAt(int index)
    {
	return events[index];
    }

    public TrackEvent getNextEvent(Event evt)
    {
	int index;
	if(evt instanceof UtilTrackEvent)
	    index = getFirstEventAfter(evt.getTime()+0.001);		
	else
	    index = indexOf(evt) + 1;

	if((index != EMPTY_COLLECTION) && (index < events_fill_p) && (index >= 0))
	    return events[index];
	else return null;
    }

    public TrackEvent getPreviousEvent(double time)
    {
	int index = getFirstEventBefore(time);

	if((index != EMPTY_COLLECTION) && (index < events_fill_p) && (index >= 0))
	    return events[index];
	else return null;
    }
    
    public TrackEvent getPreviousEvent(Event evt)
    {
	int index = getFirstEventBefore(evt.getTime());

	if((index != EMPTY_COLLECTION) && (index < events_fill_p) && (index >= 0))
	    return events[index];
	else return null;
    }
    public TrackEvent getLastEvent()
    {
	if(events_fill_p > 0)
	    return events[events_fill_p-1];
	else
	    return null;
    }

    /**
     * return the index of the given event, if it exists, or the error constants
     * NO_SUCH_EVENT, EMPTY_COLLECTION */
    public int indexOf(Event event)
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

    public int getFirstEventAfter(double time)
    {
	if (events_fill_p == 0) 
	    return EMPTY_COLLECTION;
	
	else if (events[events_fill_p - 1].getTime()<= time)
		return NO_SUCH_EVENT;	
	
	if(events[0].getTime() >= time)
	    return 0;

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
	
	return max;
	
	//else return NO_SUCH_EVENT;
    }

    public int getFirstEventBefore(double time)
    {
	if (events_fill_p == 0) 
	    return EMPTY_COLLECTION;	
	else if (events[0].getTime() >= time)  
	    return NO_SUCH_EVENT;
	else if (events[events_fill_p-1].getTime() < time)
	    return events_fill_p-1;

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
	
	return min;
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

	if (isInGroup())     
	    {
		postEdit(new UndoableAdd(event));
	    }
    }

    /**
     * generic change of an event in the database.
     * Call this function to signal the parameters changing of the event, except
     * the initial time and the duration parameters. Use moveEvent and resizeEvent for that.
     */
    public void changeEvent(TrackEvent event, String propName, Object propValue)
    {
	int index;
	
	index = indexOf(event);
	
	if (index == NO_SUCH_EVENT || index == EMPTY_COLLECTION)
	    return;

	notifyObjectChanged(event, propName, propValue);
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
    public void deleteEvent(TrackEvent event)
    {
	sendArgs[0].setObject(event);
	sendMessage(FtsObject.systemInlet, "removeEvents", 1, sendArgs);
    }
    public void deleteEvents(Enumeration events)
    {
      int i = 0;
      for (Enumeration e = events; e.hasMoreElements();) 
	  {	      
	      if(i<NUM_ARGS)
		  sendArgs[i++].setObject((TrackEvent) e.nextElement());
	      else
		  {
		      sendMessage(FtsObject.systemInlet, "removeEvents", NUM_ARGS, sendArgs); 
		      i = 0;
		  }
	  }
      
      if(i!=0)
	  sendMessage(FtsObject.systemInlet, "removeEvents", i, sendArgs);
      
    }

    public void deleteAllEvents()
    {
	while(events_fill_p != 0)
	  deleteEvent(events[0]);
    }

    private void deleteEventAt(int removeIndex)
    {
	TrackEvent event = getEventAt(removeIndex);
	if (removeIndex == NO_SUCH_EVENT || removeIndex == EMPTY_COLLECTION)
	    return;
	
	if (isInGroup())
	    postEdit(new UndoableDelete(event));
	
	deleteRoomAt(removeIndex);
	
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

    private void notifyObjectsAdded(int maxTime)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectsAdded(maxTime);
    }
    
    private void notifyObjectDeleted(Object spec, int oldIndex)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectDeleted(spec, oldIndex);
    }
    
    private void notifyObjectChanged(Object spec, String propName, Object propValue)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectChanged(spec, propName, propValue);
    }
    private void notifyTrackNameChanged(String oldName, String newName)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).trackNameChanged(oldName, newName);
    }
    private void notifyTrackCleared()
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).trackCleared();
    }
    private void notifyObjectMoved(Object spec, int oldIndex, int newIndex)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).objectMoved(spec, oldIndex, newIndex);
    }
    private void notifyLastObjectMoved(Object spec, int oldIndex, int newIndex)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((TrackDataListener) e.nextElement()).lastObjectMoved(spec, oldIndex, newIndex);
    }
    private void notifyHighlighting(MaxVector hhobj, double time)
    {
	for (Enumeration e = hhListeners.elements(); e.hasMoreElements();) 
	    ((HighlightListener) e.nextElement()).highlight(hhobj.elements(), time);
    }
    private void notifyLock(boolean lock)
    {
	for (Enumeration e = stateListeners.elements(); e.hasMoreElements();) 
	    ((TrackStateListener) e.nextElement()).lock(lock);
    }
    private void notifyActive(boolean active)
    {
	for (Enumeration e = stateListeners.elements(); e.hasMoreElements();) 
	    ((TrackStateListener) e.nextElement()).active(active);
    }
    /**
     * requires to be notified when the database changes
     */
    public void addListener(TrackDataListener theListener)
    {
	listeners.addElement(theListener);
    }
    /**
     * requires to be notified at events highlight
     */
    public void addHighlightListener(HighlightListener listener)
    {
	hhListeners.addElement(listener);
    }

    public void addTrackStateListener(TrackStateListener listener)
    {
	stateListeners.addElement(listener);
    }
    
    /**
     * removes the listener
     */
    public void removeListener(TrackDataListener theListener)
    {
	listeners.removeElement(theListener);
    }
    /**
     * removes the listener
     */
    public void removeHighlightListener(HighlightListener theListener)
    {
	hhListeners.removeElement(theListener);
    }

    public void removeTrackStateListener(TrackStateListener theListener)
    {
	stateListeners.removeElement(theListener);
    }

    // ClipableData functionalities
    public void cut()
    {	
	if (SequenceSelection.getCurrent().getModel() != this) return;
	
	copy();
	
	beginUpdate(); //cut is undoable
	SequenceSelection.getCurrent().deleteAll();
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

	if (clipboardContent != null && areMyDataFlavorsSupported(clipboardContent))
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
		Event event;
				
		SequenceSelection.getCurrent().deselectAll();

		if(objectsToPaste.hasMoreElements())
		    {
			event = (Event) objectsToPaste.nextElement();
			
			if(event.getValue().getValueInfo() != getType())
			    {
				System.err.println("Clipboard error in paste: attempt to copy <"+event.getValue().getValueInfo().getPublicName()+"> events in <"+getType().getPublicName()+"> track!");
				return;
			    }

			try {

			    beginUpdate();  //the paste is undoable


			    requestEventCreationWithoutUpload((float)event.getTime(), 
							      event.getValue().getValueInfo().getName(), 
							      event.getValue().getPropertyCount(), 
							      event.getValue().getPropertyValues());
		    
			    while (objectsToPaste.hasMoreElements())
				{
				    event = (Event) objectsToPaste.nextElement();
				    requestEventCreationWithoutUpload((float)event.getTime(), 
								      event.getValue().getValueInfo().getName(), 
								      event.getValue().getPropertyCount(), 
								      event.getValue().getPropertyValues());
				}
		    
			    sendMessage(FtsObject.systemInlet, "upload", 0, sendArgs);
			}
			catch (Exception e) {}
		    }
	    }
    }
    
    /** ClipboardOwner interface */
    
    public void lostOwnership(Clipboard clipboard,
			      Transferable contents)
    {
	//SequenceSelection.discardTheCopy();
    }
    
    boolean areMyDataFlavorsSupported(Transferable clipboardContent)
    {
	boolean supported = true;
	
	for(int i=0; i<flavors.length;i++)
	    supported = supported&&clipboardContent.isDataFlavorSupported(flavors[i]);
	
	return supported;
    }

    //-------------------------------------------------------
    
    /* Private methods */
    
    final int getIndexAfter(double time)
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
	for (int i = index;  i < events_fill_p-1; i++)
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
	int start, end;
	public SequenceEnumeration()
	{
	    start = 0;
	    end = events_fill_p;
	    p = start;
	}
	public SequenceEnumeration(int startIndex, int endIndex)
	{
	    if(startIndex < 0) 
		start = 0;
	    else 
		start = startIndex;

	    if(endIndex > events_fill_p)
		end = events_fill_p;
	    else
		end = endIndex + 1;
	    
	    p = start;
	}
	
	public boolean hasMoreElements()
	{
	    return p < end;
	}
	
	public Object nextElement()
	{
	    return events[p++];
	}
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
			e.getTime()+((Double)e.getProperty("duration")).intValue()>= startTime)
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

		    if (e.getTime()+ ((Double)e.getProperty("duration")).intValue()<=endTime)
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
     * Move all the events of the given model in this model. Merging is allowed only between 
     * tracks of the same type. Merging is not undoable.      
     */
    public void mergeModel(TrackDataModel model)
    {
	Event event;

	beginUpdate(); 
	
	try {
	    for (Enumeration e = model.getEvents(); e.hasMoreElements();)
		{
		    event = (Event) e.nextElement();
		    requestEventCreationWithoutUpload((float)event.getTime(), 
						      event.getValue().getValueInfo().getName(), 
						      event.getValue().getPropertyCount(), 
						      event.getValue().getPropertyValues());
		}	    
	    sendMessage(FtsObject.systemInlet, "upload", 0, sendArgs);
	}
	catch (Exception e) {}
    }

    /**
     * Returns the ValueInfo contained in this model
     */
    public ValueInfo getType()
    {
	return info;
    }

    public String getName()
    {
	return trackName;
    }

    public DataFlavor[] getDataFlavors()
    {
	return flavors;
    }

    public TrackEvent getEventLikeThis(Event e)
    {
	TrackEvent evt;
	TrackEvent retEvt = null;
	int index = getFirstEventAt((float)e.getTime());
		
	if((index != EMPTY_COLLECTION) && (index < events_fill_p) && (index >= 0))	    
	    {
		evt = events[index];	
		
		if(e.getValue().samePropertyValues(evt.getValue().getPropertyValues()))
		    retEvt = evt;
		else
		    {
			evt = getNextEvent(evt);
			
			while((evt!=null)&&(evt.getTime()==e.getTime()))
			    {
				if(e.getValue().samePropertyValues(evt.getValue().getPropertyValues()))
				    {					
					retEvt = evt;
					break;
				    }		    
				else
				    evt = getNextEvent(evt);
			    }       			
		    }
	    }
	return retEvt;
    }

    /** utility function */
    protected void addFlavor(DataFlavor flavor)
    {
	int dim = flavors.length;
	DataFlavor temp[] = new DataFlavor[dim+1];
	for (int i = 0; i < dim; i++){
	    temp[i] = flavors[i];
	}
	temp[dim]=flavor;
	flavors = temp;
    }

    //---  AbstractSequence fields
    ValueInfo info;

    boolean locked = false;
    int events_size   = 256;	// 
    int events_fill_p  = 0;	// next available position
    TrackEvent events[] = new TrackEvent[256];
    private MaxVector listeners;
    private MaxVector hhListeners;
    private MaxVector stateListeners;
    private MaxVector tempVector = new MaxVector();
    private String trackName;
    public DataFlavor flavors[];

    public static DataFlavor sequenceFlavor = new DataFlavor(ircam.jmax.editors.sequence.SequenceSelection.class, "SequenceSelection");

    public final static int NUM_ARGS = 256;
    public static FtsAtom[] sendArgs = new FtsAtom[NUM_ARGS];

    static
    {
	for(int i=0; i<NUM_ARGS; i++)
	    sendArgs[i]= new FtsAtom();
    }
}








