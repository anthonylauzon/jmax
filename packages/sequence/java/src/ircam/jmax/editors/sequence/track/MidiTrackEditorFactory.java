
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;
import javax.swing.*;

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
