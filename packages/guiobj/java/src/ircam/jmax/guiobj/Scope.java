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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

public class Scope extends VectorDisplay
{
  private static final Color scopeBackgroundColor = new Color((float)0.4, (float)0.75, (float)0.8);
  private static final Color scopeMarkerColor = scopeBackgroundColor.darker();
  private static final Color scopeLineColor = new Color((float)0.8196, (float)1.0, (float)0.9569);

  int onset = 0;
  int last = 0;

  public Scope(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
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

    redraw();
  }

  public void setWidth(int w) 
  {
    super.setWidth(w);

    if(onset >= size)
      onset = size - 1;
  }

  public Color getBackgroundColor()
  {
    if (isSelected())
      return scopeBackgroundColor.darker();
    else
      return scopeBackgroundColor;
  }

  public Color getLineColor()
  {
    return scopeLineColor;
  }

  public void drawMarkers(Graphics g, int x, int y)
  {
    int zero = (int)((float)0.5 * (float)(range - 1) + (float)0.5);
      
    if (isSelected())
      g.setColor(scopeMarkerColor.darker());
    else
      g.setColor(scopeMarkerColor);

    if(onset > 0)
      g.drawLine(x + onset, y - zero - 5, x + onset, y - zero + 5);
    
    g.drawLine(x, y - zero, x + size, y - zero);
  }    
}
