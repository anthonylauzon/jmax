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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.tools.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.undo.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

  /**
   * The graphic component containing the tracks of a Sequence.
   */
public class SequencePanel extends JPanel implements Editor, TrackListener, TrackDataListener, ListSelectionListener, ScrollManager {
    
    FtsSequenceObject ftsSequenceObject;

    EditorToolbar toolbar;
    SequenceDataModel sequenceData;
    EditorContainer itsContainer;
    public InfoPanel statusBar;
    public SequenceRuler ruler;
    
    Box trackPanel;
    JScrollPane scrollTracks;
    
    Hashtable trackContainers = new Hashtable();
    MutexPropertyHandler mutex = new MutexPropertyHandler("active");
    //---
    JLabel itsZoomLabel;
    JScrollBar itsTimeScrollbar;
    Geometry geometry;
    ToolManager manager;

    public final int INITIAL_ZOOM = 20;
    public static final int MINIMUM_TIME = 10000;

    static public Color violetColor = new Color(102,102,153);
    static public Font rulerFont = new Font("SansSerif", Font.PLAIN, 10);

    Component verticalGlue = Box.createVerticalGlue();
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
    //-- Tools, toolbar and status bar:
    //- Create the ToolManager with the needed tools
    //- Create a toolbar associated to this ToolManager
    //- Create a status bar containing the toolbar
    
