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


package ircam.jmax.editors.bpf;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.util.Enumeration;

/**
 * The generic model for the data contained in a Track.
 * A Track is defined as a sequence of Event objects (objects with a 
 * starting time and a duration), each one containing an EventValue.
 * Note that a track can contain EventValues of different types. 
 * A TrackDataModel is the database for a Track, and
 * it has the functionalities to add, remove and access events, as well
 * as methods to access subset of elements using time criterias
 * (IntersectionSearch, InclusionSearch methods).
 */
public interface BpfDataModel {

    /**
     * how many events in the database
     */
    public abstract int length();

    /**
     * return the time of the last event
     */
    //public abstract double getMaximumTime();
        
    /**
     * returns an enumeration of all the events
     */
    public abstract Enumeration getPoints();

    /**
     * returns an enumeration of all the events between start and end index
     */
    //public abstract Enumeration getPoints(int startIndex, int endIndex);
        
    /**
     * returns a given event 
     */
    public abstract BpfPoint getPointAt(int index);
    
    /**
     * returns the next in time event 
     */
    public abstract BpfPoint getNextPoint(BpfPoint pnt);
    public abstract BpfPoint getNextPoint(float time);
    /**
     * returns the next in time event 
     */
    public abstract int getPreviousPointIndex(float time);
    public abstract BpfPoint getPreviousPoint(float time);
    public abstract BpfPoint getPreviousPoint(BpfPoint pnt);
    /**
     * return the index of the given event, if it exists, or the error constants
     * NO_SUCH_EVENT, EMPTY_COLLECTION 
     */
    public abstract int indexOf(BpfPoint pnt);
    
    public abstract Enumeration intersectionSearch(float start, float end, BpfAdapter adapter);
    
    //public abstract Enumeration inclusionSearch(float start, float end);
    
    //public abstract int getFirstPointAt(float time);

    public abstract BpfPoint getLastPoint();
    
    /**
     * adds an event in the database
     * @param theEvent the TrackEvent to be added
     */
    public abstract void addPoint(int index, BpfPoint pt);

    /**
     * generic change of an event in the database.
     * Call this function to signal the parameters changing of the event, except
     * the initial time and the duration parameters. Use moveEvent and resizeEvent for that.
     * @param theEvent the TrackEvent to be changed
     * @param propertyName the name of the changed property
     * @param propertyValue the new property value
     */
    //public abstract void changeEvent(TrackEvent event, String popertyName, Object propertyValue);
    
    /**
     * move an event in the database
     * @param theEvent the TrackEvent to be moved
     * @param time the new time tag
     */
    public abstract int movePointTo(int oldIndex, float newTime);
    
    /**
     * deletes an event from the database
     * @param theEvent the TrackEvent to be removed
     */
    public abstract void removePoint(int index);
    /**
     * Deletes all the events 
     */
    public abstract void removeAllPoints();
    
    /**
     * requires to be notified when the database changes
     */
    public abstract void addBpfListener(BpfDataListener theListener);
    
    /**
     * removes the listener
     */
    public abstract void removeBpfListener(BpfDataListener theListener);    
    
    /**
     */
    public abstract float getRange();
    public abstract float getMaximumValue();
    public abstract float getMinimumValue();
    public abstract void setMaximumValue(float max);
    public abstract void setMinimumValue(float min);
}









