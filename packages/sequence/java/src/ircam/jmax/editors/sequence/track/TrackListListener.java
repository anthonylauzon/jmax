
package ircam.jmax.editors.sequence.track;

import java.awt.event.*;
import java.awt.*;

/**
 * Notify the adding/removing of tracks into a sequencer */
public interface TrackListListener
{
    public abstract void eventSelected(int index, int paramIndex, TrackEvent evt);   
    public abstract void eventDeselected(int index, TrackEvent evt);   
    public abstract void showEvent(int index, TrackEvent evt, Rectangle r);   
}
