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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.bpf.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.util.*;

/**
 * The tool used to add an event in a track.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 * The type of object to add is given by the track's type.
 */ 
public class AdderTool extends Tool implements PositionListener {

  /**
   * Constructor with an Icon.
   */
  public AdderTool(ImageIcon theImageIcon) 
  {
      super("adder", theImageIcon);
      
      itsMouseTracker = new MouseTracker(this);
  }

    /**
     * the default interaction module for this tool
     */
    public InteractionModule getDefaultIM() 
    {
	return itsMouseTracker;
    }

    /**
     * called when this tool is "unmounted"
     */
    public void deactivate() 
    {
    }

    int remember_x; 
    int remember_y;

    public Cursor getCursor()
    {
	return Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR);
    }

    /**
     *PositionListener interface
     */
    public void positionChoosen(int x, int y, int modifiers) 
    {
	// starts an undoable transition	
	((UndoableData) ((BpfGraphicContext)gc).getDataModel()).beginUpdate();
	//endUpdate is called in addEvents in dataModel

	//with Shift add to selection
	if((modifiers & InputEvent.SHIFT_MASK) == 0) ((BpfGraphicContext)gc).getSelection().deselectAll();
	       
	addPoint(x, y);
    }
        
    void addPoint(int x, int y)
    {
	BpfGraphicContext bgc = (BpfGraphicContext) gc;
	FtsBpfObject ftsObj = bgc.getFtsObject();

	float time = bgc.getAdapter().getInvX(x);

	float value = bgc.getAdapter().getInvY(y);

	int index = ftsObj.getNextPointIndex(time);

	/* new time equals previous time */
	if(time == ftsObj.getPointAt(index - 1).getTime())
	    {
		if(index == ftsObj.length() || value == ftsObj.getPointAt(index - 1).getValue())
		    {
			/* do nothing! (don't add jump point at the end or double point) */
			return;
		    }
		else if(index == 1 || time == ftsObj.getPointAt(index - 2).getTime())
		    {
			/* set value of previous point (no add!) */     
			ftsObj.requestSetPoint(index-1, time, value);
			return;
		    }
	    }
	else
	    ftsObj.requestPointCreation(index, time, value); 
    }

  //-------------- Fields
  MouseTracker itsMouseTracker;
}











