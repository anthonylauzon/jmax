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

import ircam.ftsclient.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.*;
import ircam.jmax.fts.FtsUndoableObject;
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

public class FtsTrackObject extends FtsUndoableObject implements TrackDataModel, ClipableData, ClipboardOwner {
  static
  {
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("addEvent"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).addEventFromServer( (TrackEvent)args.getObject( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("addEvents"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).addEvents(argc, argv);
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("removeEvents"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).removeEvents(argc, argv);
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).clear();
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("moveEvents"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).moveEvents(argc, argv);
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("setName"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  String name = null;
	  if (args.getLength() > 0)
	    name = args.getSymbol(0).toString();
		  
	  ((FtsTrackObject)obj).setName( name);
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("lock"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).lock();		  
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("unlock"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).unlock();		  
	}
      });
    FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("active"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTrackObject)obj).active( args.getInt(0) == 1);
      }
      });
  FtsObject.registerMessageHandler( FtsTrackObject.class, FtsSymbol.get("highlightEvents"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
	((FtsTrackObject)obj).highlightEvents(argc, argv);		  
      }
    });
  }

  /**
     * Create an AbstractSequence and initialize the type vector
     * with the given type.
     */
  public FtsTrackObject(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom args[], int id)
  {
    super(server, parent, className, nArgs, args, id);

    this.info = ValueInfoTable.getValueInfo(args[0].stringValue);

    if(nArgs>1)
      this.trackName = args[1].stringValue;
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
  public void addEventFromServer(TrackEvent evt)
  {
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
      addEvent((TrackEvent)(args[i].objectValue));

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
	event = (TrackEvent)(args[i].objectValue);
	removeIndex = indexOf(event);
	deleteEventAt(removeIndex);
      }
    // ends the undoable transition
    endUpdate();

    setDirty();
  }

  public void clear()
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
    TrackEvent evt;
    int oldIndex, newIndex;
    double time;
    double maxTime = 0.0;
    TrackEvent maxEvent = null;
    int maxOldIndex = 0; int maxNewIndex = 0;
    for(int i=0; i<nArgs; i+=2)
      {
	evt  = (TrackEvent)(args[i].objectValue);
	oldIndex = indexOf(evt);
	deleteRoomAt(oldIndex);
	time = (double)(args[i+1].floatValue);

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

  public void setName(String name)
  {    
    if(name == null)
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


  public void lock()
  {
    locked = true;
    notifyLock(true);
  }

  public void unlock()
  {
    locked = false;
    notifyLock(false);
  }

  public void active(boolean active)
  {
    notifyActive(active);
  }

  public void highlightEvents(int nArgs, FtsAtom args[])
  {
    int selIndex;
    TrackEvent event = null;
    
    MaxVector events = new MaxVector();

    for(int i=0; i<nArgs; i++)
      {
	event = (TrackEvent)(args[i].objectValue);
	events.addElement(event);
      }
    
    double time = ((TrackEvent)args[0].objectValue).getTime();
    notifyHighlighting(events, time);
  }
  public void requestEventCreation(float time, String type, int nArgs, Object arguments[])
  {
    args.clear();
    args.add(time);
    args.add(type);

    for(int i=0; i<nArgs; i++)
      if(arguments[i] instanceof Double)
	args.add(((Double)arguments[i]).floatValue());
      else
	args.add(arguments[i]);

    try{
      send( FtsSymbol.get("addEvent"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending addEvent Message!");
	e.printStackTrace(); 
      }
  }
  
  public void requestEventCreationWithoutUpload(float time, String type, int nArgs, Object arguments[])
  {
    args.clear();
    args.add(time);
    args.add(type);

    for(int i=0; i<nArgs; i++)
      if(arguments[i] instanceof Double)
	args.add(((Double)arguments[i]).floatValue());
      else
	args.add(arguments[i]);

    try{
      send( FtsSymbol.get("makeEvent"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending makeEvent Message!");
	e.printStackTrace(); 
      }   
  }

  public void requestEventMove(TrackEvent evt, double newTime)
  {
    args.clear();
    args.add(evt);
    args.add((float)newTime);
      
    try{
      send( FtsSymbol.get("moveEvents"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending moveEvents Message!");
	e.printStackTrace(); 
      }   
  }

  public void requestEventsMove(Enumeration events, int deltaX, Adapter a)
  {
    TrackEvent aEvent;

    args.clear();
    for (Enumeration e = events; e.hasMoreElements();) 
      {	  
	aEvent = (TrackEvent) e.nextElement();		    
	args.add(aEvent);
	args.add((float)a.getInvX(a.getX(aEvent)+deltaX));
      }
      
    try{
      send( FtsSymbol.get("moveEvents"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending moveEvents Message!");
	e.printStackTrace(); 
      }   
  }
    
  public void requestSetName(String newName)
  {
    args.clear();
    args.add(newName);
      
    try{
      send( FtsSymbol.get("setName"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending setName Message!");
	e.printStackTrace(); 
      }   
  }    

  public void requestClearTrack()
  {
    try{
      send( FtsSymbol.get("clear"));
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending clear Message!");
	e.printStackTrace(); 
      }   
  }    

  public void export()
  {
    try{
      send( FtsSymbol.get("export_midi_dialog"));
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending export_midi_dialog Message!");
	e.printStackTrace(); 
      }  
  }

  public void requestSetActive(boolean active)
  {
    args.clear();
    args.add((active)? 1 : 0);

    try{
      send( FtsSymbol.get("active"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending active Message!");
	e.printStackTrace(); 
      }  
  }

  public void requestUpload()
  {
    try{
      send( FtsSymbol.get("upload"));
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending upload Message!");
	e.printStackTrace(); 
      }  
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
    args.clear();
    args.add(event);

    try{
      send( FtsSymbol.get("removeEvents"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending removeEvents Message!");
	e.printStackTrace(); 
      }  
  }
  public void deleteEvents(Enumeration events)
  {
    args.clear();
    for (Enumeration e = events; e.hasMoreElements();) 
      args.add((TrackEvent) e.nextElement());
      
    try{
      send( FtsSymbol.get("removeEvents"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTrackObject: I/O Error sending removeEvents Message!");
	e.printStackTrace(); 
      }  
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
	    System.err.println("Clipboard error in paste: content does not support "+
			       SequenceDataFlavor.getInstance().getHumanPresentableName());
	  } 
	catch (IOException ioe)
	  {
	    System.err.println("Clipboard error in paste: content is no more an "+
			       SequenceDataFlavor.getInstance().getHumanPresentableName());
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
		System.err.println("Clipboard error in paste: attempt to copy <"+
				   event.getValue().getValueInfo().getPublicName()+
				   "> events in <"+getType().getPublicName()+"> track!");
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
		    
	      requestUpload();
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

      try{
	send( FtsSymbol.get("upload"));
      }
      catch(IOException e)
	{
	  System.err.println("FtsBpfObject: I/O Error sending upload Message!");
	  e.printStackTrace(); 
	}
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

  protected FtsArgs args = new FtsArgs();
}
