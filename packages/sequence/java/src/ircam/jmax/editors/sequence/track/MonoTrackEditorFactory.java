
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;
import javax.swing.*;

/**
 * Creates new MidiTrack editors. */
public class MonoTrackEditorFactory implements TrackEditorFactory {
    
    public TrackEditor newEditor(Track track, Geometry geometry)
    {
	return new MonoTrackEditor(geometry, track);
    }

    public int getWeight()
    {
	return MONODIMENSIONAL_EDITOR_WEIGHT;
    }

    //-- Fields
    static int MONODIMENSIONAL_EDITOR_WEIGHT = 4;
    public static MonoTrackEditorFactory instance = new MonoTrackEditorFactory();
}
