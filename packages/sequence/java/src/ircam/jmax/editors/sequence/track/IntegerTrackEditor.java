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
public class IntegerTrackEditor extends MonoTrackEditor
{
    public IntegerTrackEditor(Geometry g, Track track)
    {
	super(g, track);

	if(track.getProperty("maximumValue")==null)
	    track.setProperty("maximumValue", new Integer(IntegerValue.DEFAULT_MAX_VALUE));
	if(track.getProperty("minimumValue")==null)
	    track.setProperty("minimumValue", new Integer(IntegerValue.DEFAULT_MIN_VALUE));
	if(track.getProperty("viewMode")==null)
	    track.setProperty("viewMode", new Integer(viewMode));

	((MonoDimensionalAdapter)gc.getAdapter()).setLabelMapper(IntegerLabelMapper.getMapper());
	setRenderer(new IntegerTrackRenderer(gc));

	super.setAdapter(new IntegerAdapter(geometry, gc, MONODIMENSIONAL_TRACK_OFFSET));
    }
    
    /*public JPopupMenu getMenu()
      {
      MonoTrackPopupMenu.getInstance().update(this);
      return MonoTrackPopupMenu.getInstance();
      }*/

    int viewMode = PEAKS_VIEW;
    static public final int PEAKS_VIEW = 2;
    static public final int STEPS_VIEW = 3;
    static public final int BREAK_POINTS_VIEW = 4;
}


