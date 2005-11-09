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

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.tools.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.undo.*;
import java.beans.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.Geometry;

/**
* The graphic component containing the tracks of a Sequence.
 */
public class TrackPanel extends JPanel implements SequenceEditor, TrackDataListener, ListSelectionListener, ScrollManager, Serializable {
	
  transient FtsTrackObject ftsTrackObject;
	
  transient TrackDataModel trackData;
  transient EditorContainer itsContainer;
  public SequenceRuler ruler;
	public TempoBar tempoBar;
	
	transient ListSelectionListener markersSelectionListener = null;
	transient SequenceSelection currentMarkersSelection = null;
  transient FtsTrackObject markers = null;
	
  transient JScrollPane scrollTracks;
	transient JPanel centerSection, separate_tracks;
  //---
  transient JLabel itsZoomLabel;
  transient JScrollBar itsTimeScrollbar;
  transient Geometry geometry;
  transient SequenceToolManager manager;
  transient TrackBase track;
  transient TrackEditor trackEditor;
	
  public final int INITIAL_ZOOM = 20;
  public static final int MINIMUM_TIME = 10000;
  
  /**
		* Constructor based on a SequenceDataModel containing the tracks to edit.
   */
  public TrackPanel( EditorContainer container, TrackDataModel data) 
  {  
    itsContainer = container;
    trackData = data; 
		
    setDoubleBuffered(false);
    ftsTrackObject = (FtsTrackObject)data;
    
    track = new TrackBase( ftsTrackObject);
    track.setProperty("selected", Boolean.TRUE);
		
    //Create abd init a Geometry object for this sequencer 
    {
      geometry = new Geometry();
      geometry.setXZoom(20);
      geometry.setYZoom(300);
      geometry.setYInvertion(true);
      geometry.setYTransposition(136);
			geometry.setProperty("gridMode", new Integer(TrackEditor.TIME_GRID));
    }
    //------------------------------------------------
    // Create the ruler
    ruler = new SequenceRuler(geometry, this);
		
    //-------------------------------------------------
    //- Create the ToolManager with the needed tools
    manager = new SequenceToolManager( SequenceTools.instance);
    Tool arrow = manager.getToolByName("edit");     
    manager.activate(arrow, null); //we do not have a gc yet...
		
    //------------------- prepare track editor:
    trackEditor = TrackEditorFactoryTable.newEditor( track, geometry, false);
    trackEditor.getGraphicContext().setToolManager( manager);
    trackEditor.getGraphicContext().setFrame( itsContainer.getFrame());
    trackEditor.getGraphicContext().setScrollManager( this);
    trackEditor.getSelection().addListSelectionListener(this);
    trackEditor.setContainer(this);
    SequenceSelection.setCurrent( trackEditor.getSelection());
    data.addListener(this); 
    data.addHighlightListener(ruler);
    manager.addContextSwitcher(new ComponentContextSwitcher( trackEditor.getComponent(), trackEditor.getGraphicContext()));
    manager.contextChanged( trackEditor.getGraphicContext());
    scrollTracks = new JScrollPane( trackEditor.getComponent(), JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
                                    JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    setLayout( new BorderLayout());
    
    //------------------------------------------------
    separate_tracks = new JPanel();
    separate_tracks.setLayout( new BorderLayout());
    
    if( trackData.getType().getPublicName().equals( AmbitusValue.SCOOB_PUBLIC_NAME))
    {
      // Create TempoBar
      tempoBar = new TempoBar(geometry, ftsTrackObject, this);
      
      //------------------ prepares Center Section
      centerSection = new JPanel();			
      scrollTracks.setBorder(new EtchedBorder());
      centerSection.setBorder(new EtchedBorder());
      centerSection.setLayout( new BorderLayout());
      centerSection.add( scrollTracks, BorderLayout.CENTER);
      
      separate_tracks.add( centerSection, BorderLayout.CENTER);
    }
    else
      separate_tracks.add( scrollTracks, BorderLayout.CENTER);
    
    //------------------ prepares ruler & northSection
    Box northSection = new Box(BoxLayout.Y_AXIS); 
    ruler.setSize(SequenceWindow.DEFAULT_WIDTH, 20);		
    northSection.add(ruler);
    
    separate_tracks.add(northSection, BorderLayout.NORTH);
    
    //---------- prepares the time zoom listeners
    geometry.addZoomListener( new ZoomListener() {
      public void zoomChanged(float zoom, float oldZoom)
    {
        repaint();
        double markersTime = -1.0;
        double notesTime = -1.0;
        
        TrackEvent lastMarker = null;
        TrackEvent lastEvent = trackData.getLastEvent();
        if(markers != null) 
        {
          lastMarker = markers.getLastEvent();
          if(lastMarker != null)
            markersTime = lastMarker.getTime() + 100;
        }
        if(lastEvent != null)
          notesTime = lastEvent.getTime() + ((Double)lastEvent.getProperty("duration")).intValue();
        
        if(notesTime > 0.0 || markersTime > 0.0)
          resizePanelToTimeWithoutScroll((int)((notesTime > markersTime) ? notesTime : markersTime));
        
        if(ftsTrackObject.editorObject != null)
          ftsTrackObject.editorObject.setZoom(zoom);
    }
    });
    
    //-------------- prepares the SOUTH scrollbar (time scrolling) and its listener    
    int totalTime = MINIMUM_TIME;
    itsTimeScrollbar = new JScrollBar(Scrollbar.HORIZONTAL, 0, 1000, 0, totalTime);
    itsTimeScrollbar.setUnitIncrement(10);
    itsTimeScrollbar.setBlockIncrement(1000);
    itsTimeScrollbar.addAdjustmentListener(new AdjustmentListener() {	
      public void adjustmentValueChanged(AdjustmentEvent e) {
        int currentTime = e.getValue();
        geometry.setXTransposition(-currentTime);
        if(ftsTrackObject.editorObject != null)
          ftsTrackObject.editorObject.setTransposition(-currentTime);
      }
    });
    separate_tracks.add( itsTimeScrollbar, BorderLayout.SOUTH);
    add( separate_tracks, BorderLayout.CENTER);
				
    validate();
    
    for(Enumeration e = data.getEvents(); e.hasMoreElements();)
      trackEditor.updateNewObject((TrackEvent)e.nextElement());
    
    geometry.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
      public void propertyChange(PropertyChangeEvent e)
    {	  
        String name = e.getPropertyName();
        if( name.equals("gridMode"))
        {
          int grid = ((Integer) e.getNewValue()).intValue();
          if( grid == MidiTrackEditor.MEASURES_GRID)
            centerSection.add( tempoBar, BorderLayout.NORTH);
          else
            centerSection.remove( tempoBar);
          
          /*tempoBar.resetDisplayer();*/
          centerSection.validate();
          TrackPanel.this.validate();
          if(getEditorContainer().getFrame().isVisible())
            getEditorContainer().getFrame().pack();          
        }
    }
    });
    
