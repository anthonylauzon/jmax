
package ircam.jmax.editors.sequence.track;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * An editor for String values. These values are expressed in the form of a String expression 
 * on the inputs: this editor just pops up a text dialog to enter the text.*/
public class StringEditor extends ValueEditor
{
    /**
     * Begin editing the given event */
    public void startEdit(TrackEvent event)
    {

	FormulaDialog.popupDialog(event);
    }

    /**
     * Returns a static instance of this editor */
    public static StringEditor getInstance()
    {
	return instance;
    }

    //--- StringEditor fields
    static StringEditor instance = new StringEditor();
    static FormulaDialog formulaDialog;
}
