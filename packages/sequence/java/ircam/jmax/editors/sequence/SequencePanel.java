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

import ircam.jmax.editors.sequence.tools.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.undo.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.beans.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.Geometry;

/**
* The graphic component containing the tracks of a Sequence.
 */
public class SequencePanel extends PopupToolbarPanel implements SequenceEditor, TrackListener, TrackDataListener, ListSelectionListener, ScrollManager, Serializable{
	
  FtsSequenceObject ftsSequenceObject;
  SequenceDataModel sequenceData;
  transient EditorContainer itsContainer;
  transient public SequenceRuler ruler;
	transient public TempoBar tempoBar;
  
	transient ListSelectionListener markersSelectionListener = null;
	transient SequenceSelection currentMarkersSelection = null;
	
  Box trackPanel;
	transient JPanel centerSection;
	transient JScrollPane scrollTracks;
  transient Hashtable trackContainers = new Hashtable();
  transient MutexPropertyHandler mutex = new MutexPropertyHandler("selected");
  //---
  transient JLabel itsZoomLabel;
  transient JScrollBar itsTimeScrollbar;
  transient Geometry geometry;
  transient SequenceToolManager manager;
  
  public final int INITIAL_ZOOM = 20;
  public static final int MINIMUM_TIME = 10000;
  
  static public Color violetColor = new Color(102, 102, 153);
  static public Font rulerFont = new Font("SansSerif", Font.PLAIN, 10);
	
	transient EmptySequencePopupMenu popup = null;
	
  transient Component verticalGlue = Box.createVerticalGlue();
  /**
		* Constructor based on a SequenceDataModel containing the tracks to edit.
   */
  public SequencePanel(EditorContainer container, SequenceDataModel data) 
  {  
    itsContainer = container;
    sequenceData = data;
    setDoubleBuffered(false);
    ftsSequenceObject = (FtsSequenceObject)data;
    ftsSequenceObject.addTrackListener(this);
		
    //Create abd init a Geometry object for this sequencer 
    {
			geometry = new Geometry();
			geometry.setXZoom(20);
			geometry.setYZoom(300);
			geometry.setYInvertion(true);
			geometry.setYTransposition(136);
    }
    //------------------------------------------------
    // Create the ruler
    ruler = new SequenceRuler(geometry, this);
		
    //-------------------------------------------------
    //- Create the ToolManager with the needed tools
    
    manager = new SequenceToolManager( SequenceTools.instance);
    Tool arrow = manager.getToolByName("edit");     
    manager.activate(arrow, null); //we do not have a gc yet...
		
    //------------------- prepare the track panel:
    trackPanel = new Box(BoxLayout.Y_AXIS);
		
    scrollTracks = new JScrollPane(trackPanel, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
																	 JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    
    trackPanel.add(verticalGlue);
		
    setLayout(new BorderLayout());
		
    JPanel separate_tracks = new JPanel();
    separate_tracks.setLayout(new BorderLayout());
		
		//------------------ prepares TempoBar
		Dimension td = new Dimension(SequenceWindow.DEFAULT_WIDTH+TrackContainer.BUTTON_WIDTH, TempoBar.TEMPO_HEIGHT);
		tempoBar = new TempoBar(geometry, ftsSequenceObject, this);
		tempoBar.setSize(SequenceWindow.DEFAULT_WIDTH+TrackContainer.BUTTON_WIDTH, TempoBar.TEMPO_HEIGHT);
		tempoBar.setPreferredSize(td);
		tempoBar.setMinimumSize(td);
		
		centerSection = new JPanel();			
		Border border = scrollTracks.getBorder();
		scrollTracks.setBorder(BorderFactory.createEmptyBorder());
		centerSection.setBorder(border);
		centerSection.setLayout( new BorderLayout());
		centerSection.add( scrollTracks, BorderLayout.CENTER);
		
		separate_tracks.add( centerSection, BorderLayout.CENTER);
	    
    //------------------ prepares Ruler     
    Box northSection = new Box(BoxLayout.Y_AXIS);

    ruler.setSize(SequenceWindow.DEFAULT_WIDTH + TrackContainer.BUTTON_WIDTH, 20);
    ruler.setPreferredSize(new Dimension(SequenceWindow.DEFAULT_WIDTH+TrackContainer.BUTTON_WIDTH, 20));
		northSection.add(ruler);
		    	
    separate_tracks.add(northSection, BorderLayout.NORTH);
		
    //---------- prepares the time zoom listeners
    geometry.addZoomListener( new ZoomListener() {
			public void zoomChanged(float zoom, float oldZoom)
			{
				repaint();
				TrackEvent lastEvent = sequenceData.getLastEvent();
				if(lastEvent!=null)
					resizePanelToTimeWithoutScroll((int)(lastEvent.getTime()+
																							 ((Double)lastEvent.getProperty("duration")).intValue()));
			}
		});
		
		geometry.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
		  {	  
				String name = e.getPropertyName();
				if( name.equals("gridMode"))
				{
					int grid = ((Integer) e.getNewValue()).intValue();
					if( grid == MidiTrackEditor.MEASURES_GRID)
					{
						centerSection.add( tempoBar, BorderLayout.NORTH);
						revalidate();
					}
					else
					{
						centerSection.remove( tempoBar);
						revalidate();
					}
				}
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
			}
    });
		
