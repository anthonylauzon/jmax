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

class SequenceTableDialog extends JDialog implements TrackDataListener{
    
  SequenceTableDialog(Track trk, Frame frame, SequenceGraphicContext gc)
  {
    super(frame, "table dialog: track <"+trk.getName()+">", false);
    this.frame = frame;
    this.track = trk;
    
		/* upper section: Events List */
    TrackTableModel eventsModel = new TrackTableModel(track.getTrackDataModel());
    eventsPanel = new SequenceTablePanel(eventsModel, gc, gc.getSelection());
    
		/* lower section: Measures List */
		FtsTrackObject markersTrack = gc.getMarkersTrack();
		TrackTableModel markersModel = null;
		if( markersTrack!=null)
		{
			markersModel = new TrackTableModel( (TrackDataModel)markersTrack);
			markersPanel = new SequenceTablePanel(markersModel, gc, gc.getMarkersSelection());
		
			splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT, eventsPanel, markersPanel);
			splitPane.setOneTouchExpandable(true);
			splitPane.setDividerLocation(150);
			
			getContentPane().add( splitPane);
		}
		else
			getContentPane().add(eventsPanel);
    
    track.getTrackDataModel().addListener(this);
    
    setLocation(200, 200);
    Dimension dim = eventsPanel.getSize();
    
    if(dim.height+30 > DEFAULT_HEIGHT) dim.height = DEFAULT_HEIGHT;
    else dim.height += 30;
    setSize(dim);
    
    getContentPane().validate();
    validate();
    pack();		
		
		addComponentListener( new ComponentAdapter() {
			public void componentResized(ComponentEvent e)
			{
				Rectangle bounds = SequenceTableDialog.this.getBounds();
				((FtsTrackObject)track.getTrackDataModel()).editorObject.setTableSize(bounds.width, bounds.height);
			}
			public void componentMoved(ComponentEvent e){}
		});
  
		int tab_w = ((FtsTrackObject)track.getTrackDataModel()).editorObject.tab_w;
		int tab_h = ((FtsTrackObject)track.getTrackDataModel()).editorObject.tab_h;
		if( tab_w != -1 && tab_h != -1)
			setSize(tab_w, tab_h);
	}

    /**
     * TrackDataListener interface
     */
  public void objectChanged(Object spec, String propName, Object propValue){}
      
  public void objectAdded(Object spec, int index) 
  {
    if( !uploading)
      {
			getContentPane().validate();
			eventsPanel.validate();
			validate();
      }
  }
      
  public void objectsAdded(int maxTime) 
  {
    getContentPane().validate();
    eventsPanel.validate();
    validate();
  }
    
  public void objectDeleted(Object whichObject, int index) 
  {
    getContentPane().validate();
    eventsPanel.validate();
    validate();
  }

  public void trackCleared() 
  {
    getContentPane().validate();
    eventsPanel.validate();
    validate();
  }
  boolean uploading = false;
  public void startTrackUpload( TrackDataModel track, int size)
  {
    uploading = true;
  }    
  public void endTrackUpload( TrackDataModel track)
  {
    uploading = false;
  }    
  public void startPaste(){}    
  public void endPaste(){}    
  public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex){}
  public void trackNameChanged(String oldName, String newName){};

	public SequenceTablePanel getPanel()
	{
		return eventsPanel;
	}
	
  Track track;
  SequenceTablePanel eventsPanel, markersPanel;
	JSplitPane splitPane;
  Frame frame;
	final static int DEFAULT_HEIGHT = 600;
}





