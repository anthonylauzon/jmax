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
public class MonodimensionalTrackEditor extends PopupToolbarPanel implements ListSelectionListener, TrackEditor
{
    public MonodimensionalTrackEditor(Geometry g, Track track)
    {
	super();

	this.geometry = g;
	this.track = track;

	track.getTrackDataModel().addListener(new TrackDataListener() {
	    public void objectDeleted(Object whichObject, int oldIndex) {MonodimensionalTrackEditor.this.repaint();}
	    public void objectAdded(Object whichObject, int index) {
		MonodimensionalTrackEditor.this.repaint();
	    }
	    public void objectChanged(Object whichObject) {MonodimensionalTrackEditor.this.repaint();}
	    public void objectMoved(Object whichObject, int oldIndex, int newIndex) {MonodimensionalTrackEditor.this.repaint();}
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
    
    public JPopupMenu getMenu()
    {
	return gc.getToolbar().itsPopupMenu;
    }


    public void paint(Graphics g) 
    {
	super.paint(g);
	int x = 0;
	int y = 0;
	int w = gc.getGraphicDestination().getSize().width;
	int h = gc.getGraphicDestination().getSize().height;

	g.setColor(Color.white);
	g.fillRect(x, y, w, h);

	g.setColor(Color.black);
	g.drawLine(x, y+h/2, x+w, y+h/2);

	// paint what is needed:
	Rectangle r = new Rectangle (0,0,w, h);
	TrackEvent temp;
	
	for (Enumeration e = objectsIntersecting( r.x, r.y, r.width, r.height); e.hasMoreElements();) 
	    {
		temp = (TrackEvent) e.nextElement();
		temp.getRenderer().render( temp, g, SequenceSelection.getCurrent().isInSelection(temp), gc);
	    }
	
    }

    
    private Enumeration objectsIntersecting(int x, int y, int w, int h)
    {
	TrackEvent aTrackEvent;
	Vector tempList = new Vector();

	int startTime = gc.getAdapter().getInvX(x);
	int endTime = gc.getAdapter().getInvX(x+w);

	for (Enumeration e = track.getTrackDataModel().intersectionSearch(startTime, endTime); e.hasMoreElements();) 
	    {
		aTrackEvent = (TrackEvent) e.nextElement();

		if (aTrackEvent.getRenderer().touches(aTrackEvent, x, y, w, h, gc))
		    {
			tempList.addElement(aTrackEvent);
		    }
	    }
	return tempList.elements();
    }

    private void createGraphicContext(Geometry geometry, TrackDataModel model)
    {
	selection = new SequenceSelection(model);

	gc = new SequenceGraphicContext(model, selection, track); //loopback?
	gc.setGraphicSource(this);
	gc.setGraphicDestination(this);

	gc.setAdapter(new MonoDimensionalAdapter(geometry, MONODIMENSIONAL_TRACK_OFFSET));

	gc.setRenderManager(new AbstractRenderer() {
	    /**
	     * returns the first event containg the given point */
	    public Object firstObjectContaining(int x, int y)
		{
		    TrackEvent aTrackEvent;
		    TrackEvent last = null;
		    
		    int time = gc.getAdapter().getInvX(x);
		    
		    for (Enumeration e = track.getTrackDataModel().intersectionSearch(time, time +1); e.hasMoreElements();) 
			
			{      
			    aTrackEvent = (TrackEvent) e.nextElement();
			    
			    if (aTrackEvent.getRenderer().contains(aTrackEvent, x, y, gc))
				last = aTrackEvent;
			}
		    
		    return last;
		}
	    
	    /**
	     * returns an enumeration of all the events whose graphic representation
	     * intersects the given rectangle.
	     */
	    public Enumeration objectsIntersecting(int x, int y, int w, int h) 
		{
		    return MonodimensionalTrackEditor.this.objectsIntersecting(x, y, w, h);
		}
	    
	    
	    
	});
    }



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

    // good old f*****g HTML-style...

    public Dimension getMinimumSize()
    {
	return new Dimension(400, 80);
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(400, 80);
    }


    //--- FricativeTrackEditor fields
    Geometry geometry;
    SequenceGraphicContext gc;
    SequenceSelection selection;
    static int MONODIMENSIONAL_TRACK_OFFSET = 0;

    Track track;
}
