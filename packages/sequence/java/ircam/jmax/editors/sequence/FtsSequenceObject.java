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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.renderers.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

import java.util.*;
import java.io.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsSequenceObject extends FtsObjectWithEditor implements SequenceDataModel
{
 static
  {
    FtsObject.registerMessageHandler( FtsSequenceObject.class, FtsSymbol.get("addTracks"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSequenceObject)obj).addTracks(args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsSequenceObject.class, FtsSymbol.get("removeTracks"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSequenceObject)obj).removeTracks(args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsSequenceObject.class, FtsSymbol.get("moveTrack"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSequenceObject)obj).moveTrack((FtsTrackObject)args.getObject( 0), args.getInt( 1));
	}
      });
    FtsObject.registerMessageHandler( FtsSequenceObject.class, FtsSymbol.get("setName"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsSequenceObject)obj).setName(args.getSymbol( 0).toString());
	}
      });
  }
  
  /**
   * constructor.
   */
  public FtsSequenceObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length);

    listeners = new MaxVector();
    ValueInfoTable.init();
    SequenceImages.init();
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from fts.
  //////////////////////////////////////////////////////////////////////////////////////
  
  /**
   * Fts callback: open the editor associated with this FtsSequenceObject.
   * If not exist create them else show them.
   */
  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(sequence == null)
      {
	sequence = new SequenceWindow(this);
	setEditorFrame(sequence);
      }
    showEditor();
  }

  /**
   * Fts callback: destroy the editor associated with this FtsSequenceObject.
   */
  public void destroyEditor()
  {
    sequence = null;
    disposeEditor();
  }
  public void addTracks(int nArgs , FtsAtom args[])
  {
    FtsTrackObject trackObj;
    TrackBase track;
    int time;
    int trackTime = 0;
    
    int id = args[0].intValue;
    trackObj = new FtsTrackObject(getServer(), this, id, "track", args, 1, nArgs);
    track = new TrackBase(trackObj);
    tracks.addElement(track);
    
    notifyTrackAdded(track);
  }
   
  public void removeTracks(int nArgs , FtsAtom args[])
  {
    FtsTrackObject trackObj;
    Track track;
    int time;
    int trackTime = 0;
    
    for(int i=0; i<nArgs; i++)
      {
	trackObj = (FtsTrackObject)(args[i].objectValue);
	track = getTrack(trackObj);
	tracks.removeElement(track);
	notifyTrackRemoved(track);
      }    
  }

  public void moveTrack(FtsTrackObject trackObj, int newPosition)
  {
    int time;
    int trackTime = 0;
    
    Track track = getTrack(trackObj);
    int oldPosition = getTrackIndex(track);
    
    sequence.itsSequencePanel.moveTrackTo(track, newPosition);
    notifyTrackMoved(track, oldPosition, newPosition);
  }

  public void setName(String name)
  {
    sequence.setName(name);
  }

  public void setOpenedAllTracks(boolean opened)
  {
    Track track;
    Object val;
    if(opened) val = Boolean.TRUE;
    else val = Boolean.FALSE;
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      ((Track)e.nextElement()).setProperty("opened", val);
  }

  /**
   * return how many tracks in the sequence
   */
  public int trackCount()
  {
    return tracks.size();
  }

  /**
   * return the name of the sequence
   */
  public String getName()
  {
    return name;
  }

  /**
   * Returns the i-th track in the vector 
   */
  public Track getTrackAt(int i)
  {
    return (Track) tracks.elementAt(i);
  }

  public int getTrackIndex(Track track)
  {
    return tracks.indexOf(track);
  }
    
  public Enumeration getTracks()
  {
    return tracks.elements();
  }

  public Enumeration getTracks(ValueInfo type)
  {
    Track track;
    Vector temp = new Vector();
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
	track = (Track)e.nextElement();
	if(track.getTrackDataModel().getType() == type) temp.addElement(track);
      }
    return temp.elements();
  }
  /**
   * Returns the track associated with this FtsTrackObject 
   */
  public Track getTrack(FtsTrackObject obj)
  {
    Track track;
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
	track = (Track)e.nextElement();
	if(track.getFtsTrack() == obj) return track;
      }
    return null;
  }

  public Enumeration getTypes()
  {
    ValueInfo type;
    Vector types = new Vector();
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
	type = ((Track)e.nextElement()).getTrackDataModel().getType();
	if(!types.contains(type)) types.addElement(type);
      }
    return types.elements();
  }

  /**
   * Remove a Track from this sequencer 
   */
  public void removeTrack(Track track)
  {
    if(track==null) return;
    
    int result = JOptionPane.OK_OPTION;
    if(track.getTrackDataModel().length() > 0)
      result = JOptionPane.showConfirmDialog( sequence,
					      "Removing tracks is not Undoable.\nOK to remove ?",
					      "Warning",
					      JOptionPane.YES_NO_OPTION,
					      JOptionPane.WARNING_MESSAGE);
    if(result == JOptionPane.OK_OPTION)
      requestTrackRemove(track);
  }

  public void changeTrack(Track track)
  {
    notifyTrackChanged(track);
  }

  public TrackEvent getLastEvent()
  {
    Track track;
    TrackEvent evt;
    TrackEvent lastEvt = null;
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
	track = (Track)e.nextElement();
	evt = track.getTrackDataModel().getLastEvent();
	if(lastEvt == null) lastEvt = evt;
	else 
	  if(evt!=null)		    
	    if(lastEvt.getTime() < evt.getTime()) lastEvt = evt;
      }
    return lastEvt;
  }

  public void requestTrackCreation(String type)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get(type));
    
    try{
      send( FtsSymbol.get("add_track"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsSequenceObject: I/O Error sending add_track Message!");
	e.printStackTrace(); 
      }
  }

  public void requestTrackMove(Track track, int position)
  {
    args.clear();
    args.addObject( track.getFtsTrack());
    args.addInt( position);
	
    try{
      send( FtsSymbol.get("move_track"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsSequenceObject: I/O Error sending move_track Message!");
	e.printStackTrace(); 		
      }
  }
  
  public void requestTrackRemove(Track track)
  {
    args.clear();
    args.addObject( track.getFtsTrack());
	
    try{
      send( FtsSymbol.get("remove_track"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsSequenceObject: I/O Error sending remove_track Message!");
	e.printStackTrace(); 		
      }
  }

  public void requestSequenceName()
  {
    try{
      send( FtsSymbol.get("getName"));
    }
    catch(IOException e)
      {
	System.err.println("FtsSequenceObject: I/O Error sending getName Message!");
	e.printStackTrace(); 		
      }
  }

  public void importMidiFile()
  {
    try{
      send( FtsSymbol.get("import_midifile_dialog"));
    }
    catch(IOException e)
      {
	System.err.println("FtsSequenceObject: I/O Error sending import_midifile_dialog Message!");
	e.printStackTrace(); 		
      }
  }

  /**
   * Require to be notified when database change
   */
  public void addTrackListener(TrackListener theListener) 
  {
    listeners.addElement(theListener);
  }

  /**
   * Remove the listener
   */
  public void removeTrackListener(TrackListener theListener) 
  {
    listeners.removeElement(theListener);
  }

  private void notifyTrackAdded(Track track)
  {
    for (Enumeration e=listeners.elements(); e.hasMoreElements();)
      ((TrackListener)(e.nextElement())).trackAdded(track);
  }

  private void notifyTracksAdded(int maxTime)
  {
    for (Enumeration e=listeners.elements(); e.hasMoreElements();)
      ((TrackListener)(e.nextElement())).tracksAdded(maxTime);
  }

  private void notifyTrackRemoved(Track track)
  {
    for (Enumeration e=listeners.elements(); e.hasMoreElements();)
      ((TrackListener)(e.nextElement())).trackRemoved(track);
  }

  private void notifyTrackChanged(Track track)
  {
    for (Enumeration e=listeners.elements(); e.hasMoreElements();)
      ((TrackListener)(e.nextElement())).trackChanged(track);
  }

  private void notifyTrackMoved(Track track, int oldPosition, int newPosition)
  {
    for (Enumeration e=listeners.elements(); e.hasMoreElements();)
      ((TrackListener)(e.nextElement())).trackMoved(track, oldPosition, newPosition);
  }

  SequenceWindow sequence = null;  
  
  Vector tracks = new Vector();
  MaxVector listeners = new MaxVector();
  String name = new String("unnamed"); //to be assigned by FTS, usually via a specialized KEY
  
  protected FtsArgs args = new FtsArgs();
}











