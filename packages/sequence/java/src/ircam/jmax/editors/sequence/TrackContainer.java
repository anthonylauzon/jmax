
package ircam.jmax.editors.sequence;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.File;
import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.MaxApplication;

/**
 * A graphic component that contains a single track editor*/
class TrackContainer extends JPanel {

    public TrackContainer(Track t, TrackEditor trackEditor)
    {
	super();
	setLayout(new BorderLayout());

	this.track = t;
	this.trackEditor = trackEditor;

	// icons: 
	String fs = File.separator;
	String path = MaxApplication.getProperty("sequencePackageDir")+fs+"images" +fs;

	activationButton = new JToggleButton(new ImageIcon(path+"unselected_track.gif"));
	activationButton.setSelectedIcon(new ImageIcon(path+"selected_track.gif"));
	add(activationButton, BorderLayout.WEST);
	add(trackEditor.getComponent(), BorderLayout.CENTER);
	
	// --- set the "active" property of the track when the button is pressed
	activationButton.addActionListener( new ActionListener() {
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("active", Boolean.TRUE);
		}
	});

	// --- change the selected state of the button when the "active" property of the track changes
	track.getPropertySupport().addPropertyChangeListener( new ActiveListener(activationButton));
	// an inner class
    }


    /**
     * Returns the button used to activate this track.*/
    public AbstractButton getActivationButton()
    {
	return activationButton;
    }

    /**
     * A listener of the "active" property of a track. Its role
     * is to switch the state of the activation button when
     * the active state changes */
    class ActiveListener implements PropertyChangeListener {

	public ActiveListener(AbstractButton b)
	{
	    this.b = b;
	}

	public void propertyChange(PropertyChangeEvent evt)
	{
	    boolean active = false;	    
	    
	    if (evt.getPropertyName().equals("active"))
		active = ((Boolean) evt.getNewValue()).booleanValue();
	    else System.err.println("CHEE? "+evt.getPropertyName());

	    b.setSelected(active);
	}

	AbstractButton b;
    }


    //--- Fields
    TrackEditor trackEditor;
    Track track;
    JToggleButton activationButton;
}