    manager = new ToolManager(SequenceTools.instance);
    toolbar = new EditorToolbar(manager, EditorToolbar.HORIZONTAL);
    toolbar.setSize(/*156*//*180*//*204*/228, 25);    
    toolbar.setPreferredSize(new Dimension(/*156*//*180*//*204*/228, 25));    
    Tool arrow = manager.getToolByName("arrow");     
    manager.activate(arrow, null); //we do not have a gc yet...
    //------------------- prepare the track panel:
    trackPanel = new Box(BoxLayout.Y_AXIS);
    scrollTracks = new JScrollPane(trackPanel, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
				   JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    
    trackPanel.add(verticalGlue);

    //ftsSequenceObject.requestTrackCreation("noteevt");

    setLayout(new BorderLayout());

    JPanel separate_tracks = new JPanel();
    separate_tracks.setLayout(new BorderLayout());

    separate_tracks.add(scrollTracks, BorderLayout.CENTER);
    
    //------------------ prepares the Status bar    
    Box northSection = new Box(BoxLayout.Y_AXIS);
    
    statusBar = new InfoPanel();

    manager.addToolListener(new ToolListener() {
	public void toolChanged(ToolChangeEvent e) 
	    {
		
		if (e.getTool() != null) 
		    {
			statusBar.post(e.getTool(), "");
		    }
	    }
    });
 
    statusBar.setSize(300, 30);

    JPanel toolbarPanel = new JPanel();
    toolbarPanel.setSize(/*156*//*180*//*204*/228, 25);
    toolbarPanel.setPreferredSize(new Dimension(/*156*//*180*//*204*/228, 25));
    toolbarPanel.setLayout(new BorderLayout());
    toolbarPanel.add(toolbar, BorderLayout.CENTER);
    toolbarPanel.validate();
    statusBar.addWidgetAt(toolbarPanel, 2);
    statusBar.validate();

    ruler.setSize(300, 20);

    northSection.add(statusBar);
    northSection.add(ruler);	
    separate_tracks.add(northSection, BorderLayout.NORTH);

    itsContainer.getFrame().validate();
    itsContainer.getFrame().pack();

    itsContainer.getFrame().setVisible(true);
    
    //---------- prepares the time zoom listeners
    geometry.addZoomListener( new ZoomListener() {
	public void zoomChanged(float zoom)
	    {
		statusBar.post(manager.getCurrentTool(),"zoom "+((int)(zoom*100))+"%");
		repaint();
		TrackEvent lastEvent = sequenceData.getLastEvent();
		if(lastEvent!=null)
		    resizePanelToTimeWithoutScroll((int)(lastEvent.getTime()+
							 ((Double)lastEvent.getProperty("duration")).intValue()));
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
  }

    /**
     * Callback from the model. This is called when a new track is added, but also
     * as a result of a merge */
    public void trackAdded(Track track)
    {
	if(track.getName().equals("untitled"))
	    track.getFtsTrack().requestSetName("untitled"+getCurrentUntitledTrackIndex());

	TrackEditor teditor = TrackEditorFactoryTable.newEditor(track, geometry);
	teditor.getGraphicContext().setToolManager(manager);
	teditor.getGraphicContext().setFrame(itsContainer.getFrame());
	teditor.getGraphicContext().setScrollManager(this);
	manager.addContextSwitcher(new ComponentContextSwitcher(teditor.getComponent(), teditor.getGraphicContext()));

	trackPanel.remove(verticalGlue);
	TrackContainer trackContainer = new TrackContainer(track, teditor);
	trackContainer.setBorder(new EtchedBorder()); 
	trackContainer.setMaximumSize(new Dimension(trackContainer.getMaximumSize().width, teditor.getDefaultHeight()));//?????

	trackPanel.add(trackContainer);
	trackPanel.add(verticalGlue);

	trackPanel.validate();
	scrollTracks.validate();
	scrollTracks.getVerticalScrollBar().setValue(scrollTracks.getVerticalScrollBar().getMaximum());

	mutex.add(track);
	trackContainers.put(track, trackContainer);

	teditor.getSelection().addListSelectionListener(this);//????
	track.setProperty("active", Boolean.TRUE);

	//added to update maximum time if needed
	track.getTrackDataModel().addListener(this);    
	//add sequenceRuler as highlighting listener 
	track.getTrackDataModel().addHighlightListener(ruler);    
    
	//resize the frame
	int height;	
	Dimension dim = itsContainer.getFrame().getSize();	
	if(dim.height < Sequence.MAX_HEIGHT)
	{
	    if(sequenceData.trackCount() == 1)
		itsContainer.getFrame().
		    setSize(dim.width, Sequence.EMPTY_HEIGHT + ruler.getSize().height + 21 + trackContainer.getSize().height);
	    else
		if(dim.height + trackContainer.getSize().height <= Sequence.MAX_HEIGHT)
		    itsContainer.getFrame().setSize(dim.width, dim.height + trackContainer.getSize().height);
		else 
		    if(dim.height < Sequence.MAX_HEIGHT)
			itsContainer.getFrame().setSize(dim.width, Sequence.MAX_HEIGHT);
	}

	//updates events in track
	for(Enumeration e = track.getTrackDataModel().getEvents(); e.hasMoreElements();)
	  teditor.updateNewObject((TrackEvent)e.nextElement());
    }

    public int getCurrentUntitledTrackIndex()
    {
	int index = 0;	
	while(sequenceData.getTrackByName("untitled"+index) != null)
	    index++;

	return index;
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
	     itsContainer.getFrame().setSize(dim.width, Sequence.EMPTY_HEIGHT);	
	else
	    if(!scrollTracks.getVerticalScrollBar().isVisible())		
		itsContainer.getFrame().setSize(dim.width, 
						Sequence.EMPTY_HEIGHT+ruler.getSize().height+21+getAllTracksHeight());	
    }

    /**
     * Callback from the model. It can be called when a track changed */
    public void trackChanged(Track track)
    {
	TrackContainer trackContainer = (TrackContainer) trackContainers.get(track);
	trackContainer.validate();
	trackPanel.validate();
	scrollTracks.validate();

	Rectangle absBounds = SwingUtilities.convertRectangle(trackContainer, trackContainer.getBounds(), trackPanel);
	scrollTracks.getViewport().scrollRectToVisible(absBounds);
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
	track.setProperty("active", Boolean.TRUE);
    }

   /**
     * called when the database is changed: DataTrackListener interface
     */
    
    public void objectChanged(Object spec, String propName, Object propValue) {}
    public void objectAdded(Object spec, int index) 
    {
	resizePanelToEventTime((TrackEvent)spec);	
    }
    public void objectsAdded(int maxTime) 
    {
	resizePanelToTime(maxTime);	
    }
    public void objectDeleted(Object whichObject, int index){}
    public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
    {
	resizePanelToEventTime((TrackEvent)whichObject);
    }
    public void trackNameChanged(String oldName, String newName){}

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

  ////////////////////////////////////////////////////////////
  public void Copy()
  {
      Track track = mutex.getCurrent();
      if(track!=null)
	  ((ClipableData) track.getTrackDataModel()).copy();
  }

  public void Cut()
  {
      Track track = mutex.getCurrent();
      if(track!=null)
	  ((ClipableData) track.getTrackDataModel()).cut();
  }

  public void Paste()
  {
      Track track = mutex.getCurrent();
      if(track!=null)
	  ((ClipableData) track.getTrackDataModel()).paste();
  }

  public void Duplicate()
  {
    Copy();
    Paste();
  }

  public void Undo()
  {
      Track track = mutex.getCurrent();
      if(track!=null)
	  try 
	      {
		  ((UndoableData) track.getTrackDataModel()).undo();
	      } catch (CannotUndoException e1) {
		  System.out.println("can't undo");
	      }
  }

  public void Redo()
  {
      Track track = mutex.getCurrent();
      if(track!=null)
	  try 
	      {
		  ((UndoableData) track.getTrackDataModel()).redo();
	      } catch (CannotRedoException e1) {
		  System.out.println("can't redo");
	      }
  }

    public void Settings(){
	//itsScrPanel.settings();
    }

    /*public void Merge(){
      new MergeDialog(sequenceData, geometry);
      }*/
    
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

    public Frame getFrame(){
	return itsContainer.getFrame();
    }

    public FtsSequenceObject getFtsSequenceObject()
    {
	return ftsSequenceObject;
    }
  //------------------- Editor interface ---------------
  final public Fts getFts()
  {
    return MaxApplication.getFts();
  }
    
    public MaxDocument getDocument()
    {
	return ftsSequenceObject.getDocument();
    }

  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
  public void Close(boolean doCancel){
    itsContainer.getFrame().setVisible(false);
    ftsSequenceObject.closeEditor();
  }
    
    /**
     * ListSelectionListener interface
     */    
    public void valueChanged(ListSelectionEvent e)
    {
	if (SequenceSelection.getCurrent().size()==1)
	    {
		TrackEvent evt = (TrackEvent)SequenceSelection.getCurrent().getSelected().nextElement();
		makeVisible(evt);
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

    /////////////////ScrollManager Interface
    public void scrollIfNeeded(int time)
    {
	resizePanelToTime(time);
    }
    public void scrollToValue(int value)
    {
	itsTimeScrollbar.setValue(value);
    }
}















