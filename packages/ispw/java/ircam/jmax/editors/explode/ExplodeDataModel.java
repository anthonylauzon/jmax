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


package ircam.jmax.editors.explode;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.util.Enumeration;

/**
 * the functionalities of the model for the explode data.
 * ExplodeRemoteData implements it.
 */
public interface ExplodeDataModel {

  /**
   * how many events in the database?
   */
  public abstract int length();
  

  /**
   * returns an enumeration of all the events
   */
  public abstract Enumeration getEvents();
  

  /**
   * returns a given event
   */
  public abstract ScrEvent getEventAt(int index);

  /**
   * return the index of the given event, if it exists, or the error constants
   * NO_SUCH_EVENT, EMPTY_COLLECTION */
  public abstract int indexOf(ScrEvent event);

  public abstract Enumeration intersectionSearch(int start, int end);

  public abstract Enumeration inclusionSearch(int start, int end);

  public abstract int getFirstEventAt(int time);
 
  /**
   * adds an event in the database
   */
  public abstract void addEvent(ScrEvent theEvent);

  /**
   * generic change of an event in the database.
   * Call this function to signal the parameters changing of the event, except
   * the initial time and the duration parameters. Use moveEvent and resizeEvent instead
   */
  public abstract void changeEvent(ScrEvent event);

  /**
   * move an event in the database
   */
  public abstract void moveEvent(ScrEvent event, int time);


  /**
   * deletes an event from the database
   */
  public abstract void deleteEvent(ScrEvent theEvent);


  /**
   * requires to be notified when the database changes
   */
  public abstract void addListener(ExplodeDataListener theListener);

  /**
   * removes the listener
   */
  public abstract void removeListener(ExplodeDataListener theListener);


}




