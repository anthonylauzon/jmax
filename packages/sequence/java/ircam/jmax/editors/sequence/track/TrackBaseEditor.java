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
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.beans.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

public abstract class TrackBaseEditor extends PopupToolbarPanel implements TrackDataListener, ListSelectionListener, TrackEditor
{
  public TrackBaseEditor(Geometry geom, Track trk, boolean isInSequence)
	{ 
    super();	
		
		this.track = trk;
    this.geometry = geom;
		
    setBackground(Color.white);
    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
		
    setPreferredSize( new Dimension(SequenceWindow.DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, getInitialHeight()));
    
    setOpaque(false);
		
    gc = createGraphicContext(geometry, track);
		
		setDisplayer();
		initGridMode();
		
    track.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent e)
		  {	  
				String name = e.getPropertyName();
				if (name.equals("selected") && e.getNewValue().equals(Boolean.TRUE))
					SequenceSelection.setCurrent(selection);
			}
		});
		
    geometry.addTranspositionListener(new TranspositionListener() {
			public void transpositionChanged(int newTranspose)
		{
				repaint();
		}
		});
		
    selection.addListSelectionListener(this);
		
    selection.setOwner(new SelectionOwner() {
			public void selectionDisactivated()
		{
				repaint();
		}
			public void selectionActivated()
		{
				repaint();
		}
		});
				
    track.getTrackDataModel().addListener(this);
    
    track.getTrackDataModel().addTrackStateListener(new TrackStateListener(){
			public void lock(boolean lock)
		  {
				for (Enumeration e = oldElements.elements(); e.hasMoreElements();) 
					((TrackEvent) e.nextElement()).setHighlighted(false);
				
				oldElements.removeAllElements();
				getTrack().setProperty("locked", new Boolean(lock));
		  }
			public void active(boolean active)
		  {
				getTrack().setProperty("active", (active) ? Boolean.TRUE : Boolean.FALSE);
		  }
			public void restoreEditorState(FtsTrackEditorObject editorState)
		  {
				geometry.setXZoomFactor(editorState.zoom);
				gc.getScrollManager().scrollToValue(-editorState.transp);
				if(!editorState.label.equals(""))
					setLabelType(editorState.label);
				setViewMode(editorState.view);				
				setRangeMode(editorState.rangeMode, true);
        setGridMode(editorState.gridMode);
			};
			public void hasMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
		  {
				listenToMarkers(markers, markersSelection);
			}
			public void updateMarkers(FtsTrackObject markers, SequenceSelection markersSelection)
			{			
				if(currentSelMarkers!=null)
					currentSelMarkers.removeListSelectionListener( TrackBaseEditor.this);
				if(currentMarkers != null)
					currentMarkers.removeListener( markerTrackListener);
				if( markers != null && markersSelection != null)
					listenToMarkers( markers, markersSelection);
				else
				{
					currentSelMarkers = null;
					currentMarkers = null;
				}
				repaint();
			}
      public void ftsNameChanged(String name){}
		});
		
    track.getTrackDataModel().addHighlightListener(new HighlightListener(){
			public void highlight(Enumeration elements, double time)
		{
				TrackEvent temp;
				boolean first = true;
				
				Rectangle clipRect = gc.getTrackClip().intersection(gc.getScrollManager().getViewRectangle());
				Graphics g = getGraphics();
				g.setClip(clipRect.x, clipRect.y, clipRect.width, clipRect.height);
				
				for (Enumeration e = oldElements.elements(); e.hasMoreElements();) 
				{
					temp = (TrackEvent) e.nextElement();
					temp.setHighlighted(false);
					temp.getRenderer().render(temp, g, false, gc);			    
				}
				oldElements.removeAllElements();
				
				for (Enumeration e = elements; e.hasMoreElements();) 
				{
					temp = (TrackEvent) e.nextElement();
					if(first)
					{
						scrollEvent = temp;
						SwingUtilities.invokeLater(new Runnable(){
							public void run()
						{
								if(scrollEvent != null)
								{
									gc.getScrollManager().makeVisible(scrollEvent);
									scrollEvent = null;
								}
						}
						});
						
						first = false;
					}
					temp.setHighlighted(true);
					temp.getRenderer().render(temp, g, Event.HIGHLIGHTED, gc);
					oldElements.addElement(temp);			    
				}
		}
		});
    
    addMouseListener(new MouseListener(){
			public void mouseClicked(MouseEvent e){}
			public void mousePressed(MouseEvent e){}
			public void mouseReleased(MouseEvent e){}
			public void mouseEntered(MouseEvent e)
		  {
				requestFocus();
		  }
			public void mouseExited(MouseEvent e){
				gc.getDisplayer().display("");
			}
		});
    addMouseMotionListener(new MouseMotionListener(){
			public void mouseMoved(MouseEvent e)
		  {
        displayMousePosition(e.getX(), e.getY());
      }
			public void mouseDragged(MouseEvent e){}
		});
		
		markerTrackListener = new TrackDataListener() {
			public void objectChanged(Object spec, String propName, Object propValue){repaint();}
			public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){repaint();}
			public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){repaint();}
			public void objectAdded(Object whichObject, int index){repaint();}
			public void objectsAdded(int maxTime){repaint();}
			public void objectDeleted(Object whichObject, int oldIndex){repaint();}
			public void trackCleared(){repaint();}
			public void startTrackUpload( TrackDataModel track, int size){}
			public void endTrackUpload( TrackDataModel track){}
			public void startPaste(){}
			public void endPaste(){}
		};
		
		if( isInSequence && gc.getMarkersTrack() != null)
			listenToMarkers( gc.getMarkersTrack(), gc.getMarkersSelection());
    
    component = this;
}

