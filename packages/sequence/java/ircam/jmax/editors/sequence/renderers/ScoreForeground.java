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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;

import java.awt.*;
import java.util.Enumeration;

/**
* The foreground layer of a score representation */
public class ScoreForeground implements Layer {
	
  public ScoreForeground(SequenceGraphicContext theGc)
{
    gc = theGc;
    //itsEventRenderer = new AmbitusEventRenderer(gc);
}

Rectangle tempRect = new Rectangle();
/**
* render all the visible objects in this layer (all the notes in the window)
 */
public void render(Graphics g, int order) 
{   
	tempRect.setBounds(0,0,gc.getGraphicDestination().getSize().width, 
										 gc.getGraphicDestination().getSize().height);
	
	render(g, tempRect, order);
}

/** Layer interface */
public void render(Graphics g, Rectangle r, int order)
{
	TrackEvent temp;
	
	Rectangle tempr = (Rectangle) g.getClip();
	
	
	g.clipRect(ScoreBackground.KEYEND, 0, gc.getGraphicDestination().getSize().width-ScoreBackground.KEYEND, gc.getGraphicDestination().getSize().height);
	
	
	for (Enumeration e = gc.getRenderManager().objectsIntersecting( r.x, r.y, r.width, r.height); e.hasMoreElements();) 
	{
		temp = (TrackEvent) e.nextElement();
		temp.getRenderer().render( temp, g, gc.getSelection().isInSelection(temp), gc);
	}
	
	if( gc.getGridMode() == TrackEditor.MEASURES_GRID)
		renderMeasures(g);
	
	g.setClip(tempr);
}

private void renderMeasures(Graphics g)
{
	FtsTrackObject markers = gc.getMarkersTrack();
	if( markers!= null)
	{
		TrackEvent evt;
		Dimension d = gc.getGraphicDestination().getSize();
		SequenceSelection sel = gc.getMarkersSelection();
		
    for (Enumeration e = markers.intersectionSearch( gc.getAdapter().getInvX(ScoreBackground.KEYEND), 
																										 gc.getAdapter().getInvX(d.width-ScoreBackground.KEYEND)); e.hasMoreElements();) 
		{
			evt = (TrackEvent) e.nextElement();
			evt.getRenderer().render( evt, g, sel.isInSelection(evt), gc);
		}
	}
}

/** Returns the current EventRenderer */
public ObjectRenderer getObjectRenderer()
{
	return null;
}


//--- Fields
SequenceGraphicContext gc;
ObjectRenderer itsEventRenderer;

}
