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
    super("edit tool", theImageIcon);
    
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

    /*
     * click on the editor with CTRL key pressed add a point 
     */
  public void controlAction(int x, int y, int modifiers)
  {
      mountIModule(itsSelectionMover);
      
      BpfGraphicContext bgc = (BpfGraphicContext) gc;
      FtsBpfObject ftsObj = bgc.getFtsObject();

      ((UndoableData) ftsObj).beginUpdate();

      //with Shift add to selection
      if((modifiers & InputEvent.SHIFT_MASK) == 0) bgc.getSelection().deselectAll();
	       
      float time = bgc.getAdapter().getInvX(x);
      float value = bgc.getAdapter().getInvY(y);
      int index = ftsObj.getNextPointIndex(time);

      ftsObj.requestPointCreation(index, time, value); 
  
      mountIModule(itsSelecter);
  }
  /**
   * drag listener called by the SelectionMover UI module,
   * at the end of its interaction.
   * Moves all the selected elements
   */
  public void dragEnd(int x, int y)
  {
    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    if((deltaX==0)&&(deltaY==0)) 
      {
	mountIModule(itsSelecter);
	gc.getGraphicDestination().repaint();    
	return;
      }

    ((BpfGraphicContext)gc).getSelection().moveSelection(deltaX, deltaY, (BpfGraphicContext)gc);
    
    mountIModule(itsSelecter);
    gc.getGraphicDestination().repaint();    
  }

  public void updateStartingPoint(int deltaX, int deltaY)
  {
    startingPoint.x+=deltaX;
    startingPoint.y+=deltaY;
  }

  public void edit(int x, int y, int modifiers)
  {
      ((BpfEditor)gc.getGraphicDestination()).showListDialog();
  }
  //---Fields
  BpfSelectionMover itsSelectionMover;

  int itsMoveMode;
}