void setDisplayer()
{
	JPanel labelPanel = new JPanel();
	labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
	labelPanel.setOpaque(false);
	
	displayLabel = new JLabel();
	displayLabel.setFont( Displayer.displayFont);
	displayLabel.setForeground( Color.gray);
	displayLabel.setPreferredSize( new Dimension(102, 15));
	displayLabel.setMaximumSize( new Dimension(102, 15));
	displayLabel.setMinimumSize( new Dimension(102, 15));
	
	labelPanel.add( Box.createRigidArea( new Dimension(5, 0)) );
	labelPanel.add( displayLabel);
	labelPanel.add( Box.createHorizontalGlue());
	
	add(labelPanel);
	add(Box.createVerticalGlue());
	
	gc.setDisplayer( new Displayer(){
		public void display(String text)
	{
			displayLabel.setText( text);
	}
	});	
}

void displayMousePosition(int x, int y)
{
  double time = gc.getAdapter().getInvX( x);
  if(time < 0) time = 0;	      
  int val =  gc.getAdapter().getInvY( y);
  
  gc.getDisplayer().display(Displayer.numberFormat.format(time)+" , "+Displayer.numberFormat.format(val));	  
}

void initGridMode()
{
	Object prop = geometry.getProperty("gridMode");
	if(prop != null)
		gc.setGridMode( ((Integer)prop).intValue());
}
public void setGridMode(int gridMode){}

void listenToMarkers( FtsTrackObject markers, SequenceSelection markSel)
{
	currentMarkers = markers;
	currentSelMarkers = markSel;
	currentSelMarkers.addListSelectionListener(this);
	currentMarkers.addListener( markerTrackListener);	
}

public SequenceSelection getMarkerSelection()
{
	return currentSelMarkers;
}

public abstract void reinit();
abstract void createPopupMenu();
abstract SequenceGraphicContext createGraphicContext(Geometry geometry, Track track);
public abstract int getDefaultHeight();

public JPopupMenu getMenu()
{    
	if( popup ==null)
		createPopupMenu();
	
	popup.update();
	return popup;
}

public int trackCount()
{
	if( gc.getFtsObject() instanceof FtsSequenceObject)
		return ((FtsSequenceObject)gc.getFtsObject()).trackCount();
	else
		return 1;
}

/**
* Callback from the toolbar when a new tool have been
 * selected by the user
 */ 
public void toolChanged(ToolChangeEvent e){}

/*******************************************************************************
* DataTrackListener interface
******************************************************************************/

boolean uploading  = false;
public void objectChanged(Object spec, String propName, Object propValue) 
{
	repaint();
}

public void objectAdded(Object spec, int index) 
{
	if( !uploading)
		repaint();
}

public void objectsAdded(int maxTime) 
{
	repaint();
}

public void objectDeleted(Object whichObject, int index) 
{
	repaint();
}

