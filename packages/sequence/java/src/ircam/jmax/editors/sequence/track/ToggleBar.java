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

import java.awt.*;
import java.io.*;
import java.beans.*;
import java.awt.event.*;
import javax.swing.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.JMaxApplication;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.editors.sequence.renderers.*;
/**
 **/
public class ToggleBar extends ircam.jmax.toolkit.PopupToolbarPanel implements PropertyChangeListener
{
    public ToggleBar(TrackEditor trkEd, int index)
    {
	super();
	trackEditor = trkEd;
	track = trackEditor.getTrack();
	trackIndex = index;
	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	openButton = new JButton(SequenceImages.getImageIcon("closed_track"));
	openButton.setToolTipText("open");
	openButton.setPreferredSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	openButton.setMaximumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	openButton.setMinimumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));

	activeButton = new JButton(SequenceImages.getImageIcon("unmute"));
 	activeButton.setToolTipText("active/inactive");
	activeButton.setPreferredSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	activeButton.setMaximumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	activeButton.setMinimumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));

	barButton = new JButton();
	barButton.setHorizontalAlignment(AbstractButton.LEFT);
	barButton.setForeground(Color.darkGray);
	barButton.setMargin(new Insets(0, 7, 0, 100));
	barButton.setFont(toggleBarFont);
	barButton.setText(trackIndex+" - "+track.getName());
	barButton.setToolTipText("open");
	barButton.setPreferredSize(new Dimension(TOGGLEBAR_DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT));

	add(openButton);
	add(activeButton);
	add(barButton);

	barButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		    track.setProperty("selected", Boolean.TRUE);
		}
	});
	openButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		    track.setProperty("selected", Boolean.TRUE);
		}
	});

	activeButton.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent e)
		{
		    track.getFtsTrack().requestSetActive(!active);
		}
	    });

	barButton.addMouseListener(new MouseListener(){
	     public void mousePressed(MouseEvent e)
		 {
		     if (e.isPopupTrigger()) 
			 {
			     ToggleBar.this.doPopup(e);
			 }
		 }
	     public void mouseClicked(MouseEvent e){}
	     public void mouseReleased(MouseEvent e){}
	     public void mouseEntered(MouseEvent e){}
	     public void mouseExited(MouseEvent e){}
	});

	validate();

	track.getTrackDataModel().addListener(new TrackDataListener(){
		public void objectChanged(Object spec, String propName, Object propValue) {}
		public void objectAdded(Object spec, int index){}
		public void objectsAdded(int maxTime){}
		public void objectDeleted(Object whichObject, int index){}
		public void trackCleared(){}
		public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
		public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex){}
		public void trackNameChanged(String oldName, String newName)
		{
		    barButton.setText(trackIndex+" - "+newName);
		}
	    });
    }

    void doPopup(MouseEvent e)
    {
	processMouseEvent(e);
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(TOGGLEBAR_DEFAULT_WIDTH, TOGGLEBAR_HEIGHT);
    }

    public JPopupMenu getMenu()
    {
	ClosedTrackPopupMenu.getInstance().update(trackEditor);
	return ClosedTrackPopupMenu.getInstance();
    }

    public void changeIndex(int index)
    {
	trackIndex = index;
	barButton.setText(trackIndex+" - "+track.getName());
    }

    // propertyChangeListener interface
    public void propertyChange(PropertyChangeEvent evt)
    {
      String name = evt.getPropertyName();
      
      if(name.equals("active"))
      {
	  active = ((Boolean) evt.getNewValue()).booleanValue();
	  if(active)
	      activeButton.setIcon(SequenceImages.getImageIcon("unmute"));
	  else
	      activeButton.setIcon(SequenceImages.getImageIcon("mute"));
      }
    }
    public static Font toggleBarFont = new Font("monospaced", Font.PLAIN, 10);
    boolean opened = true;
    boolean active = true;
    JButton openButton, barButton, activeButton;
    public static int TOGGLEBAR_HEIGHT = 14; 
    public static int TOGGLEBAR_DEFAULT_WIDTH = 1500; 
    int trackIndex;
    Track track;
    TrackEditor trackEditor;
}













