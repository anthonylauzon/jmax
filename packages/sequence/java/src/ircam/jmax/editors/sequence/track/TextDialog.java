
package ircam.jmax.editors.sequence.track;

import javax.swing.*;
import java.awt.event.*;

/**
 * The dialog associated to the StringValue class.
 * This dialog sets the "text" property of the client TrackEvent.
 * */

class TextDialog extends JDialog {
    
    /**
     * A TextDialog should not be constructed directly: use the static method popupDialog() instead*/
    private TextDialog()
    {
	super();
	
	JPanel panel = new JPanel();
	
	panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
	panel.add(new JLabel("Text  "));
	
	textEditField = new JTextField();
	
	/**
	 * An action is generated at Carriage Return */
	textEditField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e)
		{
		    dismissDialog();
		}
	});
	
	panel.add(textEditField);
	getContentPane().add(panel);	 
	
	setLocation(200, 200);
	setSize(250, 55);
    }
    
    
    public static void popupDialog(TrackEvent event)
    {
	editingEvent = event; 
	//remember this event. 
	//It will be used to set its "text" property in the dismiss() call
	
	textEditField.setText((String)event.getProperty("text"));
	instance.setVisible(true);
    }
    
    private void dismissDialog()
    {
	editingEvent.setProperty("text", textEditField.getText());
	setVisible(false);
    }
    
    //--- TextDialog fields
    static TrackEvent editingEvent;
    static JTextField textEditField;
    static TextDialog instance = new TextDialog();
}
