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

package ircam.jmax.editors.explode;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;

/**
 * The main class for a score representation.
 * It provides the support for piano-roll editing,
 * using a background layer and a foreground.
 * The grid is rendered in the ScoreBackground
 * The events are painted by the ScoreForeground.
 */
public class ScoreRenderer extends AbstractRenderer{
  
  /**
   * Constructor.
   */
  public ScoreRenderer(ExplodeGraphicContext theGc) 
  {  
    super();
    gc = theGc;
    gc.setAdapter(new PartitionAdapter(gc));
    {//-- prepares the parameters for the adapter
      gc.getAdapter().setXZoom(20);
      gc.getAdapter().setYZoom(300);
      gc.getAdapter().setYInvertion(true);
      gc.getAdapter().setYTransposition(136);
    }

    tempList = new MaxVector();

    itsForegroundLayer = new ScoreForeground(gc);

    itsLayers.addElement(new ScoreBackground(gc));
    itsLayers.addElement(itsForegroundLayer);
  }
  


  /**
   * returns its (current) event renderer
   */
  public ObjectRenderer getObjectRenderer() 
  {
    return itsForegroundLayer.getObjectRenderer();
  }


  
  /**
   * returns the events whose graphic representation contains
   * the given point.
   */
  public Enumeration objectsContaining(int x, int y) 
  {  
    ScrEvent aScrEvent;

    tempList.removeAllElements();

    int startTime = gc.getAdapter().getInvX(0);
    int endTime = gc.getAdapter().getInvX(gc.getGraphicDestination().getSize().width);

    for (Enumeration e = gc.getDataModel().intersectionSearch(startTime, endTime); e.hasMoreElements();)
      {      
	aScrEvent = (ScrEvent) e.nextElement();

	if (getObjectRenderer().contains(aScrEvent, x, y, gc))
	  tempList.addElement(aScrEvent);
      }

    return tempList.elements();
  }

  /**
   * Returns the first event containg the given point.
   * If there are more then two objects, it returns the
   * the topmost in the visual hyerarchy*/
  public Object firstObjectContaining(int x, int y)
  {
    ScrEvent aScrEvent;
    ScrEvent last = null;

    int time = gc.getAdapter().getInvX(x);

    for (Enumeration e = gc.getDataModel().intersectionSearch(time, time +1); e.hasMoreElements();) 
      
      {      
	aScrEvent = (ScrEvent) e.nextElement();

	if (getObjectRenderer().contains(aScrEvent, x, y, gc))
	  last = aScrEvent;
      }

    return last;
  }

  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle.
   */
  public Enumeration objectsIntersecting(int x, int y, int w, int h) 
  {
    ScrEvent aScrEvent;

    tempList.removeAllElements();
    int startTime = gc.getAdapter().getInvX(x);
    int endTime = gc.getAdapter().getInvX(x+w);


    for (Enumeration e = gc.getDataModel().intersectionSearch(startTime, endTime); e.hasMoreElements();) 
      {
	aScrEvent = (ScrEvent) e.nextElement();

	if (getObjectRenderer().touches(aScrEvent, x, y, w, h, gc))
	  {
	    tempList.addElement(aScrEvent);
	  }
      }
    return tempList.elements();
  }


  //------------------  Fields
  ExplodeGraphicContext gc;

  ExplodeDataModel itsExplodeDataModel;

  ScoreForeground itsForegroundLayer;
  
  public static final int XINTERVAL = 10;
  public static final int YINTERVAL = 3;

  private MaxVector tempList;
}



