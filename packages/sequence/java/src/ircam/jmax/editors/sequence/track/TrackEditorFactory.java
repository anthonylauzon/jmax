
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.toolkit.*;

/**
 * The interface of Factories for editors of Tracks
 */
public interface TrackEditorFactory {
    public TrackEditor newEditor(Track track, Geometry geometry);
    public int getWeight();
}
