
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;
import javax.swing.*;

/**
 * Creates new MidiTrack editors. */
public class MonodimensionalTrackEditorFactory implements TrackEditorFactory {
    
    public TrackEditor newEditor(Track track, Geometry geometry)
    {
	return new MonodimensionalTrackEditor(geometry, track);
    }

    public int getWeight()
    {
	return MONODIMENSIONAL_EDITOR_WEIGHT;
    }

    //-- Fields
    static int MONODIMENSIONAL_EDITOR_WEIGHT = 4;
    public static MonodimensionalTrackEditorFactory instance = new MonodimensionalTrackEditorFactory();
}
