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
public class AnythingTrackEditor extends JPanel implements TrackEditor
{
    public AnythingTrackEditor(Geometry g, Track track)
    {
	super();

	this.geometry = g;
	this.itsTrack = track;

	itsTrack.getTrackDataModel().addListener(new TrackDataListener() {
	    public void objectDeleted(Object whichObject, int oldIndex) {}
	    public void trackCleared() {}
	    public void objectAdded(Object whichObject, int index) {}
	    public void objectsAdded(int maxTime) {}
	    public void objectChanged(Object whichObject, String propName, Object propValue) {}
	    public void objectMoved(Object whichObject, int oldIndex, int newIndex) {}
	    public void lastObjectMoved(Object whichObject, int oldIndex, int newIndex) {}
	    public void trackNameChanged(String oldName, String newName) {
		itsTrack.setProperty("trackName", newName);

		System.err.println("AnythingTrackEditor name changed "+newName);
	    }
	});

	geometry.addTranspositionListener(new TranspositionListener() {
	    public void transpositionChanged(int newTranspose)
		{
		    repaint();
		}
	});

	createGraphicContext(geometry, itsTrack.getTrackDataModel());

	itsTrack.setProperty("locked", new Boolean(true));

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

	gc = new SequenceGraphicContext(model, selection, this); //loopback?
	gc.setGraphicSource(this);
	gc.setGraphicDestination(this);
	ad = new MonoDimensionalAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET);
	itsTrack.getPropertySupport().addPropertyChangeListener(ad);
	gc.setAdapter(ad);

	renderer = new AnythingTrackRenderer(gc);
	gc.setRenderManager(renderer);
    }

    public void setAdapter(MonoDimensionalAdapter adapter)
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
	return new Dimension(Sequence.DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, DEFAULT_HEIGHT);
    }
    public Track getTrack()
    {
	return itsTrack;
    }
    //--- AnythingTrackEditor fields
    Geometry geometry;
    SequenceGraphicContext gc;
    SequenceSelection selection;
    static int MONODIMENSIONAL_TRACK_OFFSET = 0;
    static public int DEFAULT_HEIGHT = 70;
    AnythingTrackRenderer renderer;
    MonoDimensionalAdapter ad;
    Track itsTrack;
}




