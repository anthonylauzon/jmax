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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;


/**
 * the tool used to resize a selection of events.
 * It uses a SelectionResizer to actually resize the
 * objects.
 */
public class ResizerTool extends SelecterTool implements DragListener {

  /**
   * constructor.
   */
  public ResizerTool(ImageIcon theIcon) 
  {
    super("Resizer", theIcon);
    
    itsSelectionResizer = new SequenceSelectionResizer(this);

  }


  /**
   * called when this module is "unmounted"
   */
  public void deactivate() 
  {
  }

  /**
   * a single object has been selected, in coordinates x, y:
   * Mount the resizer interaction module.
   * Overrides the abstract SelecterTool.singleObjectSelected */
  public void singleObjectSelected(int x, int y, int modifiers) 
  {
    mountIModule(itsSelectionResizer, x, y);
  }

  /** 
   * a group of objects was selected 
   *overrides the abstract SelecterTool.multipleObjectSelected */
  void multipleObjectSelected()
  {
  }

  /**
   * drag listening, called by the SelectionResizer UI Module.
   * Resizes all the selected objects,
   * and then mount its default UI Module
   */
  public void dragEnd(int x, int y) 
  {
    TrackEvent aEvent;

    int deltaX = x-startingPoint.x;
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;

    // starts a serie of undoable transitions
    //((UndoableData) egc.getDataModel()).beginUpdate();
    
    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (TrackEvent) e.nextElement();

	if (egc.getAdapter().getLenght(aEvent)+deltaX > 0)
	  egc.getAdapter().setLenght(aEvent, egc.getAdapter().getLenght(aEvent)+deltaX);
      }

    //((UndoableData) egc.getDataModel()).endUpdate();
    
    mountIModule(itsSelecter);

  }


  //------------ Fields
  SelectionResizer itsSelectionResizer;

}



