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

package ircam.jmax.editors.sequence.track;


import ircam.jmax.fts.*;
import ircam.jmax.editors.sequence.*;

/**
 * The interface of the objects that can be put in a sequence. They have a starting time   
 * and a value that can be of different types - ex. Ambitus notes, silences, fricatives, etc... 
 * The value field is an FtsRemoteData. 
 * The setProperty and getProperty methods are used to assign parameters without knowing 
 * the type of FtsRemoteData this event contains.
 * The same structure is reflected on the server side. */
public interface Event 
{
    /**
     * Get the initial time for this event */
    public abstract double getTime();

    /**
     * This is the method that must be called by the editors to
     * change the initial time of an event. It takes care of
     * keeping the data base consistency */
    public abstract void move(double time);
    /**
     * Set the time for this event */
    public abstract void setTime(double time);

    /**
     * Set the named property */
    public abstract void setProperty(String name, Object value);

    /**
     * Get the given property */
    public abstract Object getProperty(String name);

    /**
     * Returns the value of this event as an FtsRemoteData */
    public abstract EventValue getValue();

    /** Set the FtsData corresponding to this event */
    public abstract void setValue(EventValue value);

    public abstract boolean isHighlighted();

    final public static int SELECTED    = 0;
    final public static int DESELECTED  = 1;
    final public static int HIGHLIGHTED = 2;
}





