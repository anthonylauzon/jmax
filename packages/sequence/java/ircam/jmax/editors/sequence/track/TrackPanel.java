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
import ircam.jmax.editors.sequence.tools.*;
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
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.Geometry;

  /**
   * The graphic component containing the tracks of a Sequence.
   */
public class TrackPanel extends JPanel implements SequenceEditor, TrackDataListener, ListSelectionListener, ScrollManager, Serializable {
    
  transient FtsTrackObject ftsTrackObject;

  transient EditorToolbar toolbar;
  transient TrackDataModel trackData;
  transient EditorContainer itsContainer;
  transient public InfoPanel statusBar;
  public SequenceRuler ruler;
    
  transient JScrollPane scrollTracks;
  //---
  transient JLabel itsZoomLabel;
  transient JScrollBar itsTimeScrollbar;
  transient Geometry geometry;
  transient SequenceToolManager manager;
  transient TrackBase track;
  transient TrackEditor trackEditor;
  transient JProgressBar progressBar;

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
    }
    //------------------------------------------------
    // Create the ruler
    ruler = new SequenceRuler(geometry, this);

    //-------------------------------------------------
    //-- Tools, toolbar and status bar:
    //- Create the ToolManager with the needed tools
    //- Create a toolbar associated to this ToolManager
    //- Create a status bar containing the toolbar

    if( ftsTrackObject.getType().getName().equals( AmbitusValue.AMBITUS_NAME) )
      {
	manager = new SequenceToolManager( SequenceTools.scoobInstance);
	toolbar = new EditorToolbar( manager, EditorToolbar.HORIZONTAL);
	toolbar.setSize( 110, 25);    
	toolbar.setPreferredSize( new Dimension( 110, 25));    
      }    
    else
      {
	manager = new SequenceToolManager( SequenceTools.numberInstance);
	toolbar = new EditorToolbar( manager, EditorToolbar.HORIZONTAL);
	toolbar.setSize( 156, 25);    
	toolbar.setPreferredSize( new Dimension( 156, 25));    
      }

    Tool arrow = manager.getToolByName("arrow");     
    manager.activate(arrow, null); //we do not have a gc yet...

    //------------------- prepare track editor:
    trackEditor = TrackEditorFactoryTable.newEditor( track, geometry);
    trackEditor.getGraphicContext().setToolManager( manager);
    trackEditor.getGraphicContext().setFrame( itsContainer.getFrame());
    trackEditor.getGraphicContext().setScrollManager( this);
    trackEditor.getSelection().addListSelectionListener(this);
    SequenceSelection.setCurrent( trackEditor.getSelection());
    data.addListener(this); 
    data.addHighlightListener(ruler);
    manager.addContextSwitcher(new ComponentContextSwitcher( trackEditor.getComponent(), trackEditor.getGraphicContext()));//???
    manager.contextChanged( trackEditor.getGraphicContext());//????
    scrollTracks = new JScrollPane( trackEditor.getComponent(), JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
				    JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
    setLayout( new BorderLayout());

    JPanel separate_tracks = new JPanel();
    separate_tracks.setLayout( new BorderLayout());
    separate_tracks.add( scrollTracks, BorderLayout.CENTER);
    
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
    statusBar.setSize(SequenceWindow.DEFAULT_WIDTH, 30);

    JPanel toolbarPanel = new JPanel();
    toolbarPanel.setSize(228, 25);
    toolbarPanel.setPreferredSize(new Dimension(228, 25));
    toolbarPanel.setLayout(new BorderLayout());
    toolbarPanel.add(toolbar, BorderLayout.CENTER);
    toolbarPanel.validate();
    statusBar.addWidgetAt(toolbarPanel, 2);
      
    JPanel panel = new JPanel();
    panel.setSize( 150, 10);
    panel.setVisible( false);
    statusBar.addWidget( panel);

    progressBar = new JProgressBar( 0, 100);
    progressBar.setPreferredSize(new Dimension(150, 20));
    progressBar.setSize( 150, 20);
    progressBar.setStringPainted( true);
    progressBar.setValue( 0);
    progressBar.setVisible( false);
    statusBar.addWidget( progressBar);

    statusBar.validate();

    ruler.setSize(SequenceWindow.DEFAULT_WIDTH, 20);

    northSection.add(statusBar);
    northSection.add(ruler);	
    separate_tracks.add(northSection, BorderLayout.NORTH);
    //---------- prepares the time zoom listeners
    geometry.addZoomListener( new ZoomListener() {
	public void zoomChanged(float zoom, float oldZoom)
	{
	  statusBar.post(manager.getCurrentTool(),"zoom "+((int)(zoom*100))+"%");
	  repaint();
	  TrackEvent lastEvent = trackData.getLastEvent();
	  if(lastEvent != null)
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
    separate_tracks.add( itsTimeScrollbar, BorderLayout.SOUTH);
    add( separate_tracks, BorderLayout.CENTER);

    validate();
    
    for(Enumeration e = data.getEvents(); e.hasMoreElements();)
      trackEditor.updateNewObject((TrackEvent)e.nextElement());
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

  public void objectChanged(Object spec, String propName, Object propValue) {}
  public void objectAdded(Object spec, int index) 
  {
    if( !uploading)
      resizePanelToEventTime((TrackEvent)spec);	
    progressBar.setValue( index);
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
    progressBar.setMaximum( size);
    progressBar.setValue( 0);
    SwingUtilities.invokeLater( new Runnable(){
      public void run(){
          progressBar.setVisible( true);
      }
    });
  }
  public void endTrackUpload( TrackDataModel track)
  {
    uploading = false;
    if( track.length() > 0)
      resizePanelToEventTimeWithoutScroll( track.getLastEvent());
    SwingUtilities.invokeLater( new Runnable(){
      public void run(){
        progressBar.setVisible( false);
      }
    });
  }
  public void startPaste(){}
  public void endPaste(){}
  public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
  public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex)
  {
    resizePanelToEventTime((TrackEvent)whichObject);
  }
  public void trackNameChanged(String oldName, String newName)
  {
    if( itsContainer instanceof TrackWindow)
        ((TrackWindow)itsContainer).setName( newName);
  }

  //controll if the object is in the actual scrollable area. if not extend the area
  private void resizePanelToEventTime(TrackEvent evt)
  {
    if( evt != null)
      {
	int evtTime = (int)(evt.getTime()) + ((Double)evt.getProperty("duration")).intValue();
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

	if(itsContainer instanceof TrackWindow)
	{
	   TrackWindow window = (TrackWindow)itsContainer;
	   window.getEditMenu().pasteAction.setEnabled(true);
	}

  }

  public void cut()
  {
    ((ClipableData) trackData).cut();

	if(itsContainer instanceof TrackWindow)
	{
	   TrackWindow window = (TrackWindow)itsContainer;
	   window.getEditMenu().pasteAction.setEnabled(true);
	}
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
    try 
      {
	((UndoableData) trackData).undo();
      } catch (CannotUndoException e1) {
	System.out.println("Can't undo");
      }
  }

  public void redo()
  {
    try 
      {
	((UndoableData) trackData).redo();
      } catch (CannotRedoException e1) {
	System.out.println("Can't redo");
      }
  }

  public FtsGraphicObject getFtsObject()
  {
    return ftsTrackObject;
  }

  public EditorToolbar getToolbar()
  {
    return toolbar;
  }

  public StatusBar getStatusBar()
  {
    return statusBar;
  }
  public Frame getFrame(){
    return itsContainer.getFrame();
  }
  public EditorContainer getEditorContainer(){
    return itsContainer;
  }
  public void close(boolean doCancel){
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

	if(itsContainer instanceof TrackWindow)
	{
	   TrackWindow window = (TrackWindow)itsContainer;
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















