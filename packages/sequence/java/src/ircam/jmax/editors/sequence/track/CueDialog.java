
package ircam.jmax.editors.sequence.track;

import javax.swing.*;
import java.awt.event.*;

/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class CueDialog extends JDialog {
    
    /**
     * A CueDialog should not be constructed directly: use the static method popupDialog() instead*/
    private CueDialog()
    {
	super();
	
	JPanel panel = new JPanel();
	
	panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
	panel.add(new JLabel("Cue  "));
	
	CueEditField = new JTextField();
	
	/**
	 * An action is generated at Carriage Return */
	CueEditField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e)
		{
		    dismissDialog();
		}
	});
	
	panel.add(CueEditField);
	getContentPane().add(panel);	 
	
	setLocation(200, 200);
	setSize(250, 55);
    }
    
    
    public static void popupDialog(TrackEvent event)
    {
	editingEvent = event; 
	//remember this event. 
	//It will be used to set the "expression" property in the dismiss() call
	
	CueEditField.setText(""+event.getProperty("integer"));
	instance.setVisible(true);
    }
    
    private void dismissDialog()
    {
	int temp = Integer.parseInt(CueEditField.getText());
	editingEvent.setProperty("integer", new Integer(temp));
	setVisible(false);
    }
    
    //--- CueEditor fields
    static TrackEvent editingEvent;
    static JTextField CueEditField;
    static CueDialog instance = new CueDialog();
}
