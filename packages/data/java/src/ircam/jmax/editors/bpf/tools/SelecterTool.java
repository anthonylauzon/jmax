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

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The base class of all the tools that use a selection
 * to establish the "target" of their action and that have the
 * Selecter as the basic (default) interaction module.
 * It takes care of handling the selection process.
 * Examples of this kind of tools are: 
 * the Arrow tool, the resizer tool, the mover tool.
 * @see ArrowTool
 * @see ResizerTool
 */
public abstract class SelecterTool extends Tool implements GraphicSelectionListener{
  
  public SelecterTool(String theName, ImageIcon theImageIcon)
  {
    super(theName, theImageIcon);

    itsSelecter = new Selecter(this);
  }

  /**
   * the default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsSelecter;
  }


    /**
     * Called at double-click. Interested tools will derive this method
     * with the specific semantic */
    public void edit(int x, int y, int modifiers)
    {
    }

  /**
   * called by the Selecter UI module at mouse down
   */
  public void selectionPointChoosen(int x, int y, int modifiers) 
  {
      if((modifiers & InputEvent.CTRL_MASK)!=0)
	  controlAction(x, y, modifiers);
      else
	  {
	      BpfSelection selection = ((BpfGraphicContext)gc).getSelection();
	      gc.getGraphicDestination().requestFocus();//???

	      BpfPoint point = (BpfPoint) gc.getRenderManager().firstObjectContaining(x, y);
	      if (point != null) 
		  { //click on event
		      startingPoint.setLocation(x,y);

		      if (!selection.isInSelection(point)) 
			  {
			      if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
				  selection.deselectAll();

			      selection.select(point);
			  }
		      else
			  selection.setLastSelectedPoint(point);
	      
		      singleObjectSelected(x, y, modifiers);
		  }
	      else //click on empty
		  if ((modifiers & InputEvent.SHIFT_MASK) == 0)
		      if (!selection.isSelectionEmpty())
			  selection.deselectAll(); 
	  }
  }

  public void selectionPointDoubleClicked(int x, int y, int modifiers) 
    {
	edit(x, y, modifiers);
    }

  public void controlAction(int x, int y, int modifiers) 
    {
	//
    }

  /**
   * called by the selecter UI module
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
      gc.getGraphicDestination().requestFocus();//???

      if (w ==0) w=1;// at least 1 pixel wide
      if (h==0) h=1;
    
      selectArea(x, y, w, h);
    
      multipleObjectSelected();
  }

  /**
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
      BpfGraphicContext bgc = (BpfGraphicContext)gc;
      selectArea(bgc.getRenderManager(), bgc.getSelection(), x, y,  w,  h);
  }

  
  /**
   * Graphically selects all the objects in a given rectangle, given a Render
   * and a Selection
   */
  public static void selectArea(RenderManager r, BpfSelection s, int x, int y, int w, int h) 
  {
      s.select(r.objectsIntersecting(x, y, w, h));
  }

  /**
   * a single object has been selected, in coordinates x, y */
  abstract void singleObjectSelected(int x, int y, int modifiers);

  /** 
   * a group of objects was selected */
  abstract void multipleObjectSelected();

  //--- Fields
  Selecter itsSelecter;
  Point startingPoint = new Point();
}

