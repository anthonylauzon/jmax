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

      /* new time equals previous time */
      if(time == ftsObj.getPointAt(index - 1).getTime())
	  {
	      if(index == ftsObj.length() || value == ftsObj.getPointAt(index - 1).getValue())
		  {
		      // do nothing! (don't add jump point at the end or double point)
		  }
	      else if(index == 1 || time == ftsObj.getPointAt(index - 2).getTime())
		  {
		      // set value of previous point (no add!)      
		      ftsObj.requestSetPoint(index-1, time, value); 
		  }
	  }
	  else
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
    BpfPoint aPoint;
    BpfPoint newPoint;
    BpfGraphicContext bgc = (BpfGraphicContext) gc;
    BpfAdapter a = bgc.getAdapter();
    FtsBpfObject ftsObj = bgc.getFtsObject();

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    if(bgc.getSelection().isInSelection(ftsObj.getPointAt(0)))
	deltaX = 0;

    if((deltaX==0)&&(deltaY==0)) 
      {
	mountIModule(itsSelecter);
	gc.getGraphicDestination().repaint();    
	return;
      }
    //clip deltaY///////////////////
    if(deltaY > 0)
	{
	    int minY = a.getY(bgc.getSelection().getMinValueInSelection());
	    int h = bgc.getGraphicDestination().getSize().height;
	    if(minY + deltaY > h)
		deltaY = h - minY;
	}
    else
	{
	    int maxY = a.getY(bgc.getSelection().getMaxValueInSelection());
	    if(maxY + deltaY < 0)
		deltaY = -maxY;
	}

    //clip deltaX///////////////////
    BpfPoint last, first;
    int firstIndex, lastIndex;
    first =  bgc.getSelection().getFirstInSelection();
    firstIndex = bgc.getSelection().getMinSelectionIndex();
    if(bgc.getSelection().size()==1)
	{
	    last = first;
	    lastIndex = firstIndex;
	}
    else
	{
	    last =  bgc.getSelection().getLastInSelection();
	    lastIndex = bgc.getSelection().getMaxSelectionIndex();
	}
    int lastX =  a.getX(last);
    int firstX = a.getX(first);
	
    BpfPoint next = ftsObj.getNextPoint(last);
    BpfPoint prev = ftsObj.getPreviousPoint(first);
    int nextX = -1;
    int prevX = -1;
    if(next!=null)
      nextX = a.getX(next);
    if(prev!=null)
      prevX = a.getX(prev);

    if((lastX+deltaX > nextX)&&(next!=null)) deltaX = nextX-lastX;
	else if((firstX+deltaX < prevX)&&(prev!=null)) deltaX = prevX-firstX;

    // starts a serie of undoable moves
    ((UndoableData) bgc.getDataModel()).beginUpdate();

    //control of Jumps and double points  ///////////////
    
    boolean repeat = false; 
    if(bgc.getSelection().size()>1)
	{
	    if(deltaX<0)
		{
		    if(first.getTime() == ftsObj.getNextPoint(first).getTime())
			repeat = true;
		}		   
	    else
		if(last.getTime() == ftsObj.getPreviousPoint(last).getTime())
		    repeat = true;
	}

    if(firstX+deltaX <= prevX)
	{
	    if(firstIndex == ftsObj.length() - 1 || first.getValue() == prev.getValue())
		{
		    //remove the first selected index and send the rest;
		    bgc.getSelection().deSelect(first);
		    ftsObj.requestPointRemove(firstIndex);
		}
	    else
		if(firstIndex == 1 ||
		   a.getInvX(firstX + deltaX) <= ftsObj.getPointAt(firstIndex - 2).getTime())
		    {
			//remove prev and send the points with index -1
			if(repeat)
			    {
				//remove prev and first
				int[] indexs = {firstIndex, firstIndex-1};
				bgc.getSelection().deSelect(first);
				ftsObj.requestPointsRemove(indexs);	
			    }
			else
			    ftsObj.requestPointRemove(firstIndex-1);		    
			firstIndex--;		    
		    }
		else
		    if(repeat) 
			{
			    bgc.getSelection().deSelect(first);
			    ftsObj.requestPointRemove(firstIndex);
			}
	}
    else if((lastX+deltaX == nextX) && (lastIndex < ftsObj.length() - 1 ))
	{
	    if(last.getValue() == next.getValue())
		{
		    //remove the last selected index and send the rest;
		    bgc.getSelection().deSelect(last);
		    ftsObj.requestPointRemove(lastIndex);
		}
	    else 
		if(lastIndex == ftsObj.length() - 2 ||
		   a.getInvX(lastX + deltaX) >= ftsObj.getPointAt(lastIndex + 2).getTime())
		    {
			// remove next and send all 
			if(repeat)
			    {
				//remove prev and first
				int[] indexs = {lastIndex+1, lastIndex};
				bgc.getSelection().deSelect(last);
				ftsObj.requestPointsRemove(indexs);				
			    }
			else
			    ftsObj.requestPointRemove(lastIndex+1);		    
		    }
		else
		    if(repeat) 
			{
			    bgc.getSelection().deSelect(last);
			    ftsObj.requestPointRemove(lastIndex);
			}
	}
    /////////////////////////////////////////

    int i = 0;
    int selSize = bgc.getSelection().size();
    float[] times = new float[selSize];
    float[] values = new float[selSize];
    for (Enumeration e = bgc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aPoint = (BpfPoint) e.nextElement();
	times[i] = a.getInvX(a.getX(aPoint) + deltaX);
	values[i++] = a.getInvY(a.getY(aPoint)+deltaY);
      }

    ftsObj.requestSetPoints(firstIndex, times, values);

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






