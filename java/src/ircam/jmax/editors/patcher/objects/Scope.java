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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

class Scope extends VectorDisplay
{
  int onset = 0;
  int last = 0;

  public Scope(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    super.setBackgroundColor(new Color((float)0.0, (float)0.5, (float)0.4));
    super.setMarkerColor(new Color((float)0.0, (float)0.25, (float)0.2));
    super.setLineColor(Color.cyan);
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if(Squeack.isDown(squeack))
      last = mouse.x;
    else if(Squeack.isDrag(squeack))
      {
	int x = mouse.x;
	int n = x - last;

	if(n != 0)
	  {
	    onset += n;
	    
	    if(onset < 0)
	      onset = 0;
	    else if(onset >= size)
	      onset = size - 1;

	    /* send to server */
	    ((FtsScopeObject)ftsObject).setOnset(onset);

	    last = x;
	  }
      }
  }
}
