package ircam.jmax.editors.sequence.track;

import java.awt.*;
import ircam.jmax.editors.sequence.*;

/**
 * The functionalities of an editor of track data. */
public interface TrackEditor {

    /**
     * Returns the awt component used during the edit.*/
    public Component getComponent();
    /**
     * Returns the graphic context for this editor */
    public SequenceGraphicContext getGraphicContext();
}
