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

import java.util.*;
import java.awt.*;

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
    
    itsMouseTracker = new MouseTracker(this);
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

  public Cursor getCursor()
  {
    return Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR);
  }
  
  /**
   * called by the MouseTracker
   */
    public void positionChoosen(int x, int y, int modifiers) 
    {
	BpfGraphicContext bgc = (BpfGraphicContext) gc;

	BpfPoint point = (BpfPoint) bgc.getRenderManager().firstObjectContaining(x, y);
	
	if (point != null) 
	    {
		// starts an undoable transition	
		((UndoableData) bgc.getDataModel()).beginUpdate();

		if( bgc.getSelection().isInSelection(point))
		    bgc.getSelection().deleteAll();
		else
		    {
			bgc.getFtsObject().requestPointRemove(bgc.getDataModel().indexOf(point));		
			bgc.getSelection().deselectAll();
		    }
	    }
    } 

    //---- Fields

    MouseTracker itsMouseTracker;
}