    separate_tracks.add(itsTimeScrollbar, BorderLayout.SOUTH);
    add(separate_tracks, BorderLayout.CENTER);
    validate();
  
		markersSelectionListener = new ListSelectionListener(){
			public void valueChanged(ListSelectionEvent e)
			{
				SequenceSelection sel = ftsSequenceObject.getMarkersSelection();
				if( sel.size() > 0)
				{
					TrackEvent evt = (TrackEvent) sel.getSelected().nextElement();
					makeVisible(evt);
				}
			}
		};
		
	scrollTracks.addMouseListener(new MouseListener(){
			public void mouseClicked(MouseEvent e){}
			public void mousePressed(MouseEvent e){	
				if(sequenceData.trackCount() == 0)
					SequencePanel.this.processMouseEvent(e);
			}
			public void mouseReleased(MouseEvent e){}
			public void mouseEntered(MouseEvent e)
		  {
				requestFocus();
			}
			public void mouseExited(MouseEvent e){}
		});
	}
	
	public JPopupMenu getMenu()
	{	
		if(popup == null)
			createPopup();
		
		popup.update();
		return popup;
	}
	
	void createPopup()
	{
		popup = new EmptySequencePopupMenu(this);
	}
	
	/**
		* Callback from the model. This is called when a new track is added, but also
	 * as a result of a merge */
  public void trackAdded(Track track)
  {    
    TrackEditor teditor = TrackEditorFactoryTable.newEditor(track, geometry, true);
    teditor.getGraphicContext().setToolManager(manager);
    teditor.getGraphicContext().setFrame(itsContainer.getFrame());
    teditor.getGraphicContext().setScrollManager(this);
    teditor.setContainer(this);
    
    manager.addContextSwitcher(new ComponentContextSwitcher(teditor.getComponent(), teditor.getGraphicContext()));
    
    trackPanel.remove(verticalGlue);
    TrackContainer trackContainer = new TrackContainer(track, teditor);
    trackContainer.setBorder(new EtchedBorder()); 
		
    trackPanel.add(trackContainer);
    trackPanel.add(verticalGlue);
    
    trackPanel.validate();
    scrollTracks.validate();
    scrollTracks.getVerticalScrollBar().setValue(scrollTracks.getVerticalScrollBar().getMaximum());
		
    mutex.add(track);
    trackContainers.put(track, trackContainer);
		
		teditor.getSelection().addListSelectionListener(this);
		track.setProperty("selected", Boolean.TRUE);
			
		track.getTrackDataModel().addListener(this);    
		track.getTrackDataModel().addHighlightListener(ruler);    		
		track.getTrackDataModel().addTrackStateListener(new TrackStateListener(){
				public void lock(boolean lock){}
				public void active(boolean active){}
				public void restoreEditorState(FtsTrackEditorObject editorState){};
				public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
				{
					currentMarkersSelection = markersSelection;
					currentMarkersSelection.addListSelectionListener( markersSelectionListener);
				}
				public void updateMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
				{				
					currentMarkersSelection.removeListSelectionListener( markersSelectionListener);
					currentMarkersSelection = markersSelection;
					if( currentMarkersSelection != null)
						currentMarkersSelection.addListSelectionListener( markersSelectionListener);
				}
        public void ftsNameChanged(String name){}
			});		
		
		//resize the frame //////////////////////////////////////////////////////////////
		int height;	
    boolean toPack = false;
		Dimension dim = itsContainer.getFrame().getSize();
		if(dim.height < SequenceWindow.MAX_HEIGHT)
		{
			int tcHeight = trackContainer.getSize().height;
			
			if(sequenceData.trackCount() == 1)
      {
				itsContainer.getFrame().setSize(dim.width, SequenceWindow.EMPTY_HEIGHT + tcHeight);
        toPack = true;
			}
      else
				if(dim.height + tcHeight <= SequenceWindow.MAX_HEIGHT)
        {
					itsContainer.getFrame().setSize(dim.width, dim.height + tcHeight);
          toPack = true;
        }
        else 
				if(dim.height < SequenceWindow.MAX_HEIGHT)
					itsContainer.getFrame().setSize(dim.width, SequenceWindow.MAX_HEIGHT);
		}
			
		///////////////////////////////////////////////////////////////////////////////////
		//updates events in track
		
		for(Enumeration e = track.getTrackDataModel().getEvents(); e.hasMoreElements();)
			teditor.updateNewObject((TrackEvent)e.nextElement());
		
		itsContainer.getFrame().validate();
    if(toPack)
      itsContainer.getFrame().pack();
  }
	
  public void tracksAdded(int maxTime)
  {
    if(maxTime>0)
      resizePanelToTimeWithoutScroll(maxTime);
  }
	
  /**
		* Callback from the model. It can be called when two tracks are merged into one */
  public void trackRemoved(Track track)
  {
    TrackContainer trackContainer = (TrackContainer) trackContainers.get(track);
    
    trackPanel.remove(trackContainer);
    
    trackContainer.getTrackEditor().dispose();
    
    trackPanel.validate();
    trackPanel.repaint();
    scrollTracks.validate();
    
    mutex.remove(track);
    
    trackContainers.remove(track);
    
    //resize of the frame
    Dimension dim = itsContainer.getFrame().getSize();
    if(sequenceData.trackCount() == 0)
      itsContainer.getFrame().setSize(dim.width, SequenceWindow.EMPTY_HEIGHT);	
    else
      if(!scrollTracks.getVerticalScrollBar().isVisible())		
				itsContainer.getFrame().setSize(dim.width, 
																				SequenceWindow.EMPTY_HEIGHT+ruler.getSize().height+21+getAllTracksHeight());	
    
    itsContainer.getFrame().validate();
    
    resetTrackIndexs();
  }
	
  /**
		* Callback from the model. It can be called when a track changed */
  public void trackChanged(Track track)
  {
    TrackContainer trackContainer = (TrackContainer) trackContainers.get(track);
    trackContainer.validate();
    trackPanel.validate();
    scrollTracks.validate();
  }
  
  public void trackMoved(Track track, int oldPosition, int newPosition)
  {
    resetTrackIndexs();
  }
  public void ftsNameChanged(String name){}
	
  void resetTrackIndexs()
  {
    Component[] comp = trackPanel.getComponents();  
    for(int i = 0; i < comp.length-1; i++)
      ((TrackContainer)comp[i]).changeIndex(i);
  }
  
  public int getAllTracksHeight()
  {
    int height = 0;
    for(Enumeration e = trackContainers.elements(); e.hasMoreElements();)
      height += ((TrackContainer)e.nextElement()).getSize().height;
    return height;
  }
  
  boolean isVisible(int y)
  {
    Rectangle r = scrollTracks.getViewport().getViewRect();
    return ((y >= r.y)&&(y <= r.y +r.height));
  }
  
  public void moveTrackTo(Track track, int pos)
  {
    TrackContainer trackContainer = (TrackContainer) trackContainers.get(track);
    trackPanel.remove(trackContainer);
    trackPanel.add(trackContainer, pos);
    trackPanel.validate();
    scrollTracks.validate();
    scrollTracks.getVerticalScrollBar().setValue(trackContainer.getBounds().y);
    track.setProperty("selected", Boolean.TRUE);
  }
  
  /**
		* called when the database is changed: DataTrackListener interface
   */
  boolean uploading = false;
  public void objectChanged(Object spec, String propName, Object propValue) {}
  public void objectAdded(Object spec, int index) 
  {
    if( !uploading)
      resizePanelToEventTime((TrackEvent)spec);	
  }
  public void objectsAdded(int maxTime) 
  {
    if( !uploading)
      resizePanelToTime(maxTime);	
  }
  public void objectDeleted(Object whichObject, int index){}
  public void trackCleared(){}
  public void startTrackUpload(TrackDataModel track, int size)
  {
    uploading = true;
  }
  public void endTrackUpload(TrackDataModel track)
  {
    uploading = false;
		if( track.length() > 0)
			resizePanelToEventTimeWithoutScroll( track.getLastEvent());
	}
  public void startPaste(){}
  public void endPaste(){}
  public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex)
  {
    resizePanelToEventTime((TrackEvent)whichObject);
  }	
  //controll if the object is in the actual scrollable area. if not extend the area
  private void resizePanelToEventTime(TrackEvent evt)
  {
    int evtTime = (int)(evt.getTime()) + ((Double)evt.getProperty("duration")).intValue();
    resizePanelToTime(evtTime);
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
  
  //******** Merge reintroduction *******************//
  public void Merge(){
    ValueInfo type;
    Track track;
    boolean first = true;
    TrackDataModel firstTrackModel = null;
    
    int result = JOptionPane.showConfirmDialog( this,
																								"Merging tracks is not Undoable.\nOK to merge ?",
																								"Warning",
																								JOptionPane.YES_NO_OPTION,
																								JOptionPane.WARNING_MESSAGE);
    if (result == JOptionPane.OK_OPTION)
		{
			for(Enumeration e = ftsSequenceObject.getTypes(); e.hasMoreElements();)
			{
				type = (ValueInfo) e.nextElement();
				first = true;
				for (Enumeration en = ftsSequenceObject.getTracks(type); en.hasMoreElements();)
	      {
					track = (Track) en.nextElement();				
					if((track.getProperty("mute") == null)||!((Boolean)track.getProperty("mute")).booleanValue())
					{
						if(first) 
						{
							firstTrackModel = track.getTrackDataModel();
							reinitTrackEditor(track);
							first = false;
						}
						else
						{				
							firstTrackModel.mergeModel(track.getTrackDataModel());
							ftsSequenceObject.requestTrackRemove(track);
						}
					}
	      }
			}
			
		}	
  }
  
  public void reinitTrackEditor(Track track)
  {
    ((TrackContainer) trackContainers.get(track)).getTrackEditor().reinit();	
  }
	
  public void removeActiveTrack()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
      sequenceData.removeTrack(track);
  }
	
  public Track getCurrentTrack()
  {
    return mutex.getCurrent();
  }
  
  public TrackEditor getCurrentTrackEditor()
  {
    Track current = mutex.getCurrent();
    if(current != null)
      return ((TrackContainer) trackContainers.get(current)).getTrackEditor();
    else
      return null;
  }
	
  public Frame getFrame(){
    return itsContainer.getFrame();
  }
	
  public FtsSequenceObject getFtsSequenceObject()
  {
    return ftsSequenceObject;
  }
	
	void updateCurrentMarkers()
	{
		
	}
	
  ////////////////////////////////////////////////////////////
  //------------------- Editor interface ---------------
  public void copy()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
    {
      ((ClipableData) track.getTrackDataModel()).copy();
			
      if(JMaxApplication.getProperty("no_menus") == null)
        Actions.pasteAction.setEnabled(true);
    }
  }
	
  public void cut()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
    {
      ((ClipableData) track.getTrackDataModel()).cut();
      if(JMaxApplication.getProperty("no_menus") == null)
        Actions.pasteAction.setEnabled(true);
    }
  }
  
  public void paste()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
      ((ClipableData) track.getTrackDataModel()).paste();
  }
  
  public void duplicate()
  {
    copy();
    paste();
  }
	
  public void undo()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
      try 
			{
				((UndoableData) track.getTrackDataModel()).undo();
			} catch (CannotUndoException e1) {
				System.out.println("Can't undo");
			}
  }
	
  public void redo()
  {
    Track track = mutex.getCurrent();
    if(track!=null)
      try 
			{
				((UndoableData) track.getTrackDataModel()).redo();
			} catch (CannotRedoException e1) {
				System.out.println("Can't redo");
			}
  }
	
  public FtsGraphicObject getFtsObject()
  {
    return ftsSequenceObject;
  }
	
  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
	
  public void close(boolean doCancel){
    itsContainer.getFrame().setVisible(false);
    ftsSequenceObject.requestDestroyEditor(); 
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsContainer);
  }
  public void save(){}
  public void saveAs(){}
  public void print()
  {
    SequenceWindow win = (SequenceWindow)itsContainer;
		
    RepaintManager.currentManager(win).setDoubleBufferingEnabled(false);
		
    PrintJob aPrintjob = win.getToolkit().getPrintJob( win, "Printing Sequence", null, null);
		
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
    int numSelected = SequenceSelection.getCurrent().size();
		
    if (numSelected == 1)
		{
			TrackEvent evt = (TrackEvent)SequenceSelection.getCurrent().getSelected().nextElement();
			makeVisible(evt);
		}
		
		if(itsContainer instanceof SequenceWindow && (JMaxApplication.getProperty("no_menus") == null))
		{
			SequenceWindow window = (SequenceWindow)itsContainer;
			window.getEditMenu().copyAction.setEnabled(numSelected > 0);
			window.getEditMenu().cutAction.setEnabled(numSelected > 0);
			window.getEditMenu().duplicateAction.setEnabled(numSelected > 0);
		}
  }
	
  public boolean eventIsVisible(Event evt)
  {
    int time = (int)evt.getTime();
		int duration = ((Double)evt.getProperty("duration")).intValue();
		int startTime = -geometry.getXTransposition(); 
		int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
		return ((time>startTime)&&(time+duration<endTime));
	}
	
  /**************** ScrollManager Interface ********************************/
  
  ///////AUTOMATIC SCROLLING  
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















