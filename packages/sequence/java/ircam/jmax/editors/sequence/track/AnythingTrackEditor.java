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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * A Monodimensional view for a generic Sequence. 
 * This kind of editor use a MonoDimensionalAdapter
 * to map the y values. The value returned is always half of the panel,
 * and settings of y are simply ignored. */
public class AnythingTrackEditor extends JPanel implements ListSelectionListener, TrackEditor
{
    public AnythingTrackEditor(Geometry g, Track track)
    {
	super();

	this.geometry = g;
	this.itsTrack = track;

	itsTrack.getTrackDataModel().addListener(new TrackDataListener() {
	    public void objectDeleted(Object whichObject, int oldIndex) 
	    {
	      AnythingTrackEditor.this.repaint();
	    }
	    public void trackCleared() 
	    {
	      AnythingTrackEditor.this.repaint();
	    }
	    public void endTrackUpload(){}
	    public void objectAdded(Object whichObject, int index) 
	    { 
	      AnythingTrackEditor.this.repaint();
	    }
	    public void objectsAdded(int maxTime) 
	    {
	      AnythingTrackEditor.this.repaint();
	      selection.deselectAll();
	    }
	    public void objectChanged(Object whichObject, String propName, Object propValue) 
	    {
	      AnythingTrackEditor.this.repaint();
	    }
	    public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
	    {
	      AnythingTrackEditor.this.repaint();
	    }
	    public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex) {}
	    public void trackNameChanged(String oldName, String newName) {
		itsTrack.setProperty("trackName", newName);
	    }
	});

	geometry.addTranspositionListener(new TranspositionListener() {
	    public void transpositionChanged(int newTranspose)
		{
		    repaint();
		}
	});

	itsTrack.getTrackDataModel().addTrackStateListener(new TrackStateListener(){
		public void lock(boolean lock){}
		public void active(boolean active)
		{
		    itsTrack.setProperty("active", (active) ? Boolean.TRUE : Boolean.FALSE);
		}
	    });

	createGraphicContext(geometry, itsTrack.getTrackDataModel());

	//--- make this track's selection the current 
	// one when the track is activated
	itsTrack.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
		public void propertyChange(PropertyChangeEvent e)
		{		    
		    if (e.getPropertyName().equals("selected") && e.getNewValue().equals(Boolean.TRUE))
			SequenceSelection.setCurrent(selection);
		}
	});

	//track.setProperty("locked", new Boolean(true));

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

	setBackground(Color.lightGray);
	setOpaque(false);
    }

    public void reinit(){}
    
    public JMenu getToolsMenu()
    {
	return gc.getToolbar().itsMenu;
    }
    
    public JPopupMenu getMenu()
    {
	return null;
    }

    public int trackCount()
    {
      if( gc.getFtsObject() instanceof FtsSequenceObject)
	return ((FtsSequenceObject)gc.getFtsObject()).trackCount();
      else
	return 1;
    }

    public void paintComponent(Graphics g) 
    {
      Rectangle r = g.getClipBounds();
      renderer.render(g, r); //et c'est tout	
    }

    private void createGraphicContext(Geometry geometry, TrackDataModel model)
    {
	selection = new SequenceSelection(model);

	gc = new SequenceGraphicContext(model, selection, this); //loopback?
	gc.setGraphicSource(this);
	gc.setGraphicDestination(this);
	ad = new AnythingAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET);
	itsTrack.getPropertySupport().addPropertyChangeListener(ad);
	gc.setAdapter(ad);

	renderer = new AnythingTrackRenderer(gc);
	gc.setRenderManager(renderer);
    }

    public void setAdapter(AnythingAdapter adapter)
    {
	itsTrack.getPropertySupport().removePropertyChangeListener(ad);	
	itsTrack.getPropertySupport().addPropertyChangeListener(adapter);
	gc.setAdapter(adapter);	
	ad = adapter;
    }

    public void setRenderer(AnythingTrackRenderer renderer)
    {
	this.renderer = renderer;
	gc.setRenderManager(renderer);
    }

    public void showListDialog(){}
    public void updateNewObject(Object obj){};
    void updateEventProperties(Object whichObject, String propName, Object propValue){}
    void updateRange(Object whichObject){}  

    /**
     * ListSelectionListener interface
     */
    
    public void valueChanged(ListSelectionEvent e)
    {
	repaint();
    }
      
    /**
     * Track editor interface */

    public Component getComponent()
    {
	return this;
    }

    public SequenceGraphicContext getGraphicContext()
    {
	return gc;
    }

    public int getDefaultHeight()
    {
	return DEFAULT_HEIGHT;
    }
    public void dispose(){}
    public SequenceSelection getSelection()
    {
	return selection;
    }
    public Dimension getPreferredSize()
    {
	return new Dimension(SequenceWindow.DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, DEFAULT_HEIGHT);
    }
    public Track getTrack()
    {
	return itsTrack;
    }

    public void processKeyEvent(KeyEvent e)
    {
	if(SequenceTextArea.isDeleteKey(e))
	    {
		if(e.getID()==KeyEvent.KEY_PRESSED)
		    {
			((UndoableData)itsTrack.getTrackDataModel()).beginUpdate();
			selection.deleteAll();
		    }
	    }
	else if((e.getKeyCode() == KeyEvent.VK_TAB)&&(e.getID()==KeyEvent.KEY_PRESSED))
	    if(e.isControlDown())
		selection.selectPrevious();
	    else
		selection.selectNext();

	super.processKeyEvent(e);
	requestFocus();
    }

    //--- AnythingTrackEditor fields
    Geometry geometry;
    SequenceGraphicContext gc;
    SequenceSelection selection;
    static int MONODIMENSIONAL_TRACK_OFFSET = 0;
    static public int DEFAULT_HEIGHT = 70;
    AnythingTrackRenderer renderer;
    AnythingAdapter ad;
    Track itsTrack;
}




