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
import java.awt.*;
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
public class MonoTrackEditor extends PopupToolbarPanel implements ListSelectionListener, TrackEditor
{
    public MonoTrackEditor(Geometry g, Track track)
    {
	super();

	this.geometry = g;
	this.track = track;

	track.getTrackDataModel().addListener(new TrackDataListener() {
	    public void objectDeleted(Object whichObject, int oldIndex) {MonoTrackEditor.this.repaint();}
	    public void objectAdded(Object whichObject, int index) {
		MonoTrackEditor.this.repaint();
	    }
	    public void objectsAdded(int maxTime) {
		MonoTrackEditor.this.repaint();
	    }
	    public void objectChanged(Object whichObject, String propName, Object propValue) {
		updateEventProperties(whichObject, propName, propValue);
		MonoTrackEditor.this.repaint();
	    }
	    public void objectMoved(Object whichObject, int oldIndex, int newIndex) {
		MonoTrackEditor.this.repaint();
	    }
	});

	geometry.addTranspositionListener(new TranspositionListener() {
	    public void transpositionChanged(int newTranspose)
		{
		    repaint();
		}
	});

	createGraphicContext(geometry, track.getTrackDataModel());

	//--- make this track's selection the current 
	// one when the track is activated
	track.getPropertySupport().addPropertyChangeListener(new PropertyChangeListener() {
	    public void propertyChange(PropertyChangeEvent e)
		{

		    if (e.getPropertyName().equals("active") && e.getNewValue().equals(Boolean.TRUE))
			SequenceSelection.setCurrent(selection);
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

	setBackground(Color.white);

	setOpaque(false);
    }
    
    public JMenu getToolsMenu()
    {
	return gc.getToolbar().itsMenu;
    }
    
    public JPopupMenu getMenu()
    {
	MonoTrackPopupMenu.getInstance().update(this);
	return MonoTrackPopupMenu.getInstance();
    }

    public int trackCount()
    {
	//return gc.getSequenceRemoteData().trackCount();
	return gc.getFtsSequenceObject().trackCount();
    }

    public void paintComponent(Graphics g) 
    {
      Rectangle r = g.getClipBounds();
      renderer.render(g, r); //et c'est tout	
    }

    private void createGraphicContext(Geometry geometry, TrackDataModel model)
    {
	selection = new SequenceSelection(model);

	gc = new SequenceGraphicContext(model, selection, track); //loopback?
	gc.setGraphicSource(this);
	gc.setGraphicDestination(this);
	MonoDimensionalAdapter ad = new MonoDimensionalAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET);
	track.getPropertySupport().addPropertyChangeListener(ad);
	gc.setAdapter(ad);

	renderer = new MonoTrackRenderer(gc);
	gc.setRenderManager(renderer);
    }

    public void setRenderer(MonoTrackRenderer renderer)
    {
	this.renderer = renderer;
	gc.setRenderManager(renderer);
    }

    /**
     * ListSelectionListener interface
     */
    
    public void valueChanged(ListSelectionEvent e)
    {
	repaint();
    }
    
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

    
    public void showListDialog()
    {
	if(listDialog==null) 
	    createListDialog();
	listDialog.setVisible(true);
    }

    private void createListDialog()
    {
	listDialog = new ListDialog(track, gc.getFrame(), gc);
    }

    void updateEventProperties(Object whichObject, String propName, Object propValue){}
    
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

    public void dispose()
    {
	if(listDialog != null)
	    listDialog.dispose();
    }

    public SequenceSelection getSelection()
    {
	return selection;
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(800, DEFAULT_HEIGHT);
    }

    public Track getTrack()
    {
	return track;
    }

    //--- FricativeTrackEditor fields
    Geometry geometry;
    SequenceGraphicContext gc;
    SequenceSelection selection;
    static int MONODIMENSIONAL_TRACK_OFFSET = 0;
    static public int DEFAULT_HEIGHT = 127;
    MonoTrackRenderer renderer;

    Track track;

    ListDialog listDialog = null;

    int viewMode = PEAKS_VIEW;
    static public final int PEAKS_VIEW = 2;
    static public final int STEPS_VIEW = 3;
    static public final int BREAK_POINTS_VIEW = 4;
}

