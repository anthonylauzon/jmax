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

/* FIXME: check how many imports are really needed */
import ircam.jmax.MaxApplication;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
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
    public FtsSequenceObject(Fts fts, FtsObject parent, String variableName, String classname)
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
      if(sequence == null){
	  sequence = new Sequence(this);
	  setEditorFrame(sequence);
      }
    if (! sequence.isVisible())
      sequence.setVisible(true);
    sequence.toFront();
  }

  /**
   * Fts callback: destroy the editor associated with this FtsSequenceObject.
   */
  public void destroyEditor(int nArgs, FtsAtom args[])
  {
    if(sequence != null)
    {
      sequence.dispose();
      sequence = null;
      setEditorFrame(null);
    }
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
	int position = args[1].getInt();
	
	sequence.itsSequencePanel.moveTrackTo(track, position);

	setDirty();
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

    /*public Track getTrackByName(String name)
      {
      Track track;
      for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
      track = (Track)e.nextElement();
      if(track.getName().equals(name)) return track;
      }
      return null;
      }*/
    public Enumeration getTracks()
    {
	return tracks.elements();
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

    if (result == JOptionPane.OK_OPTION)
	{
	    sendArgs[0].setObject((FtsTrackObject)track.getTrackDataModel()); 
	    sendMessage(FtsObject.systemInlet, "remove_track", 1, sendArgs);
	}
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
	sendArgs[0].setString(type); 
	sendMessage(FtsObject.systemInlet, "add_track", 1, sendArgs);
    }

    public void requestTrackMove(Track track, int position)
    {
	sendArgs[0].setObject(track.getFtsTrack()); 
	sendArgs[1].setInt(position); 
	sendMessage(FtsObject.systemInlet, "move_track", 2, sendArgs);
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

  //----- Fields
  static FtsAtom[] sendArgs = new FtsAtom[128];
  static
  {
      for(int i=0; i<128; i++)
	  sendArgs[i]= new FtsAtom();
  }
  Sequence sequence = null;  
  
  Vector tracks = new Vector();
  MaxVector listeners = new MaxVector();
  String name = new String("unnamed"); //to be assigned by FTS, usually via a specialized KEY

  //unic id for a track, starting from zero;
  private int trackId = 0; 
  static final int REMOTE_TRACK_ADD = 0; 
}











