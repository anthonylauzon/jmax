
package ircam.jmax.editors.sequence.track;

import javax.swing.*;
import java.awt.event.*;

/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "expression" property of the client TrackEvent.
 * */

class FormulaDialog extends JDialog {
    
    /**
     * A FormulaDialog should not be constructed directly: use the static method popupDialog() instead*/
    private FormulaDialog()
    {
	super();
	
	JPanel panel = new JPanel();
	
	panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
	panel.add(new JLabel("formula  "));
	
	formulaEditField = new JTextField();
	
	/**
	 * An action is generated at Carriage Return */
	formulaEditField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e)
		{
		    dismissDialog();
		}
	});
	
	panel.add(formulaEditField);
	getContentPane().add(panel);	 
	
	setLocation(200, 200);
	setSize(250, 55);
    }
    
    
    public static void popupDialog(TrackEvent event)
    {
	editingEvent = event; 
	//remember this event. 
	//It will be used to set its "expression" property in the dismiss() call
	
	formulaEditField.setText((String)event.getProperty("expression"));
	instance.setVisible(true);
    }
    
    private void dismissDialog()
    {
	editingEvent.setProperty("expression", formulaEditField.getText());
	setVisible(false);
    }
    
    //--- FormulaEditor fields
    static TrackEvent editingEvent;
    static JTextField formulaEditField;
    static FormulaDialog instance = new FormulaDialog();
}
