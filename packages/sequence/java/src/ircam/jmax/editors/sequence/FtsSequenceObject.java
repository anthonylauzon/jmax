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


/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsSequenceObject extends FtsObject implements SequenceDataModel
{

  /**
   * constructor.
   */
    public FtsSequenceObject(Fts fts, FtsObject parent, String classname)
    {
      super(fts, parent, null, "sequence", "sequence");
	
      listeners = new MaxVector();
      initValueInfoTable();
      SequenceImages.init();
    }

    private final void initValueInfoTable()
    {
	ValueInfoTable.registerInfo(AmbitusValue.info);
	ValueInfoTable.registerInfo(IntegerValue.info);
	ValueInfoTable.registerInfo(MessageValue.info);
	//ValueInfoTable.registerInfo(LogicValue.info);
	//ValueInfoTable.registerInfo(CueValue.info);
	//ValueInfoTable.registerInfo(FricativeValue.info);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

  /**
   * Fts callback: open the editor associated with this FtsSequenceObject.
   * If not exist create them else show them.
   */
  public void openEditor(int nArgs, FtsAtom args[])
  {
    if(sequence == null)
      sequence = new Sequence(this);

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
    }
  }

  /**
   * Fts callback: add a new Track(first arg) . 
   * 
   */
    /*public void addTrack(int nArgs , FtsAtom args[])
      {
      FtsTrackObject trackObj = (FtsTrackObject)(args[0].getObject());
      Track track = new TrackBase(trackObj);
      tracks.addElement(track);
      
      notifyTrackAdded(track);
      }*/
    
  public void addTracks(int nArgs , FtsAtom args[])
  {
      FtsTrackObject trackObj;
      TrackBase track;
      int time;
      int trackTime = 0;
      
      for(int i=0; i<nArgs; i++)
	  {
	      trackObj = (FtsTrackObject)(args[i].getObject());
	      track = new TrackBase(trackObj);
	      tracks.addElement(track);
	      notifyTrackAdded(track);
	      time = (int)trackObj.getMaximumTime();
	      if(time>trackTime) trackTime = time;
	      
	  }
      
    notifyTracksAdded(trackTime);
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

  /**
   * Returns the track with this name 
   */
  public Track getTrackByName(String name)
  {
    Track track;
    for(Enumeration e = tracks.elements(); e.hasMoreElements();)
      {
	track = (Track)e.nextElement();
	if(track.getName().equals(name)) return track;
      }
    return null;
  }

  /**
   * Adds a track
   */
    /*public void addTrack(Track track)
      {
      track.setId(trackId++);
      tracks.addElement(track);
      
      messVect.removeAllElements();
      messVect.addElement(track.getName());
      messVect.addElement("anything");
      
      sendMessage(FtsObject.systemInlet, "track_add", messVect);
      
      notifyTrackAdded(track);
      }*/

  /**
   * Remove a Track from this sequencer 
   */
  public void removeTrack(Track track)
  {
    if(track==null) return;
    tracks.removeElement(track);

    sendArgs[0].setObject((FtsTrackObject)track.getTrackDataModel()); 
    sendMessage(FtsObject.systemInlet, "track_remove", 1, sendArgs);

    notifyTrackRemoved(track);
  }

  public void changeTrack(Track track)
  {
    notifyTrackChanged(track);
  }

    public void requestTrackCreation(String type)
    {
	sendArgs[0].setString(type); 
	sendMessage(FtsObject.systemInlet, "track_add", 1, sendArgs);
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
    //MaxVector messVect = new MaxVector();
  String name = new String("unnamed"); //to be assigned by FTS, usually via a specialized KEY

  //unic id for a track, starting from zero;
  private int trackId = 0; 
  static final int REMOTE_TRACK_ADD = 0; 
}


