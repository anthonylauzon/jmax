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
public class SequencePanel extends JPanel implements Editor, TrackListener, TrackDataListener, ListSelectionListener {
    
    FtsSequenceObject ftsSequenceObject;

    EditorToolbar toolbar;
    SequenceDataModel sequenceData;
    EditorContainer itsContainer;
    public InfoPanel statusBar;
    public JPanel ruler;
    
    Box trackPanel;
    JScrollPane scrollTracks;
    
    Hashtable trackContainers = new Hashtable();
    MutexPropertyHandler mutex = new MutexPropertyHandler("active");
    //---
    JScrollBar itsTimeZoom;
    JLabel itsZoomLabel;
    JScrollBar itsTimeScrollbar;
    Geometry geometry;
    PartitionAdapter utilityPartitionAdapter;
    ToolManager manager;

    public final int INITIAL_ZOOM = 20;
    public static final int MINIMUM_TIME = 10000;

    static public Color violetColor = new Color(102,102,153);
    static public Font rulerFont = new Font("SansSerif", Font.PLAIN, 10);
    static public Image violinClefImage;
    static public Image faClefImage;
    static String path;

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

    //Create a Geometry object for this sequencer
    geometry = new Geometry();

    utilityPartitionAdapter = new PartitionAdapter(geometry, null);

    //Create the backgrounds images????
    try{
      path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+File.separator+"images"+File.separator;
    }
    catch(FileNotFoundException e){
	path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
    }

    violinClefImage = Toolkit.getDefaultToolkit().getImage(path+"violinClef.gif");
    faClefImage = Toolkit.getDefaultToolkit().getImage(path+"faClef.gif");
    Toolkit.getDefaultToolkit().prepareImage(violinClefImage, /*25, 54*/20, 51, this);
    Toolkit.getDefaultToolkit().prepareImage(faClefImage, 23, 30, this);
   
    //------------------------------------------------
    // Create the ruler
    ruler = new Ruler();
    ruler.setDoubleBuffered(false);
    geometry.addTranspositionListener( new TranspositionListener() {
	public void transpositionChanged(int newValue)
	    {
		ruler.repaint();
	    }
    });

    //-------------------------------------------------
    //-- Tools, toolbar and status bar:
    //- Create the ToolManager with the needed tools
    //- Create a toolbar associated to this ToolManager
    //- Create a status bar containing the toolbar
    

    manager = new ToolManager(SequenceTools.instance);
    Tool arrow = manager.getToolByName("arrow"); 
    
    manager.activate(arrow, null); //we do not have a gc yet...

    toolbar = new EditorToolbar(manager, EditorToolbar.HORIZONTAL);
    toolbar.setSize(200, 30);

    //------------------- prepare the track panel:

