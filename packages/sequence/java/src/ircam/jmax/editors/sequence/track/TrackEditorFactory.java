
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;

/**
 * The interface of Factories for editors of Tracks
 */
public interface TrackEditorFactory {
    public TrackEditor newEditor(Track track, Geometry geometry);
    public int getWeight();
}
