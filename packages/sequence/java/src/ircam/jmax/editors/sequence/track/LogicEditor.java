
package ircam.jmax.editors.sequence.track;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * An editor for logic values. These values are expressed in the form of a logic expression 
 * on the inputs: this editor just pops up a text dialog to enter a formula.*/
public class LogicEditor extends ValueEditor
{
    /**
     * Begin editing the given event */
    public void startEdit(TrackEvent event)
    {

	FormulaDialog.popupDialog(event);
    }

    /**
     * Returns a static instance of this editor */
    public static LogicEditor getInstance()
    {
	return instance;
    }

    //--- LogicEditor fields
    static LogicEditor instance = new LogicEditor();
    static FormulaDialog formulaDialog;
}