    trackPanel = new Box(BoxLayout.Y_AXIS);
    scrollTracks = new JScrollPane(trackPanel, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
				   JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

    /**********  DEBUG CODE: for now, sequenceRemoteData is Empty (the FTS counterpart does not exist yet)
     ********** so we insert some false track to test the track editors */

    //ftsSequenceObject.addTrack(new TrackBase(new AbstractSequence(ftsSequenceObject, AmbitusValue.info)));    
    ftsSequenceObject.requestTrackCreation(/*AmbitusValue.info.getName()*/"ambitus");

    setLayout(new BorderLayout());

    JPanel separate_tracks = new JPanel();
    separate_tracks.setLayout(new BorderLayout());

    //ftsSequenceObject.getTrackAt(0).setProperty("active", Boolean.TRUE);

    trackPanel.setSize(500, 50);

    separate_tracks.add(scrollTracks, BorderLayout.CENTER);
    
    //-- prepares the Status bar
    
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
 
    statusBar.setSize(300, 20);

    statusBar.addWidgetAt(toolbar, 2);

    ruler.setSize(300, 20);

    northSection.add(statusBar);
    northSection.add(ruler);	
    separate_tracks.add(northSection, BorderLayout.NORTH);

    itsContainer.getFrame().validate();
    itsContainer.getFrame().pack();

    itsContainer.getFrame().setVisible(true);

    
    //-- prepares the zoom scrollbar (time stretching) and its listeners

    itsTimeZoom = new JScrollBar(Scrollbar.HORIZONTAL, INITIAL_ZOOM, 200, 1, 1000);
    itsTimeZoom.setBlockIncrement(10);

    itsZoomLabel = new JLabel("Zoom: "+INITIAL_ZOOM+"%");
    
    itsZoomLabel.setMaximumSize(new Dimension (100, 15));
    itsZoomLabel.setMinimumSize(new Dimension (100, 15));
    itsZoomLabel.setPreferredSize(new Dimension (100, 15));
    
    geometry.addZoomListener( new ZoomListener() {
	public void zoomChanged(float zoom)
	    {
		itsZoomLabel.setText("Zoom: "+((int)(zoom*100))+"%"); 
		repaint();
	    }
    });
    
    itsTimeZoom.addAdjustmentListener(new AdjustmentListener() {
	
	public void adjustmentValueChanged(AdjustmentEvent e) {
	    
	    geometry.setXZoom(e.getValue());
	    itsTimeScrollbar.setVisibleAmount(Geometry.sizeToMsec(geometry, SequencePanel.this.getSize().width)/2);
	}
	
    });
    
    
    //-- prepares the SOUTH scrollbar (time scrolling) and its listener
    
    int totalTime = MINIMUM_TIME;
    
    itsTimeScrollbar = new JScrollBar(Scrollbar.HORIZONTAL, 0, 1000, 0, totalTime);
    itsTimeScrollbar.setUnitIncrement(Geometry.sizeToMsec(geometry, SequencePanel.this.getSize().width)/10);//WARN: setUnitIncrement seems not to work
    itsTimeScrollbar.setBlockIncrement(Geometry.sizeToMsec(geometry, SequencePanel.this.getSize().width));
    
    
    itsTimeScrollbar.addAdjustmentListener(new AdjustmentListener() {
	
	public void adjustmentValueChanged(AdjustmentEvent e) {
	    
	    int currentTime = e.getValue();
	    
	    geometry.setXTransposition(-currentTime);	    
	}
    });
    
    JPanel aSliderPanel = new JPanel();
    aSliderPanel.setLayout(new ProportionalLayout(ProportionalLayout.X_AXIS, (float) 0.75));
    aSliderPanel.add("", itsTimeScrollbar);
    Box aZoomBox = new Box(BoxLayout.X_AXIS);
    aZoomBox.add(itsZoomLabel);
    aZoomBox.add(itsTimeZoom);
    
    aSliderPanel.add("", aZoomBox);
    
    separate_tracks.add(aSliderPanel, BorderLayout.SOUTH);

    add(separate_tracks, BorderLayout.CENTER);

  }

    /**
     * Callback from the model. This is called when a new track is added, but also
     * as a result of a merge */
    public void trackAdded(Track track)
    {
	TrackEditor teditor = TrackEditorFactoryTable.newEditor(track, geometry);
	teditor.getGraphicContext().setToolManager(manager);
	teditor.getGraphicContext().setFrame(itsContainer.getFrame());
	manager.addContextSwitcher(new ComponentContextSwitcher(teditor.getComponent(), teditor.getGraphicContext()));

	TrackContainer trackContainer = new TrackContainer(track, teditor);
	
	trackContainer.setBorder(new EtchedBorder()); 
	trackPanel.add(trackContainer);
	
	trackPanel.validate();
	scrollTracks.validate();
	scrollTracks.getVerticalScrollBar().setValue(scrollTracks.getVerticalScrollBar().getMaximum());

	mutex.add(track);
	trackContainers.put(track, trackContainer);

	teditor.getSelection().addListSelectionListener(this);//????
	track.setProperty("active", Boolean.TRUE);

	//added to update maximum time if needed
	track.getTrackDataModel().addListener(this);
    }


