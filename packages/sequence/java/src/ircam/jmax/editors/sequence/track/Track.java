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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.beans.*;
import javax.swing.*;

/**
 * The interface offered by a Track: essentially, its TrackDataModel and 
 * property support. 
 * 
 * A property change support is present in this class 
 * to handle generic properties needed by the clients of this structure.
 */ 

public interface Track {


    /**
     * Get the TrackDataModel associated to this Track object. */
    public abstract TrackDataModel getTrackDataModel();

    /**
     * Sets a property for this track (ex. muted, active, other). This interface
     * does not define which properties are allowed for a track, since they can
     * be different from track to track, or can be associated to its use: for
     * example, a Track in a graphic editor would need some graphic properties that
     * a track player would ignore (and vice-versa).
     * An object that wants to listen to a generic property change of this
     * object can use the  PropertyChangeSupport provided via the getPropertySupport() method.
     * @param name the name of the property
     * @param value the value to set
     */
    public abstract void setProperty(String name, Object value);

    /**
     * Returns the value of the given property. See the setProperty method for details.*/
    public abstract Object getProperty(String name);

    /**
     * Returns the PropertyChangeSupport object associated to this track.
     * Clients can add specific listeners for specific properties as in
     * track.getPropertySupport().addChangeListener(myChangeListener); .*/
    public PropertyChangeSupport getPropertySupport();
}

