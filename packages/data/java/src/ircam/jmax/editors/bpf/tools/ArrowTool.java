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

package ircam.jmax.editors.bpf.tools;

import ircam.jmax.editors.bpf.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.ImageIcon;
import javax.swing.undo.*;


/**
 * the tool used to perform the different operations associated
 * with the "arrow" tool, that is:
 * selection, area_selection, deselection, moving.
 */ 
public class ArrowTool extends SelecterTool implements DragListener{

  /**
   * Constructor. 
   */
  public ArrowTool(ImageIcon theImageIcon) 
  {
    super("arrow", theImageIcon);
    
    itsSelectionMover = new BpfSelectionMover(this, 
					      SelectionMover.HORIZONTAL_MOVEMENT |
					      SelectionMover.VERTICAL_MOVEMENT);
  }

  /**
   * called when this tool is unmounted
   */
  public void deactivate(){}
 
  /**
   * a single object has been selected, in coordinates x, y.
   * Starts a move or a clone operation (if ALT is pressed).
   * overrides the abstract SelecterTool.singleObjectSelected */
  void singleObjectSelected(int x, int y, int modifiers)
  {
    mountIModule(itsSelectionMover, startingPoint.x, startingPoint.y);
  }

  /** 
   * a group of objects was selected 
   *overrides the abstract SelecterTool.multipleObjectSelected */
  void multipleObjectSelected()
  {
  }

  /**
   * drag listener called by the SelectionMover UI module,
   * at the end of its interaction.
   * Moves all the selected elements
   */
  public void dragEnd(int x, int y)
  {
    BpfPoint aPoint;
    BpfPoint newPoint;
    BpfGraphicContext bgc = (BpfGraphicContext) gc;
    BpfAdapter a = bgc.getAdapter();

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    // starts a serie of undoable moves
    ((UndoableData) bgc.getDataModel()).beginUpdate();
	    
    for (Enumeration e = bgc.getSelection().getSelected(); e.hasMoreElements();)
	{
	    int prevX = 0;
	    int nextX = 0;
	    float invX;

	    aPoint = (BpfPoint) e.nextElement();
	    
	    FtsBpfObject ftsObj = bgc.getFtsObject();
	    BpfPoint next = ftsObj.getNextPoint(aPoint);
	    if(next!=null)
		nextX = a.getX(next)-1;
	    BpfPoint prev = ftsObj.getPreviousPoint(aPoint.getTime());
	    if(prev!=null)
		prevX = a.getX(prev)+1;
	    
	    if((a.getX(aPoint) + deltaX > nextX)&&(next!=null))
		invX = a.getInvX(nextX);
	    else
		if((a.getX(aPoint) + deltaX < prevX)&&(prev!=null))
		    invX = a.getInvX(prevX);
		else
		    invX = a.getInvX(a.getX(aPoint) + deltaX);
	    
	    bgc.getFtsObject().requestSetPoint(bgc.getFtsObject().indexOf(aPoint), 
					       invX,
					       a.getInvY(a.getY(aPoint)+deltaY));
	}

    mountIModule(itsSelecter);
    gc.getGraphicDestination().repaint();    
  }

  public void updateStartingPoint(int deltaX, int deltaY)
  {
    startingPoint.x+=deltaX;
    startingPoint.y+=deltaY;
  }
  //---Fields
  BpfSelectionMover itsSelectionMover;

  int itsMoveMode;
}






