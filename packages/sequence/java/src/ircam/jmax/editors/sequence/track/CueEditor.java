
package ircam.jmax.editors.sequence.track;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * An editor for Cue values. */
public class CueEditor extends ValueEditor
{
    /**
     * Begin editing the given event */
    public void startEdit(TrackEvent event)
    {

	CueDialog.popupDialog(event);
    }

    /**
     * Returns a static instance of this editor */
    public static CueEditor getInstance()
    {
	return instance;
    }

    //--- CueEditor fields
    static CueEditor instance = new CueEditor();
    static CueDialog cueDialog;
}
