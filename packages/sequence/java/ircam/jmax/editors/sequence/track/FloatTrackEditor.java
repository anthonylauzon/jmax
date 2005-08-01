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
public class FloatTrackEditor extends MonoTrackEditor
{
  public FloatTrackEditor(Geometry g, Track trk, boolean isInSequence)
  {
    super(g, trk, isInSequence);
    
    if(track.getProperty("maximumValue")==null)
      track.setProperty("maximumValue", new Integer(FloatValue.DEFAULT_MAX_VALUE));
    if(track.getProperty("minimumValue")==null)
      track.setProperty("minimumValue", new Integer(FloatValue.DEFAULT_MIN_VALUE));
    if(track.getProperty("viewMode")==null)
      track.setProperty("viewMode", new Integer(viewMode));
      
    setRenderer(new IntegerTrackRenderer(gc));
    super.setAdapter(new FloatAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET));
  }
  
  public void reinit()
  {
    track.setProperty("maximumValue", new Integer(FloatValue.DEFAULT_MAX_VALUE));
    track.setProperty("minimumValue", new Integer(FloatValue.DEFAULT_MIN_VALUE));
    setViewMode(PEAKS_VIEW);
  }
    
  void updateRange(Object obj)
  {
    int max = ((IntegerAdapter)gc.getAdapter()).getMaximumValue();	
    int min = ((IntegerAdapter)gc.getAdapter()).getMinimumValue();
    float value = ((Float)((TrackEvent)obj).getProperty("value")).floatValue();
    
    if(value>(float)max) track.setProperty("maximumValue", new Integer((int)value+1));
    if(value<(float)min) track.setProperty("minimumValue", new Integer((int)value-1));
  }

  public int getDeafultViewMode()
  {
    return PEAKS_VIEW;
  }   
  
  int viewMode = -1;
  static public final int PEAKS_VIEW = 2;
  static public final int STEPS_VIEW = 3;
  static public final int BREAK_POINTS_VIEW = 4;
}




