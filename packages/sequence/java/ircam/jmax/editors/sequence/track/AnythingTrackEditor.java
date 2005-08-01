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
import ircam.jmax.*;
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
public class AnythingTrackEditor extends TrackBaseEditor
{
  public AnythingTrackEditor(Geometry g, Track trk, boolean isInSequence)
  {
    super(g, trk, isInSequence);
    setBackground(Color.lightGray);
  }

  public void reinit(){}
  
  void createPopupMenu()
  {
    popup = new TrackBasePopupMenu( this, (((FtsGraphicObject)track.getTrackDataModel()) instanceof FtsSequenceObject));
  }

  SequenceGraphicContext createGraphicContext(Geometry geometry, Track track)
  {
    selection = new SequenceSelection(track.getTrackDataModel());
    
    gc = new SequenceGraphicContext(track.getTrackDataModel(), selection, this);
    gc.setGraphicSource(this);
    gc.setGraphicDestination(this);
    ad = new AnythingAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET);
    track.getPropertySupport().addPropertyChangeListener(ad);
    gc.setAdapter(ad);

    renderer = new AnythingTrackRenderer(gc);
    gc.setRenderManager(renderer);
    return gc;
  }

  public void setAdapter(AnythingAdapter adapter)
  {
    track.getPropertySupport().removePropertyChangeListener(ad);	
    track.getPropertySupport().addPropertyChangeListener(adapter);
    gc.setAdapter(adapter);	
    ad = adapter;
  }

  public void setRenderer(AnythingTrackRenderer renderer)
  {
    this.renderer = renderer;
    gc.setRenderManager(renderer);
  }

  void updateEventProperties(Object whichObject, String propName, Object propValue){}
  void updateRange(Object whichObject){}  

  public int getDefaultHeight()
  {
    return ANY_DEFAULT_HEIGHT;
  }

  public int getDefaultViewMode()
  {
    return -1;
  }
  
  public int getInitialHeight()
  {
    return getDefaultHeight();
  }  
  
  public void objectsAdded(int maxTime) 
  {
    repaint();
    selection.deselectAll();
  }
  public void objectChanged(Object whichObject, int index, String propName, Object propValue) 
  {
    if(propName != null)
    {
      updateEventProperties(whichObject, propName, propValue);
      updateRange(whichObject);
    }
    super.objectChanged(whichObject, index, propName, propValue);
  }
  void createListDialog()
  {
    super.createListDialog();
    listDialog.setSize( new Dimension(400, 200));
  }
  
  //--- AnythingTrackEditor fields
  static int MONODIMENSIONAL_TRACK_OFFSET = 0;
  static int ANY_DEFAULT_HEIGHT = 70;

  AnythingAdapter ad;
}