    /**
     * Callback from the model. It can be called when two tracks are merged into one */
    public void trackRemoved(Track track)
    {
	TrackContainer trackContainer = (TrackContainer) trackContainers.get(track);
	
	trackPanel.remove(trackContainer);
	
	trackContainer.getTrackEditor().dispose();

	trackPanel.validate();
	scrollTracks.validate();

	mutex.remove(track);

	trackContainers.remove(track);

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

  public void moveTrackTo(TrackEditor editor, int pos)
  {
      TrackContainer trackContainer = (TrackContainer) trackContainers.get(editor.getTrack());

      if(trackPanel.getComponent(pos)==trackContainer)
	  return;
      trackPanel.remove(trackContainer);
      trackPanel.add(trackContainer, pos);
      trackPanel.validate();
      scrollTracks.validate();
      scrollTracks.getVerticalScrollBar().setValue(trackContainer.getBounds().y);
      editor.getTrack().setProperty("active", Boolean.TRUE);
  }

   /**
     * called when the database is changed: DataTrackListener interface
     */
    
    public void objectChanged(Object spec) {}
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

    //controll if the object is in the actual scrollable area. if not extend the area
    private void resizePanelToEventTime(TrackEvent evt)
    {
	int evtTime = (int)(evt.getTime()) + ((Integer)evt.getProperty("duration")).intValue();
	resizePanelToTime(evtTime);
    }

    private void resizePanelToTime(int time)
    {
	int maxVisibleTime = getMaximumVisibleTime();

	if(time > getMaximumTime())
	{
	    itsTimeScrollbar.setMaximum(time);
	    itsTimeScrollbar.setValue(time);
	}
	else 
	    if( time > maxVisibleTime)		
		itsTimeScrollbar.setValue(time-maxVisibleTime-geometry.getXTransposition()+10);
	    else
		if(time < -geometry.getXTransposition())
		    itsTimeScrollbar.setValue(time);
    }
    
  ////////////////////////////////////////////////////////////
  public void Copy()
  {
    ((ClipableData) sequenceData).copy();
  }

  public void Cut()
  {
    ((ClipableData) sequenceData).cut();
  }

  public void Paste()
  {
    ((ClipableData) sequenceData).paste();
  }

  public void Duplicate()
  {
    Copy();
    Paste();
  }

  public void Undo()
  {
    try 
      {
	//((UndoableData) sequenceData).undo();
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
      }
  }

  public void Redo()
  {
    try 
      {
	//((UndoableData) sequenceData).redo();
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

    public Frame getFrame(){
	return itsContainer.getFrame();
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
	int dur = ((Integer)evt.getProperty("duration")).intValue();
	int startTime = -geometry.getXTransposition(); 
	int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
	return ((time>startTime)&&(time+dur<endTime));
    }

    private int scrollingDelta = 10; 
    public boolean scrollBy(int eventTime)
    {
	int startTime = -geometry.getXTransposition(); 
	int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
	if(eventTime<startTime)
	    {
		System.err.println("troppo piccino ");
		itsTimeScrollbar.setValue(itsTimeScrollbar.getValue()-scrollingDelta);
		return false;//going to left
	    }
	else
	    {
		if(eventTime>endTime){
		    System.err.println("troppo grande ");
		    itsTimeScrollbar.setValue(itsTimeScrollbar.getValue()+scrollingDelta);
		}
		return true;//going to rigth
	    }
    }

    public void makeVisible(TrackEvent evt)
    {
	int time = (int)evt.getTime();
	int duration = ((Integer)evt.getProperty("duration")).intValue();
	int startTime = -geometry.getXTransposition(); 
	int endTime = geometry.sizeToMsec(geometry, getSize().width-TrackContainer.BUTTON_WIDTH - ScoreBackground.KEYEND)-1 ;
	
	if(time<startTime)
	    itsTimeScrollbar.setValue(time);
	else if(time>endTime)
	    itsTimeScrollbar.setValue(time-endTime+startTime+duration+10);
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


    /**
     * A graphic JPanel that represents a ruler containing time indications */
    class Ruler extends JPanel {

	Ruler()
	{
	    super();
	    setOpaque(false);
	    setFont(rulerFont);
	    fm = getFontMetrics(rulerFont);
	}

	public void paint(Graphics g)
	{
	    int xPosition;
	    int snappedTime;
	    AmbitusValue value = new AmbitusValue();
	    UtilTrackEvent tempEvent = new UtilTrackEvent(value);
	    int logicalTime = -geometry.getXTransposition();
	    int windowTime = getMaximumVisibleTime();	    

	    int timeStep = ScoreBackground.findBestTimeStep(windowTime-logicalTime);

	    //controll if the time string is too long (in this case draw one string on two)
	    int stringLenght = fm.stringWidth(""+(logicalTime+timeStep));
	    int delta = utilityPartitionAdapter.getX(logicalTime+timeStep)-utilityPartitionAdapter.getX(logicalTime);
	    int k;
	    if(stringLenght>delta-10) k = 2;
	    else k=1;

	    g.setColor(SequencePanel.violetColor);
	    for (int i=logicalTime+timeStep; i<windowTime; i+=timeStep*k) 
		{
		    snappedTime = (i/timeStep)*timeStep;
		    tempEvent.setTime(snappedTime);
		    xPosition = utilityPartitionAdapter.getX(tempEvent)+TrackContainer.BUTTON_WIDTH-10;

		    g.drawString(""+snappedTime, xPosition, 15);
		}
	}
	
	
	public Dimension getPreferredSize()
	{ return rulerDimension; }
	
	public Dimension getMinimumSize()
	{ return rulerDimension; }
    	
	//--- Ruler fields
	Dimension rulerDimension = new Dimension(200, 30);
	FontMetrics fm;
    }    
}