    markersSelectionListener = new ListSelectionListener(){
      public void valueChanged(ListSelectionEvent e)
    {
        SequenceSelection sel = trackEditor.getGraphicContext().getMarkersSelection();
        if( sel.size() > 0)
        {
          TrackEvent evt = (TrackEvent) sel.getSelected().nextElement();
          makeVisible(evt);
        }
    }
    };
    
    trackData.addTrackStateListener(new TrackStateListener(){
      public void lock(boolean lock){}
      public void active(boolean active){}
      public void restoreEditorState(FtsTrackEditorObject editorState){};
      public void hasMarkers(FtsTrackObject marks, SequenceSelection markersSelection)
      {
        currentMarkersSelection = markersSelection;
        currentMarkersSelection.addListSelectionListener( markersSelectionListener);
        markers = marks;
        markers.addListener(TrackPanel.this);
      }
      public void updateMarkers(FtsTrackObject marks, SequenceSelection markersSelection)
      {			
        currentMarkersSelection.removeListSelectionListener( markersSelectionListener);
        currentMarkersSelection = markersSelection;
        if(markersSelection != null)
          currentMarkersSelection.addListSelectionListener( markersSelectionListener);
        if(markers!=null)
          markers.removeListener(TrackPanel.this);
        markers = marks;
        markers.addListener(TrackPanel.this);
      }
      public void ftsNameChanged(String name){}
    });		
	}
	
  boolean isVisible(int y)
  {
    Rectangle r = scrollTracks.getViewport().getViewRect();
    return ((y >= r.y)&&(y <= r.y +r.height));
  }
	
  public TrackEditor getTrackEditor()
  {
    return trackEditor;
  }
	
  /**
		* called when the database is changed: DataTrackListener interface
   */
  boolean uploading = false;
	
  public void objectChanged(Object spec, int index, String propName, Object propValue) {}
  public void objectAdded(Object spec, int index) 
  {
    if( !uploading)
      resizePanelToEventTime((TrackEvent)spec);
  }
  public void objectsAdded(int maxTime) 
  {
    resizePanelToTime(maxTime);	
  }
  public void objectDeleted(Object whichObject, int index){}
  public void trackCleared(){}
  public void startTrackUpload( TrackDataModel track, int size)
  {
    uploading = true;
  }
  public void endTrackUpload( TrackDataModel track)
  {
    uploading = false;
    if( track.length() > 0)
      resizePanelToEventTimeWithoutScroll( track.getLastEvent());
  }
  public void startPaste(){}
  public void endPaste(){}
  public void startUndoRedo(){}
  public void endUndoRedo(){}
  public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient)
  {
    if(fromClient)
      resizePanelToEventTime((TrackEvent)whichObject);
    else
      resizePanelToEventTimeWithoutScroll((TrackEvent)whichObject);
  }
	
  //controll if the object is in the actual scrollable area. if not extend the area
  private void resizePanelToEventTime(TrackEvent evt)
  {
    if( evt != null)
		{
      int evtTime = 0;
      if( evt.getDataModel() == markers)
        evtTime = (int)evt.getTime() + 10;
      else
        evtTime = (int)(evt.getTime()) + ((Double)evt.getProperty("duration")).intValue();
			resizePanelToTime(evtTime);
		}
  }
	
  private void resizePanelToTime(int time)
  {
    int maxVisibleTime = getMaximumVisibleTime();
    int maximumTime = getMaximumTime();
    
    if(time > maximumTime)
		{
			int delta = maximumTime-itsTimeScrollbar.getMaximum();
			
			itsTimeScrollbar.setMaximum(time-delta);
			itsTimeScrollbar.setValue(time);
		}
    else 
      if( time > maxVisibleTime)		
				itsTimeScrollbar.setValue(time-maxVisibleTime-geometry.getXTransposition()+10);
		else
			if(time < -geometry.getXTransposition())
				itsTimeScrollbar.setValue(time);
  }
	
  private void resizePanelToTimeWithoutScroll(int time)
  {
    int maximumTime = getMaximumTime();
    
    if(time > maximumTime)
		{
			int delta = maximumTime-itsTimeScrollbar.getMaximum();
			itsTimeScrollbar.setMaximum(time-delta);
		}
  }
  private void resizePanelToEventTimeWithoutScroll(TrackEvent evt)
  {
    int evtTime = (int)(evt.getTime()) + ((Double)evt.getProperty("duration")).intValue();
    resizePanelToTimeWithoutScroll(evtTime);
  }
	
  ////////////////////////////////////////////////////////////
  //------------------- SequenceEditor interface ---------------
  public void copy()
  {
    ((ClipableData) trackData).copy();
    Actions.pasteAction.setEnabled(true);
  }
	
  public void cut()
  {
    ((ClipableData) trackData).cut();
    Actions.pasteAction.setEnabled(true);
  }
	
  public void paste()
  {
    ((ClipableData) trackData).paste();
  }
	
  public void duplicate()
  {
    copy();
    paste();
  }
	
  public void undo()
  {
    ftsTrackObject.requestUndo();
  }
                               
  public void redo()
  {
    ftsTrackObject.requestRedo();
  }
  
  public void selectAll()
  {
    trackEditor.getSelection().selectAll();
    trackEditor.getGraphicContext().getGraphicDestination().requestFocus();
	}
	
	public SequenceSelection getSelection()
  {
    return trackEditor.getSelection();
	}	
	
	public SequenceSelection getMarkerSelection()
  {
    return currentMarkersSelection;
	}	
  
  public FtsGraphicObject getFtsObject()
  {
    return ftsTrackObject;
  }
	
  public Frame getFrame()
  {
    return itsContainer.getFrame();
  }
  public EditorContainer getEditorContainer()
  {
    return itsContainer;
  }
  public void close(boolean doCancel)
  {
    itsContainer.getFrame().setVisible(false);
    ftsTrackObject.requestDestroyEditor(); 
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print()
  {
    TrackWindow win = (TrackWindow)itsContainer;
		
    RepaintManager.currentManager(win).setDoubleBufferingEnabled(false);
		
    PrintJob aPrintjob = win.getToolkit().getPrintJob( win, "Printing Track", null, null);
		
    if (aPrintjob != null)
		{
			Graphics aPrintGraphics = aPrintjob.getGraphics();
			
			if (aPrintGraphics != null)
			{
				win.print( aPrintGraphics);
				aPrintGraphics.dispose();
			}
			aPrintjob.end();
		}
    
    RepaintManager.currentManager(win).setDoubleBufferingEnabled(true);
  }
	
  /**
		* ListSelectionListener interface
   */    
  public void valueChanged(ListSelectionEvent e)
  {
    int numSelected = trackEditor.getSelection().size();
		
    if (numSelected == 1)
    {
      TrackEvent evt = (TrackEvent)trackEditor.getSelection().getSelected().nextElement();
      makeVisible(evt);
    }
    
    if(JMaxApplication.getProperty("no_menus") == null)
      ((TrackWindow)itsContainer).getEditMenu().updateMenu(); 
    else
      ((TrackWindow)itsContainer).getSimpleMenu().updateMenu();   
  }
	
  public boolean eventIsVisible(Event evt)
  {
    int time = (int)evt.getTime();
    int duration = ((Double)evt.getProperty("duration")).intValue();
    int startTime = -geometry.getXTransposition(); 
    int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
    return ((time>startTime)&&(time+duration<endTime));
  }
	
  /************ ScrollManager Interface *****************************/
	
  //AUTOMATIC SCROLLING  
  public boolean pointIsVisible(int x, int y)
  {
    Rectangle r = itsContainer.getViewRectangle();
    return ((x > r.x + ScoreBackground.KEYEND) && (x < r.x + r.width - TrackContainer.BUTTON_WIDTH));
  } 
  
  private int scrollingDelta = 10;//the automatic scrolling delta for the scrollbar  
		private int scrolledDelta = 2;//the corresponding graphic delta
			public int scrollBy(int x, int y)
			{
				Rectangle r = itsContainer.getViewRectangle();
				if(x < r.x  + ScoreBackground.KEYEND)
				{
					if(itsTimeScrollbar.getValue()-scrollingDelta >0)
					{
						itsTimeScrollbar.setValue(itsTimeScrollbar.getValue()-scrollingDelta);
						return -scrolledDelta;//scroll to left
					}
					else return 0;//is already scrolled to zero
				}
				else
				{		
					if(x > r.x + r.width - TrackContainer.BUTTON_WIDTH)
					{
						int value = itsTimeScrollbar.getValue()+scrollingDelta;
						if(value>itsTimeScrollbar.getMaximum()-itsTimeScrollbar.getVisibleAmount())
							itsTimeScrollbar.setMaximum(itsTimeScrollbar.getMaximum()+scrollingDelta);
						
						itsTimeScrollbar.setValue(value);
						return scrolledDelta;//scroll to rigth
					}
					else return 0;//the mouse is in the window
				}
			}
			
			public void makeVisible(TrackEvent evt)
			{
				int time = (int)evt.getTime();
				int duration = ((Double)evt.getProperty("duration")).intValue();
				int startTime = -geometry.getXTransposition(); 
				int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
				
				if((time<startTime)||(time+duration>endTime))
					itsTimeScrollbar.setValue(time);
			}
			
			public int getMaximumVisibleTime()
			{
				return geometry.sizeToMsec(geometry, getSize().width - TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
			}
			public int getMaximumTime()
			{
				int maxTransp = -(itsTimeScrollbar.getMaximum()-itsTimeScrollbar.getVisibleAmount());
				int size = getSize().width - TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND;
				
				if (geometry.getXInvertion()) 
					return (int) (maxTransp -(size)/geometry.getXZoom())-1;
				
				else return (int) ((size)/geometry.getXZoom() - maxTransp)-1;
			}
			
			public void scrollIfNeeded(int time)
			{
				resizePanelToTime(time);
			}
			public void scrollToValue(int value)
			{
				itsTimeScrollbar.setValue(value);
			}
			public Rectangle getViewRectangle()
			{
				return scrollTracks.getViewport().getViewRect();
			}
}















