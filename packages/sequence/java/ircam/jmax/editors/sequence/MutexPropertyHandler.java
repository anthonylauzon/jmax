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

import java.beans.*;
import java.util.*;
import ircam.jmax.editors.sequence.track.*;

/**
 * A class that coordinates a group of tracks on
 * a given boolean property, in such a way that when this property
 * is set to true on one track, it will be set to false into the others */
public class MutexPropertyHandler {

    /**
     * Create a new Mutually exclusive group on the given property name */
    public MutexPropertyHandler(String propertyName)
    {
	name = propertyName;
    }


    /**
     * Adds a client to the group */
    public void add(Track track)
    {
	//-- add a listener to this new client

	PropertyChangeListener listener = new PropertyChangeListener() {

	    public void propertyChange (PropertyChangeEvent e)
	    {
		Track temp;
		PropertyChangeListener pcl;

		if (!e.getPropertyName().equals("selected")) return;
		
		/** HACK! the ignore_callback flag is here just to avoid endless loops */
		if (ignore_callbacks) return;

		// the property changed for our client track. 
		// Prepare to set the opposite value into the other
		// tracks.
		Boolean opposite = ((e.getNewValue()).equals(Boolean.TRUE))?Boolean.FALSE:Boolean.TRUE;
		
		ignore_callbacks = true;
		
		for (Enumeration enum = clients.keys(); enum.hasMoreElements();)
		{
		    temp = (Track) enum.nextElement();
		    pcl = (PropertyChangeListener) clients.get(temp);
		    
		    if(pcl != this) 
		       temp.setProperty(name, opposite);
		    else current = temp;
		    
		}
		/** HACK! the ignore_callback flag is here just to avoid endless loops */
		ignore_callbacks = false;

	    }
	};
	
	track.getPropertySupport().addPropertyChangeListener(listener);
	clients.put(track, listener);
    }

    /**
     * Remove a client */
    public void remove(Track track)
    {
	PropertyChangeListener listener = (PropertyChangeListener) clients.get(track);
	track.getPropertySupport().removePropertyChangeListener(listener);

	clients.remove(track);

	if(current==track) current=null;
    }

    public Track getCurrent()
    {
	return current;
    }
 

    //--- Fields
    String name;
    Track current = null;
    Hashtable clients = new Hashtable();
    boolean ignore_callbacks = false;
}
