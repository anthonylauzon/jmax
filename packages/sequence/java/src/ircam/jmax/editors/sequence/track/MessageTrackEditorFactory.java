
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;
import javax.swing.*;
import ircam.jmax.toolkit.*;
/**
 * Creates new MidiTrack editors. */
public class MessageTrackEditorFactory implements TrackEditorFactory {
    
    public TrackEditor newEditor(Track track, Geometry geometry)
    {
	return new MessageTrackEditor(geometry, track);
    }

    public int getWeight()
    {
	return MONODIMENSIONAL_EDITOR_WEIGHT;
    }

    //-- Fields
    static int MONODIMENSIONAL_EDITOR_WEIGHT = 4;
    public static MessageTrackEditorFactory instance = new MessageTrackEditorFactory();
}
