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

package ircam.jmax.editors.explode;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

import java.util.*;
import java.awt.Component;

import javax.swing.ImageIcon;
import javax.swing.undo.*;

/**
 * the tool used to perform an erase operation.
 */
public class DeleteTool extends Tool implements PositionListener {
  
  /**
   * Constructor. 
   */
  public DeleteTool(ImageIcon theImageIcon) 
  {
    super("eraser", theImageIcon);
    
    itsMouseTracker = new VerboseMouseTracker(this);
  }

  
  /**
   * the default Interaction Module
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }
  

  /**
   * called when this tool is unmounted
   */
  public void deactivate() {}

  
  /**
   * called by the MouseTracker
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aEvent = (ScrEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
    if (aEvent != null) 
      {
	ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;
	// starts an undoable transition
	((UndoableData) egc.getDataModel()).beginUpdate();

	if (egc.getSelection().isInSelection(aEvent))
	  {
	    Enumeration e;
	    MaxVector v = new MaxVector();

	    // copy the selected elements in another MaxVector (cannot remove
	    // elements of a Vector inside a loop based on an enumeration of this vector, it simply does'nt work...)
	    for ( e = egc.getSelection().getSelected();
		  e.hasMoreElements();)
	      {
		v.addElement(e.nextElement());
	      }
	    
	    // remove them
	    for (int i = 0; i< v.size(); i++)
	      egc.getDataModel().deleteEvent((ScrEvent)(v.elementAt(i)));

	    v = null;
	  }
	else
	  {
	    egc.getDataModel().deleteEvent(aEvent);
	    egc.getSelection().deselectAll();
	  }

	((UndoableData) egc.getDataModel()).endUpdate();
      }
  } 
  


  //---- Fields

  MouseTracker itsMouseTracker;

}
