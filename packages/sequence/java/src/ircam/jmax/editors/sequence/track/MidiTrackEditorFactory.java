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
import javax.swing.*;
import ircam.jmax.toolkit.*;

/**
 * Creates new MidiTrack editors. */
public class MidiTrackEditorFactory implements TrackEditorFactory {
    
    public TrackEditor newEditor(Track track, Geometry geometry)
    {
	/*MidiTrackEditor toReturn = new MidiTrackEditor(geometry, track);
	  toReturn.setComponent(new JScrollPane(toReturn, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER));
	  return toReturn;*/
	return new MidiTrackEditor(geometry, track);
    }

    public int getWeight()
    {
	return MIDI_EDITOR_WEIGHT;
    }

    //-- Fields
    static int MIDI_EDITOR_WEIGHT = 5;
    public static MidiTrackEditorFactory instance = new MidiTrackEditorFactory();
}
