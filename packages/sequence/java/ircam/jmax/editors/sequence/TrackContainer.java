//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.JMaxApplication;

/**
* A graphic component that contains a single track editor*/
public class TrackContainer extends JPanel {
	
  public TrackContainer(Track t, TrackEditor trackEditor)
{
    super();
    setLayout(new BorderLayout());
    
    this.track = t;
    this.trackEditor = trackEditor;
    
    trackIndex = ((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).getTrackIndex(t);
    activationButton = new JToggleButton(""+trackIndex);
    activationButton.setMargin(new Insets(0, 0, 0, 0));
    activationButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 70));
		
    /////////////////////////
    openButton = new JButton(SequenceImages.getImageIcon("opened_track"));
    openButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 20));
    openButton.setMinimumSize(new Dimension(BUTTON_WIDTH, 20));
    openButton.setMaximumSize(new Dimension(BUTTON_WIDTH, 20));
    openButton.setToolTipText("close");
		
    activeButton = new JButton(SequenceImages.getImageIcon("unmute"));
    activeButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 14));
    activeButton.setMaximumSize(new Dimension(BUTTON_WIDTH, 14));
    activeButton.setMinimumSize(new Dimension(BUTTON_WIDTH, 14));
    activeButton.setToolTipText("active/inactive");
		
    JPanel bp = new JPanel();
    bp.setLayout(new BoxLayout(bp, BoxLayout.Y_AXIS));
    bp.add(openButton);
    bp.add(activeButton);
		
    buttonPanel = new JPanel();
    buttonPanel.setLayout(new BorderLayout());
    buttonPanel.add(bp, BorderLayout.NORTH);
    buttonPanel.add(activationButton, BorderLayout.CENTER);
		
    add(buttonPanel, BorderLayout.WEST);
		
    add(trackEditor.getComponent(), BorderLayout.CENTER);
		
    toggleBar = new ToggleBar(trackEditor, trackIndex);
    add(toggleBar, BorderLayout.NORTH);
    toggleBar.setVisible(false);
		
		
    setPreferredSize(new Dimension(getPreferredSize().width, trackEditor.getDefaultHeight()));
    setMaximumSize(new Dimension(getMaximumSize().width, trackEditor.getDefaultHeight()));
    
    // --- set the "active" property of the track when the button is pressed
    activationButton.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
		{
				track.setProperty("selected", Boolean.TRUE);
		}
    });
		
    openButton.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		{
				track.setProperty("opened", Boolean.FALSE);
		}
		});
		
    activeButton.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		{
				track.getFtsTrack().requestSetActive(!active);
		}
		});
		
    // --- change the selected state of the button when the "active" property of the track changes
    track.getPropertySupport().addPropertyChangeListener( new ActiveListener(activationButton));
    track.getPropertySupport().addPropertyChangeListener( toggleBar);
    // an inner class
		
    validate();
}


/**
* Returns the button used to activate this track.
 */
public AbstractButton getActivationButton()
{
	return activationButton;
}

public void changeIndex(int pos)
{
	trackIndex = pos;
	activationButton.setText(""+trackIndex);
	toggleBar.changeIndex(trackIndex);
}

/**
* A listener of the "active" property of a track. Its role
 * is to switch the state of the activation button when
 * the active state changes 
 */
class ActiveListener implements PropertyChangeListener {
	
	public ActiveListener(AbstractButton b)
	{
		this.b = b;
	}
	
	public void propertyChange(PropertyChangeEvent evt)
	{
		boolean sel = false;	    
		boolean opened = true;	    
		String name = evt.getPropertyName();
		
		if (name.equals("selected"))
	  {
			sel = ((Boolean) evt.getNewValue()).booleanValue();
			
			if(sel) b.setForeground(Color.green);
			else b.setForeground(Color.gray);
			
			b.setSelected(sel);
	  }
		else if (name.equals("opened"))
	  {
			opened = ((Boolean) evt.getNewValue()).booleanValue();
			
			int height;
			if(opened)
				height = trackEditor.getDefaultHeight();
			else
				height = ToggleBar.TOGGLEBAR_HEIGHT+4;
			
			setPreferredSize(new Dimension(getPreferredSize().width, height));
			setMaximumSize(new Dimension(getMaximumSize().width, height));
			
			trackEditor.getComponent().setVisible(opened);
			buttonPanel.setVisible(opened);
			toggleBar.setVisible(!opened);
			
			if(opened)
				((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).changeTrack(track);
	  }
		else 
			if(name.equals("active"))
			{
	      active = ((Boolean) evt.getNewValue()).booleanValue();
	      if(active)
					activeButton.setIcon(SequenceImages.getImageIcon("unmute"));
	      else
					activeButton.setIcon(SequenceImages.getImageIcon("mute"));
			}
	  else if( name.equals("viewMode"))
		{
			if(((Integer)evt.getNewValue()).intValue() < 2)//MidiTrack only
				resizeToRange();
		}
		else if(name.equals("rangeMode"))
			resizeToRange();
		/*else 
			if(name.equals("maximumValue") || name.equals("minimumValue"))
			((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).changeTrack(track);*/
	}
	
	AbstractButton b;
}

void resizeToRange()
{
	int height = ((PartitionAdapter)trackEditor.getGraphicContext().getAdapter()).getRangeHeight();
	setSize(getSize().width, height);
	setPreferredSize(new Dimension(getPreferredSize().width, height));
	setMaximumSize(new Dimension(getMaximumSize().width, height));
	((FtsSequenceObject)trackEditor.getGraphicContext().getFtsObject()).changeTrack(track);	  
}

public TrackEditor getTrackEditor()
{
	return trackEditor;
}

public Track getTrack()
{
	return track;
}

//--- Fields
TrackEditor trackEditor;
Track track;
JToggleButton activationButton;
JButton openButton;
JButton activeButton;
ToggleBar toggleBar;
JPanel buttonPanel; 
int trackIndex;
boolean active = true;
public static final int BUTTON_WIDTH = 25;
}






