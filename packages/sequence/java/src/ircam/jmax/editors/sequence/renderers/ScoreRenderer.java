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

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.JMaxApplication;

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
  public ScoreRenderer(SequenceGraphicContext theGc) 
  {  
    super();
    gc = theGc;
    gc.setRenderManager(this);
    //gc.setAdapter(new PartitionAdapter());
    {//-- prepares the parameters for the geometry object

	Geometry g = gc.getAdapter().getGeometry();
	g.setXZoom(20);
	g.setYZoom(300);
	g.setYInvertion(true);
	g.setYTransposition(136);//??
    }

    tempList = new MaxVector();

    scoreBackground = new ScoreBackground(gc);
    partitionBackground = new PartitionBackground(gc);

    itsForegroundLayer = new ScoreForeground(gc);

    itsLayers.addElement(scoreBackground);
    itsLayers.addElement(itsForegroundLayer);
  }
  
  public int getViewMode()
  {
    return viewMode;
  }

  public void setViewMode(int mode)
  {
      //per ora solo il background
      if(viewMode!=mode)
      {
	viewMode=mode;
	itsLayers.removeElementAt(0);
	if(viewMode==MidiTrackEditor.PIANOROLL_VIEW)
	  itsLayers.insertElementAt(scoreBackground, 0);
	else
	  itsLayers.insertElementAt(partitionBackground, 0);
      }
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
   * returns the events whose graphic representation contains
   * the given point.
   */
  public Enumeration objectsContaining(int x, int y) 
  {  
    TrackEvent aTrackEvent;

    tempList.removeAllElements();

    double startTime = gc.getAdapter().getInvX(0);
    double endTime = gc.getAdapter().getInvX(gc.getGraphicDestination().getSize().width);

    for (Enumeration e = gc.getDataModel().intersectionSearch(startTime, endTime); e.hasMoreElements();)
      {      
	aTrackEvent = (TrackEvent) e.nextElement();

	if (aTrackEvent.getRenderer().contains(aTrackEvent, x, y, gc))
	  tempList.addElement(aTrackEvent);
      }

    return tempList.elements();
  }

  /**
   * Returns the first event containg the given point.
   * If there are more then two objects, it returns the
   * the topmost in the visual hyerarchy*/
  public Object firstObjectContaining(int x, int y)
  {
    TrackEvent aTrackEvent;
    TrackEvent last = null;

    /*int*/double time = gc.getAdapter().getInvX(x);

    for (Enumeration e = gc.getDataModel().intersectionSearch(time, time +1); e.hasMoreElements();) 
      
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

    tempList.removeAllElements();
    double startTime = gc.getAdapter().getInvX(x);
    double endTime = gc.getAdapter().getInvX(x+w);

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
  int viewMode = MidiTrackEditor.PIANOROLL_VIEW;

  TrackDataModel itsTrackDataModel;

  public ScoreForeground itsForegroundLayer;

  public ScoreBackground scoreBackground;
  PartitionBackground partitionBackground;
  
  public static final int XINTERVAL = 10;
  public static final int YINTERVAL = 3;

  private MaxVector tempList;
}



