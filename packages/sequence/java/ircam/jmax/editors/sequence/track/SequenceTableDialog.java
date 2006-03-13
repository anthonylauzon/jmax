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
    super(frame, false);
    this.frame = frame;
    this.track = trk;
    this.sgc = gc;
		
		/* upper section: Events List */
    TrackTableModel eventsModel = new TrackTableModel(track.getTrackDataModel());
    eventsPanel = new SequenceTablePanel(eventsModel, gc, gc.getSelection());
    
		/* lower section: Measures List */
		if( gc.getMarkersTrack()!=null)
			addMarkersAndSplit();
		else
			getContentPane().add(eventsPanel);
    
    track.getTrackDataModel().addListener(this);
    
		track.getTrackDataModel().addTrackStateListener(new TrackStateListener(){
				public void lock(boolean lock){}
				public void active(boolean active){}
				public void restoreEditorState(FtsTrackEditorObject editorState){};
				public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
				{
					SequenceTableDialog.this.hasMarkers = true; 
				}
				public void updateMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
				{				
					removeMarkersAndSplit();
					if( sgc.getMarkersTrack()!=null)
						addMarkersAndSplit();
				}
        public void ftsNameChanged(String name){setDialogTitle();}
			});		
		
    track.getTrackDataModel().addListener( new TrackDataListener() {
			public void objectChanged(Object spec, int index, String propName, Object propValue){}
			public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}
			public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}
			public void objectAdded(Object whichObject, int index){}
			public void objectsAdded(int maxTime){}
			public void objectDeleted(Object whichObject, int oldIndex){}
			public void trackCleared(){}
			public void startTrackUpload( TrackDataModel track, int size){}
			public void endTrackUpload( TrackDataModel track)
			{
				if( SequenceTableDialog.this.hasMarkers && sgc.getMarkersTrack()!=null && markersPanel == null)
				{
					getContentPane().remove(eventsPanel);
					addMarkersAndSplit();
					validate();
					pack();
				}
				else 
					if( sgc.getMarkersTrack() == null && markersPanel != null)
						removeMarkersAndSplit();
			}
			public void startPaste(){}
			public void endPaste(){}
      public void startUndoRedo(){}
      public void endUndoRedo(){}
    });
		
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
        getPanel().table.requestFocus();
			}
			public void componentMoved(ComponentEvent e)
      {
        getPanel().table.requestFocus();
      }
		});
    
    addWindowListener( new WindowAdapter() {
			public void windowOpened(WindowEvent e)
      {
        getPanel().table.requestFocus();
      }
			public void windowActivated(WindowEvent e)
      {
        getPanel().table.requestFocus();
      }
		});
    addMouseListener( new MouseAdapter() {
			public void mouseEntered(MouseEvent e)
      {
        getPanel().table.requestFocus();
      }
			public void mousePressed(MouseEvent e)
      {
        getPanel().table.requestFocus();
      }
		});
  
		int tab_w = ((FtsTrackObject)track.getTrackDataModel()).editorObject.tab_w;
		int tab_h = ((FtsTrackObject)track.getTrackDataModel()).editorObject.tab_h;
		if( tab_w != -1 && tab_h != -1)
			setSize(tab_w, tab_h);
	
    setDialogTitle();
  }

  public void setDialogTitle()
  {
    if( track.getTrackDataModel().isInSequence())
    {
      SequenceDataModel seq = ((SequenceDataModel)((FtsTrackObject)track.getTrackDataModel()).getParent());
      setTitle("table track n."+seq.getTrackIndex(track)+" in sequence "+seq.getFtsName());
    }
    else
      setTitle("table track "+track.getTrackDataModel().getFtsName());    
  }
  
	private void addMarkersAndSplit()
	{
		FtsTrackObject markersTrack = sgc.getMarkersTrack();
		TrackTableModel markersModel = null;
		markersModel = new TrackTableModel( (TrackDataModel)markersTrack);
		markersPanel = new SequenceTablePanel(markersModel, sgc, sgc.getMarkersSelection());
		
		splitPane = new JSplitPane( JSplitPane.VERTICAL_SPLIT, eventsPanel, markersPanel);
		splitPane.setOneTouchExpandable( true);
		splitPane.setDividerLocation( DEFAULT_HEIGHT/2);
		splitPane.setPreferredSize( new Dimension( 650, DEFAULT_HEIGHT));
			
		getContentPane().add( splitPane);
	}

	private void removeMarkersAndSplit()
	{
		splitPane.remove(eventsPanel);
		getContentPane().remove(splitPane);
		getContentPane().add(eventsPanel);
		splitPane = null;
		markersPanel = null;
		SequenceTableDialog.this.hasMarkers = false;
		validate();
		pack();    
	}
    /**
     * TrackDataListener interface
     */
  public void objectChanged(Object spec, int index, String propName, Object propValue){}
  public void objectAdded(Object spec, int index){}
  public void objectsAdded(int maxTime){}
    
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
  public void startUndoRedo(){}
  public void endUndoRedo(){}
  public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}

	public SequenceTablePanel getPanel()
	{
		return eventsPanel;
	}
	
	boolean hasMarkers = false;
  Track track;
	SequenceGraphicContext sgc;
  SequenceTablePanel eventsPanel, markersPanel;
	JSplitPane splitPane;
  Frame frame;
	final static int DEFAULT_HEIGHT = 500;
}





