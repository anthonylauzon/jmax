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
 * SequenceRemoteData offers support for undo and clipboard operations.
 */
public class SequenceRemoteData extends FtsRemoteUndoableData implements SequenceDataModel
{

  /**
   * constructor.
   */
  public SequenceRemoteData()
  {
    super();
      
    listeners = new MaxVector();
  }


  /**
   * how many events in the data base?
   */

  public int trackCount()
  {
      return tracks.size();
  }


    public String getName()
    {
	return name;
    }

    /**
     * Returns the i-th track */
    public Track getTrackAt(int i)
    {
	return (Track) tracks.elementAt(i);
    }

  /**
   * Adds a track
   */
  public void addTrack(Track track)
  {
      tracks.addElement(track);
      for (Enumeration e=listeners.elements(); e.hasMoreElements();)
	  {
	      ((TrackListener)(e.nextElement())).trackAdded(track);
	  }
  }

  /**
   * Remove a Track from this sequencer 
   */
  public void removeTrack(Track track)
  {
      tracks.removeElement(track);
      for (Enumeration e=listeners.elements(); e.hasMoreElements();)
	  {
	      ((TrackListener)(e.nextElement())).trackRemoved(track);
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


  /* The method inherited from FtsRemoteData */

    public void call( int key, FtsStream stream)
      throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
    {
      switch( key) {
	//to be implemented
    }
  }


  //----- Fields
  /** Key for remote call add */
  

    Vector tracks = new Vector();
    MaxVector listeners = new MaxVector();
    String name = new String("unnamed"); //to be assigned by FTS, usually via a specialized KEY
}


