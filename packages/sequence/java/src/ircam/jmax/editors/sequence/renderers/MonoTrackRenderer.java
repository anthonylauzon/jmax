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
public class MonoTrackRenderer extends AbstractRenderer{
  
  /**
   * Constructor.
   */
  public MonoTrackRenderer(SequenceGraphicContext theGc) 
  {  
    super();
    gc = theGc;

    tempList = new MaxVector();

    itsForegroundLayer = new MonoTrackForeground(gc);

    itsLayers.addElement(new MonoTrackBackground(gc));
    itsLayers.addElement(itsForegroundLayer);
  }

  /**
   * returns its (current) event renderer
   */
  public ObjectRenderer getObjectRenderer() 
  {
      return null;
      //the renderer depends from the object, here...
      //return itsForegroundLayer.getObjectRenderer();
  }
  
  /**
   * Returns the first event containg the given point.
   * If there are more then two objects, it returns the
   * the topmost in the visual hyerarchy*/
  public Object firstObjectContaining(int x, int y)
  {
      TrackEvent aTrackEvent;
      TrackEvent last = null;
		    
      double time = gc.getAdapter().getInvX(x);

      for (Enumeration e = gc.getDataModel().intersectionSearch(time -1, time +1); e.hasMoreElements();) 
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
    TrackEvent aTrackEvent;
    
    double startTime = gc.getAdapter().getInvX(x);
    double endTime = gc.getAdapter().getInvX(x+w);

    tempList.removeAllElements();

    for (Enumeration e = gc.getDataModel().intersectionSearch(startTime, endTime); e.hasMoreElements();) 
    {
	aTrackEvent = (TrackEvent) e.nextElement();
	
	if (aTrackEvent.getRenderer().touches(aTrackEvent, x, y, w, h, gc))
	{
	    tempList.addElement(aTrackEvent);
	}
    }
    return tempList.elements();
  }


  //------------------  Fields
  SequenceGraphicContext gc;

  MonoTrackForeground itsForegroundLayer;
  
  protected MaxVector tempList;
}





