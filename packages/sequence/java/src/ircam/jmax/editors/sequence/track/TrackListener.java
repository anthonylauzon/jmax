
package ircam.jmax.editors.sequence.track;

import java.awt.event.*;

/**
 * Notify the adding/removing of tracks into a sequencer */
public interface TrackListener
{
    public abstract void trackAdded(Track track);   
    public abstract void tracksAdded(int maxTime);   
    public abstract void trackRemoved(Track track);   
    public abstract void trackChanged(Track track);   
    public abstract void trackMoved(Track track, int oldPosition, int newPosition);   
}
