package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
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

    public Track getTrack();

    public JPopupMenu getMenu();

    public int getDefaultHeight();

    public void dispose();
    public SequenceSelection getSelection();

    public void updateNewObject(Object obj);

    public void reinit();

    public void showListDialog();    
}


