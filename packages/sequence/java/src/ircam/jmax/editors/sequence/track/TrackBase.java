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

import ircam.jmax.editors.sequence.*;
import java.util.*;
import java.beans.*;

/**
 * A convenience base class for Track implementations. It implements 
 * property handling and property change events.*/
public class TrackBase implements Track{
    
    public TrackBase(TrackDataModel model)
    {
	this.model = model;
    }

    /**
     * Sets a property for this track (ex. muted, active, other). This interface
     * does not define which properties are allowed for a track, since they can
     * be different from track to track, or can be associated to its particular use: for
     * example, a Track in a graphic editor would need some graphic properties that
     * a track player would ignore (and vice-versa). 
     * This method fires a PropertyChangeEvent to the listeners, if the new value is
     * not null*/
    public void setProperty(String name, Object value)
    {
	Object old = properties.get(name);

	if (value == null) 
	    return;
	
	properties.put(name, value);

	propertySupport.firePropertyChange(name, old, value);
    }

    /**
     * Returns the current value of the given property. 
     * The values of properties can be intercepted also as property change events;
     * see the setProperty method for details.*/
    public  Object getProperty(String name)
    {
	return properties.get(name);
    }

    /**
     * Returns the PropertyChangeSupport object associated to this track.
     * Clients can use this object to add specific listeners for specific properties.*/
    public PropertyChangeSupport getPropertySupport()
    {
	return propertySupport;
    }

    /**
     * Returns the data model (the track data) this Track is working on */
    public TrackDataModel getTrackDataModel()
    {
	return model;
    }

    /**
     * Returns the ftsTrackObject this Track is working on */
    public FtsTrackObject getFtsTrack()
    {
	return (FtsTrackObject)model;
    }

    public int getMaximumTime()
    {
	return (int)model.getMaximumTime();
    }

    public String getName()
    {
	return model.getName();
    }
    public void setName(String name)
    {
	//this.name = name;
    }

    public boolean canEditType(ValueInfo info){
	//return model.containsType(info);
	return (model.getType() == info);
    }

    //--- Fields
    Hashtable properties = new Hashtable();
    PropertyChangeSupport propertySupport = new PropertyChangeSupport(this);
    TrackDataModel model;
    //private int id;
    private String name = "";
}






