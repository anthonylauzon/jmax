//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

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
