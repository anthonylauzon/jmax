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
public class ArrowTool extends SelecterTool implements DirectionListener, DragListener{

  /**
   * Constructor. 
   */
  public ArrowTool(ImageIcon theImageIcon) 
  {
    super("arrow", theImageIcon);
    
    itsDirectionChooser = new DirectionChooser(this);
    itsSelectionMover = new SequenceSelectionMover(this, SelectionMover.HORIZONTAL_MOVEMENT);
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
    if ((modifiers & InputEvent.ALT_MASK) == 0)
      itsMoveMode = SIMPLE;
    else itsMoveMode = CLONE;
    
    mountIModule(itsDirectionChooser, x, y);
  }

  /** 
   * a group of objects was selected 
   *overrides the abstract SelecterTool.multipleObjectSelected */
  void multipleObjectSelected()
  {
  }


    public void edit(int x, int y, int modifiers)
    {
	TrackEvent aTrackEvent = (TrackEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
	if (aTrackEvent != null) 
	    { //click on event
		aTrackEvent.getValue().edit(x, y, modifiers, aTrackEvent, (SequenceGraphicContext)gc);

		gc.getGraphicDestination().repaint();
	    }
    }


  /**
   * called by the DirectionChooser UI module
   */
  public void directionChoosen(int theDirection) 
  {
    itsSelectionMover.setDirection(theDirection);
    mountIModule(itsSelectionMover, startingPoint.x, startingPoint.y);

  }
  
  /**
   * called by the DirectionChooser UI module
   */
  public void directionAbort()
  {
    mountIModule(itsSelecter);
  }

  /**
   * drag listener called by the SelectionMover UI module,
   * at the end of its interaction.
   * Moves all the selected elements
   */
  public void dragEnd(int x, int y)
  {
    TrackEvent aEvent;
    TrackEvent newEvent;
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
    Adapter a = egc.getAdapter();

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;
    
    if(deltaX != 0) 
	{	    
	    // starts a serie of undoable moves
	    ((UndoableData) egc.getDataModel()).beginUpdate();
	    
	    if(!a.isHorizontalMovementBounded())
		{
		    int i = 0;
		    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
			{
			    aEvent = (TrackEvent) e.nextElement();		    
			    FtsTrackObject.sendArgs[i].setObject(aEvent);
			    FtsTrackObject.sendArgs[i+1].setDouble(a.getInvX(a.getX(aEvent)+deltaX));
			    i+=2;
			}
		    egc.getTrack().getFtsTrack().sendMessage(FtsObject.systemInlet, "move_events", i, FtsTrackObject.sendArgs);
		}
	    else
		for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
		    {
			int prevX = 0;
			int nextX = 0;
			
			aEvent = (TrackEvent) e.nextElement();
			
			FtsTrackObject ftsTrk = egc.getTrack().getFtsTrack();
			TrackEvent next = ftsTrk.getNextEvent(aEvent);
			if(next!=null)
			    nextX = a.getX(next)-1;
			TrackEvent prev = ftsTrk.getPreviousEvent(aEvent);
			if(prev!=null)
			    prevX = a.getX(prev)+1;
			
			if((a.getX(aEvent) + deltaX > nextX)&&(next!=null))
			    a.setX(aEvent, nextX);
			else
			    if((a.getX(aEvent) + deltaX < prevX)&&(prev!=null))
				a.setX(aEvent, prevX);
			    else
				a.setX(aEvent, a.getX(aEvent) + deltaX);
		    }
	}
    if(deltaY!=0)
	{
	    ((UndoableData) egc.getDataModel()).beginUpdate();
	    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
		{
		    aEvent = (TrackEvent) e.nextElement();
		    a.setY(aEvent, a.getY(aEvent)+deltaY);
		}    
	    ((UndoableData) egc.getDataModel()).endUpdate();
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
  MaxVector times = new MaxVector();
  MaxVector movingEvents = new MaxVector();

  DirectionChooser itsDirectionChooser;
  SelectionMover itsSelectionMover;

  final static int SIMPLE = 0;
  final static int CLONE = 1;
  int itsMoveMode;
}





