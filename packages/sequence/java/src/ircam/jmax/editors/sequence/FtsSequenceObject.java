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
import ircam.jmax.editors.sequence.renderers.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import java.util.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsSequenceObject extends FtsObjectWithEditor implements SequenceDataModel
{
  /**
   * constructor.
   */
 public FtsSequenceObject(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
 {
     super(fts, parent, variableName, "sequence", "sequence");
     
     listeners = new MaxVector();
     initValueInfoTable();
     SequenceImages.init();
 }

    private final void initValueInfoTable()
    {
	ValueInfoTable.registerInfo(AmbitusValue.info);
	ValueInfoTable.registerInfo(IntegerValue.info);
	ValueInfoTable.registerInfo(MessageValue.info);
	ValueInfoTable.registerInfo(FloatValue.info);
	ValueInfoTable.registerInfo(MidiValue.info);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

  /**
   * Fts callback: open the editor associated with this FtsSequenceObject.
   * If not exist create them else show them.
   */
  public void createEditor(int nArgs, FtsAtom args[])
  {
    if(sequence == null)
	{
	    sequence = new Sequence(this);
	    setEditorFrame(sequence);
	}
    showEditor();
  }

  /**
   * Fts callback: destroy the editor associated with this FtsSequenceObject.
   */
  public void destroyEditor(int nArgs, FtsAtom args[])
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
      
      for(int i=0; i<nArgs; i++)
	  {
	      trackObj = (FtsTrackObject)(args[i].getObject());
	      trackObj.setParent(this);//!!!!!!!!!!!

	      track = new TrackBase(trackObj);
	      tracks.addElement(track);

	      notifyTrackAdded(track);

	      time = (int)trackObj.getMaximumTime();
	      if(time>trackTime) trackTime = time;	      
	  }

      setDirty();
      
      notifyTracksAdded(trackTime);
  }
   
  public void deleteTracks(int nArgs , FtsAtom args[])
  {
      FtsTrackObject trackObj;
      Track track;
      int time;
      int trackTime = 0;
      
      for(int i=0; i<nArgs; i++)
	  {
	      trackObj = (FtsTrackObject)(args[i].getObject());
	      track = getTrack(trackObj);
	      tracks.removeElement(track);
	      notifyTrackRemoved(track);
	  }    
      setDirty();
  }

    public void moveTrack(int nArgs , FtsAtom args[])
    {
	int time;
	int trackTime = 0;
      
	FtsTrackObject trackObj = (FtsTrackObject)(args[0].getObject());
	Track track = getTrack(trackObj);
	int oldPosition = getTrackIndex(track);
	int newPosition = args[1].getInt();
	
	sequence.itsSequencePanel.moveTrackTo(track, newPosition);
	notifyTrackMoved(track, oldPosition, newPosition);

	setDirty();
    }

    public void setName(int nArgs , FtsAtom args[])
    {
	sequence.setName(args[0].getString());
    }

    public void setEditorGeometry(int nArgs , FtsAtom args[])
    {
	sequence.itsSequencePanel.geometry.setXZoom((int)(100*args[0].getFloat()));
	sequence.itsSequencePanel.itsTimeScrollbar.setValue(-args[1].getInt());
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
	FtsTrackObject.sendArgs[0].setString(type); 
	sendMessage(FtsObject.systemInlet, "add_track", 1, FtsTrackObject.sendArgs);
    }

    public void requestTrackMove(Track track, int position)
    {
	FtsTrackObject.sendArgs[0].setObject(track.getFtsTrack()); 
	FtsTrackObject.sendArgs[1].setInt(position); 
	sendMessage(FtsObject.systemInlet, "move_track", 2, FtsTrackObject.sendArgs);
    }

    public void requestTrackRemove(Track track)
    {
	FtsTrackObject.sendArgs[0].setObject((FtsTrackObject)track.getTrackDataModel()); 
	sendMessage(FtsObject.systemInlet, "remove_track", 1, FtsTrackObject.sendArgs);
    }
    
    public void requestSetZoom(float zoom)
    {
	FtsTrackObject.sendArgs[0].setFloat(zoom); 
	sendMessage(FtsObject.systemInlet, "set_zoom", 1, FtsTrackObject.sendArgs);    
	setDirty();
    }

    public void requestSetScroll(int scroll)
    {
	FtsTrackObject.sendArgs[0].setInt(scroll); 
	sendMessage(FtsObject.systemInlet, "set_scroll", 1, FtsTrackObject.sendArgs);    
	setDirty();
    }

    public void requestSequenceName()
    {
	sendMessage(FtsObject.systemInlet, "get_name", 0, null);
    }

    public void importMidiFile()
    {
	sendMessage(FtsObject.systemInlet, "import_midi_dialog", 0, null);
    }

    public void closeEditor()
    {
	sendMessage(FtsObject.systemInlet, "close_editor", 0, null);
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

  Sequence sequence = null;  
  
  Vector tracks = new Vector();
  MaxVector listeners = new MaxVector();
  String name = new String("unnamed"); //to be assigned by FTS, usually via a specialized KEY
}











