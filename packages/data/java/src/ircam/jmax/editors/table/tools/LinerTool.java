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

package ircam.jmax.editors.table.tools;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The class used to implement the behaviour of the "line" tool of the 
 * Table editor. It uses a LineDrawer interaction module.
 */
public class LinerTool extends TableTool implements LineListener{
  
  /**
   * constructor */
  public LinerTool(ImageIcon theImageIcon)
  {
    super("liner", theImageIcon);

    itsLiner = new LineDrawer(this);
  }

    /**
     * Redefined to deselect all at activation time */
    public void reActivate(GraphicContext gc) 
    {
	super.reActivate(gc);
	getGc().getSelection().deselectAll();
    }

  /**
   * The default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsLiner;
  }


  /**
   * LineListener interface. 
   * Called by the lineDrawer UI module at mouse down.
   */
  public void lineStart(int x, int y, int modifiers) 
  {
    getGc().getFtsObject().beginUpdate();
    startingPoint.setLocation(x, y);
  }

  /**
   * LineListener interface. Called by the LineDrawer UI module.
   */
  public void lineEnd(int x, int y) 
  {
    drawLine(startingPoint.x, startingPoint.y, x, y);
    getGc().getFtsObject().requestEndEdit();
  }

  /**
   * An utility function to draw a line between two given points in the given
   * graphic context. The points are expressed in screen coordinates.
   */
  public void drawLine(int x1, int y1, int x2, int y2)
  {
    TableAdapter ta = getGc().getAdapter();

    if (x1 > x2) //the line is given 'a l'inverse'
      {
	int temp;
	temp = y1;
	y1 = y2;
	y2 = temp;
	temp = x1;
	x1 = x2;
	x2 = temp;
      }

    int start = ta.getInvX(x1); 
    int end = ta.getInvX(x2);

    getGc().getDataModel().interpolate(start, end, ta.getInvY(y1), ta.getInvY(y2));
  }

  //--- Fields
  LineDrawer itsLiner;

  Point startingPoint = new Point();
  
}







