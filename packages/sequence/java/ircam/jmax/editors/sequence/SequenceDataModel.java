//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 


package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.util.Enumeration;

/**
 * The model of data for a sequence: a set of Tracks with the
 * usual add, remove and access Track methods.
 */
public interface SequenceDataModel {

  /**
   * how many tracks?
   */
  public abstract int trackCount();
  
    /**
     * Returns the i-th track */
  public abstract Track getTrackAt(int i);

  public abstract int getTrackIndex(Track track);

  public abstract Enumeration getTracks();
  /**
   * adds a Track to this sequencer
   */
    //public abstract void addTrack(Track track);

  /**
   * deletes an event from the database
   */
  public abstract void removeTrack(Track track);


  public TrackEvent getLastEvent();

  /**
   * requires to be notified when this database changes
   */
  public abstract void addTrackListener(TrackListener theListener);

  /**
   * removes the listener
   */
  public abstract void removeTrackListener(TrackListener theListener);

  public abstract String getFtsName();
}




