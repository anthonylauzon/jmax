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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
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
public class FmatTrackEditor extends MonoTrackEditor
{
  public FmatTrackEditor(Geometry g, Track track, boolean isInsequence)
  {
    super(g, track, isInsequence);
    viewMode = FMAT_VIEW;
    
    super.setAdapter(new FmatAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET));
    
    g.addZoomListener( new ZoomListener() {
			public void zoomChanged(float zoom, float oldZoom)
		  {
				updateEventsLength();
			}
		});
  }

  void displayMousePosition(int x, int y)
  {
    double time = gc.getAdapter().getInvX( x);
    if(time < 0) time = 0;	          
    gc.getDisplayer().display( Displayer.numberFormat.format(time));	  
  }
  
  void createPopupMenu()
  {
    popup = new TrackBasePopupMenu( this, (((FtsGraphicObject)track.getTrackDataModel()) instanceof FtsSequenceObject));
  }

  public int getDefaultHeight()
  {
    return FMAT_DEFAULT_HEIGHT;
  }
  
  public void setViewMode(int mode){}
  public int getViewMode(){return FMAT_VIEW;}
  
  void doEdit(Event evt, int x, int y){}
  
  //update "duration" in order to have the same graphic length with the new zoom value
  //for all events in track  (called at zoom change). Called also at object instantiation
  //to avoid the problem of the gc==null and so setting the good duration

  void updateEventsLength()
  {
    TrackEvent aTrackEvent;
    for (Enumeration e = gc.getDataModel().getEvents(); e.hasMoreElements();) 
	  {      
      aTrackEvent = (TrackEvent) e.nextElement();
      ((FmatValue)aTrackEvent.getValue()).updateLength(aTrackEvent, gc); 
	  }
  }
  
  public void updateNewObject(Object obj)
  {
    TrackEvent evt = (TrackEvent)obj;
    ((FmatValue)(evt.getValue())).updateLength(evt, gc); 
  }
  
  void uploadEnd()
  {
    updateEventsLength();
  }
  
  static public final int FMAT_VIEW = 5;
  static public int FMAT_DEFAULT_HEIGHT = 70;
}