public void trackCleared() 
{
	repaint();
}
public void startTrackUpload( TrackDataModel track, int size)
{
	uploading  = true;
}
public void endTrackUpload( TrackDataModel track)
{
	uploading  = false;
	setRangeMode(getRangeMode(), false);
  uploadEnd();
}
public void startPaste(){}
public void endPaste(){}

public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient) 
{
	repaint();
}

public void objectMoved(Object whichObject, int oldIndex, int newIndex, boolean fromClient){}

/******************/
void uploadEnd(){}

/*****************************************************************************
* ListSelectionListener interface
*****************************************************************************/

public void valueChanged(ListSelectionEvent e)
{
	repaint();
}

/* avoid to paint the white background twice*/   
public void update(Graphics g) {}


public SequenceSelection getSelection()
{
	return selection;
}


public void dispose()
{
	if(listDialog != null)
		listDialog.dispose();
}

/****************** TrackEditor interface **********************************/

public Component getComponent()
{
	return component;
}

public void setComponent(Component c)
{
	component = c;
}

public SequenceGraphicContext getGraphicContext()
{
	return gc;
}

public Track getTrack()
{
	return track;
}

public void updateNewObject(Object obj){};

public void setViewMode(int viewType)
{
	if(viewMode!=viewType)
	{
		viewMode=viewType;
		track.setProperty("viewMode", new Integer(viewType));
		repaint();
	}    
}
public int getViewMode()
{
	return viewMode;
}

public void setRangeMode(int rangeMode, boolean changed)
{
	this.rangeMode = rangeMode;
}
public int getRangeMode()
{
	return this.rangeMode;
}

public void showListDialog()
{
	if(listDialog==null) 
		createListDialog();
	listDialog.setVisible(true);
}

public void setContainer(SequenceEditor container)
{
  this.container = container;
}

void createListDialog()
{
	listDialog = new SequenceTableDialog(track, gc.getFrame(), gc);
}

public void processKeyEvent(KeyEvent e)
{
  if(e.getID()==KeyEvent.KEY_PRESSED)
  {
    switch( e.getKeyCode())
    {
      case KeyEvent.VK_DELETE:
      case KeyEvent.VK_BACK_SPACE:
        ((UndoableData)track.getTrackDataModel()).beginUpdate();
        getSelection().deleteAll();
        if(currentSelMarkers != null)
          currentSelMarkers.deleteByType("marker");
        break;
      case KeyEvent.VK_LEFT:       
        if(getSelection().size() == 0 && (currentSelMarkers != null && currentSelMarkers.size() > 0))
          currentSelMarkers.selectPreviousByType();
        else
          getSelection().selectPrevious();
        break;
      case KeyEvent.VK_RIGHT:
        if(getSelection().size() == 0 && (currentSelMarkers != null && currentSelMarkers.size() > 0))
          currentSelMarkers.selectNextByType();
        else
          getSelection().selectNext();
        break;
      default:
        break;
    }  
  }
	super.processKeyEvent(e);
	requestFocus();
}

public void paintComponent(Graphics g) 
{
	Rectangle r = g.getClipBounds();
	renderer.render(g, r);
}

public void forceBackgroundRepaint()
{
  Object obj = getTrack().getProperty("repaint");
  boolean rep = true;
  if( obj !=  null)
    rep = !((Boolean)obj).booleanValue();
  
  getTrack().setProperty("repaint", new Boolean(rep)); //to repaint background
}
public void setLabelType(String type){}

public int getInitialHeight()
{
  return DEFAULT_HEIGHT;
}

//--- MidiTrack fields
transient Geometry geometry;
transient SequenceGraphicContext gc;
transient Track track;
transient AbstractRenderer renderer;
transient Component component;
transient SequenceSelection selection;
transient TrackEvent scrollEvent = null;

transient MaxVector oldElements = new MaxVector();
transient SequenceTableDialog listDialog = null;

transient FtsTrackObject currentMarkers = null; 
transient SequenceSelection currentSelMarkers = null;
transient TrackDataListener markerTrackListener = null;

public int DEFAULT_HEIGHT = 430;
public int viewMode, rangeMode;
TrackBasePopupMenu popup = null;
JLabel displayLabel;

public transient SequenceEditor container = null;

int gridMode = TIME_GRID;
}




