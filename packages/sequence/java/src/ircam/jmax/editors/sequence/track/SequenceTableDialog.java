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
import javax.swing.*;
import java.util.*;
import java.awt.event.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class SequenceTableDialog extends JDialog implements TrackDataListener/*, TrackListListener, PopupProvider, ListContainer*/{
    
    SequenceTableDialog(Track track, Frame frame, SequenceGraphicContext gc)
    {
	super(frame, "table dialog: track <"+track.getName()+">", false);
	this.frame = frame;
	this.track = track;

	TrackTableModel tabModel = new TrackTableModel(track.getTrackDataModel());
	tabPanel = new SequenceTablePanel(tabModel, gc);

	getContentPane().add(tabPanel);
	
	track.getTrackDataModel().addListener(this);

	setLocation(200, 200);
	Dimension dim = tabPanel.getSize();
	
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	getContentPane().validate();
	validate();
	pack();
    }

    /**
     * TrackDataListener interface
     */
    public void objectChanged(Object spec, String propName, Object propValue){}
      
    public void objectAdded(Object spec, int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	//pack();
    }
      
    public void objectsAdded(int maxTime) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	//pack();
    }
    
    public void objectDeleted(Object whichObject, int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	//pack();
    }

    public void trackCleared() 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	//pack();
    }
    
    public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
    public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex){}
    public void trackNameChanged(String oldName, String newName){};

    Track track;
    SequenceTablePanel tabPanel;
    Frame frame;
}





