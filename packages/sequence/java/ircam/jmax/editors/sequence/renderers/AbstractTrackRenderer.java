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

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.JMaxApplication;

/**
 * The main class for a monodimensionalTrack representation.
 * It provides the support for  editing,
 * using a background layer and a foreground.
 * The grid is rendered in the MonoTrackBackground
 * The events are painted by the MonoTrackForeground.
 */
public class AbstractTrackRenderer extends AbstractRenderer{
  
  /**
   * Constructor.
   */
  public AbstractTrackRenderer(SequenceGraphicContext theGc) 
  {  
    super();
		gc = theGc;
		tempList = new MaxVector();
  }
  
	public Object firstMarkerContaining(int x, int y)
  {
		TrackEvent aTrackEvent;
		TrackEvent last = null;
  
		double startTime = gc.getAdapter().getInvX(x);
		double endTime = gc.getAdapter().getInvX(x+AmbitusEventRenderer.CUE_WIDTH+2);

		/* search for markers */
		if(gc.getMarkersTrack() != null)
		{
			for (Enumeration e = gc.getMarkersTrack().intersectionSearch(startTime, endTime); e.hasMoreElements();) 
			{      
				aTrackEvent = (TrackEvent) e.nextElement();
			
				if (aTrackEvent.getRenderer().contains(aTrackEvent, x, y, gc))
					last = aTrackEvent;
			}
		}	
		return last;
	}

	public Enumeration markersIntersecting(int x, int y, int w, int h) 
	{
		TrackEvent aTrackEvent;
	
		tempList.removeAllElements();
		double startTime = gc.getAdapter().getInvX(x);
		double endTime = gc.getAdapter().getInvX(x+w);
  
		/* search for markers */
		if(gc.getMarkersTrack() != null)
		{
			for (Enumeration e = gc.getMarkersTrack().intersectionSearch(startTime, endTime); e.hasMoreElements();) 
			{
				aTrackEvent = (TrackEvent) e.nextElement();
		
				if (aTrackEvent.getRenderer().touches(aTrackEvent, x, y, w, h, gc))
					tempList.addElement(aTrackEvent);
			}
		}
		return tempList.elements();
	}
//------------------  Fields
SequenceGraphicContext gc;
protected MaxVector tempList;
}





