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
      if((modifiers & SHORTCUT)!=0)
	  controlAction(x, y, modifiers);
      else
	  {
	      BpfSelection selection = ((BpfGraphicContext)gc).getSelection();
	      gc.getGraphicDestination().requestFocus();//???

	      BpfPoint point = (BpfPoint) gc.getRenderManager().firstObjectContaining(x, y);	      
	      if (point != null) 
		  { //click on point
		      startingPoint.setLocation(x,y);
		      if (!selection.isInSelection(point)) 
			  {
			      if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
				  {
				      selection.deselectAll();
				      selection.select(point);
				  }
			      else
				  {
				      int min = selection.getMinSelectionIndex();
				      int max = selection.getMaxSelectionIndex();
				      int index = ((BpfGraphicContext)gc).getFtsObject().indexOf(point);
				      if((min<0)||(max<0)) selection.select(point);
				      else
					  {
					      if(index < min) 					  
						  selection.setSelectionInterval(index, max);
					      else if(index > max)
						  selection.setSelectionInterval(min, index);
				  
					      selection.setLastSelectedPoint(point);
					  }
				  }
			  }
		      else
			  selection.setLastSelectedPoint(point);
	      
		      singleObjectSelected(x, y, modifiers);
		  }
	      else 
		  if(!clickOnConnection(x, y, modifiers))//click on empty
		      {
			  if ((modifiers & InputEvent.SHIFT_MASK) == 0)
			      if (!selection.isSelectionEmpty())
				  selection.deselectAll(); 
		      }
	  }
      ((BpfGraphicContext)gc).displaySelectionInfo();		    
  }

  public void selectionPointDoubleClicked(int x, int y, int modifiers) 
  {
    edit(x, y, modifiers);
  }

  public void controlAction(int x, int y, int modifiers) 
  {
      //
  }

  public boolean clickOnConnection(int x, int y, int modifiers)
  {
      BpfAdapter a = ((BpfGraphicContext)gc).getAdapter();
      FtsBpfObject data = ((BpfGraphicContext)gc).getFtsObject();
      BpfPoint start, end;

      float time = a.getInvX(x);
      start = data.getPreviousPoint(time);
      if(start!=null) end = data.getNextPoint(start);
      else end = data.getNextPoint(time);
	  
      if( start == null || end == null) return false;

      int startX = a.getX(start);
      int startY = a.getY(start);
      int endX = a.getX(end);
      int endY = a.getY(end);

      boolean down  = (startX <= endX);
      boolean right = (startY <= endY);

      boolean onConnection = false;

      if (down)
	  {
	      if ((x < (startX - 4)) || (x > (endX + 4)))
		  onConnection = false;
	  }
      else
	  {
	      if ((x < (endX - 4)) || (x > (startX + 4)))
		  onConnection = false;
	  }
      
      if (right)
	  {
	      if ((y < startY - 4) || (y > endY + 4))
		  onConnection = false;
	  }
      else
	  {
	      if ((y < endY - 4) || (y > startY + 4))
		  onConnection = false;
	  }
      
      float length = (float) Math.sqrt((startX - endX)*(startX - endX) + (startY - endY)*(startY - endY));
      float z = (float) ((startY - y) * (endX - startX) - (startX - x) * (endY - startY));
      
      if (z > 0.0)
	  onConnection = ((z/length) < 4.0);
      else
	  onConnection = ((z/length) > -4.0);
  
      if(onConnection)
	  {
	      BpfSelection selection = ((BpfGraphicContext)gc).getSelection();

	      startingPoint.setLocation(x,y);
	      
	      if (!(selection.isInSelection(start)&&selection.isInSelection(end))) 
		  {
		      if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
			  {
			      selection.deselectAll();
			      selection.setSelectionInterval(data.indexOf(start), data.indexOf(end));
			  }
		      else
			  {
			      int min = selection.getMinSelectionIndex();
			      int max = selection.getMaxSelectionIndex();
			      int index = ((BpfGraphicContext)gc).getFtsObject().indexOf(start);
			      if((min<0)||(max<0))  selection.setSelectionInterval(index, index+1);
			      else
				  {
				      if(index < min) 					  
					  selection.setSelectionInterval(index, max);
				      else if(index+1 > max)
					  selection.setSelectionInterval(min, index+1);
				      
				      selection.setLastSelectedPoint(end);
				  }
			  }
		  }
	      else
		  selection.setLastSelectedPoint(end);
		
	      singleObjectSelected(x, y, modifiers);
	  }

      return onConnection;
  }

  /**
   * called by the selecter UI module
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
      gc.getGraphicDestination().requestFocus();//???

      if((w==0)&&(h==0)) return;
    
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
  public static int SHORTCUT = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
}

