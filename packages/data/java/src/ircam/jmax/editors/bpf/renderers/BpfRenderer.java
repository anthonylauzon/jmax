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

package ircam.jmax.editors.bpf.renderers;

import ircam.jmax.editors.bpf.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.MaxApplication;

/**
 * The main class for a monodimensionalTrack representation.
 * It provides the support for  editing,
 * using a background layer and a foreground.
 * The grid is rendered in the MonoTrackBackground
 * The events are painted by the MonoTrackForeground.
 */
public class BpfRenderer extends AbstractRenderer{
  
  /**
   * Constructor.
   */
  public BpfRenderer(BpfGraphicContext theGc) 
  {  
    super();
    gc = theGc;

    tempList = new MaxVector();

    itsForegroundLayer = new BpfForeground(gc);

    itsLayers.addElement(new BpfBackground(gc));
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
      BpfPoint pt;
      BpfPoint last = null;
		    
      float time = gc.getAdapter().getInvX(x);

      for (Enumeration e = gc.getDataModel().intersectionSearch(time -1, time +1,gc.getAdapter()); e.hasMoreElements();) 
	  {      
	      pt = (BpfPoint) e.nextElement();
	      
	      if (PointRenderer.getRenderer().contains(pt, x, y, gc))
		  last = pt;
	  }
      return last;
  }

  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle.
   */
  public Enumeration objectsIntersecting(int x, int y, int w, int h) 
  {
    BpfPoint point;
    
    float startTime = gc.getAdapter().getInvX(x);
    float endTime = gc.getAdapter().getInvX(x+w);

    tempList.removeAllElements();

    for (Enumeration e = gc.getDataModel().intersectionSearch(startTime, endTime, gc.getAdapter()); e.hasMoreElements();) 
    {
	point = (BpfPoint) e.nextElement();
	
	if (PointRenderer.getRenderer().touches(point, x, y, w, h, gc))
	    {
		tempList.addElement(point);
	    }
    }
    return tempList.elements();
  }


  //------------------  Fields
  BpfGraphicContext gc;

  BpfForeground itsForegroundLayer;
  
  protected MaxVector tempList;
}





