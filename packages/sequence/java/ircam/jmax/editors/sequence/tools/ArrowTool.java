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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

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
    super("edit", theImageIcon);
    
    itsDirectionChooser = new DirectionChooser(this);
    itsSelectionMover = new SequenceSelectionMover(this, SelectionMover.HORIZONTAL_MOVEMENT);
    itsSelectionResizer = new SequenceSelectionResizer(this);
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
	mountIModule(itsDirectionChooser, x, y);
}

/** 
* a group of objects was selected 
*overrides the abstract SelecterTool.multipleObjectSelected */
void multipleObjectSelected()
{
}

/******************************************************************************
*********** AddingEvents *****************************************************
******************************************************************************/
public void controlAction(int x, int y, int modifiers)
{
	mountIModule(itsSelectionMover);
	
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	egc.getTrack().setProperty("selected", Boolean.TRUE);
	
	if(egc.getDataModel().isLocked()) return;
	
	// starts an undoable transition
	((UndoableData) egc.getDataModel()).beginUpdate();
	//endUpdate is called in addEvents in dataModel
	
	//with Shift add to selection
	if((modifiers & InputEvent.SHIFT_MASK) == 0) egc.getSelection().deselectAll();
	
	ValueInfo info = egc.getTrack().getTrackDataModel().getType();
	addEvent(x, y, (EventValue) info.newInstance());
	
	mountIModule(itsSelecter);
}

void addEvent(int x, int y, EventValue value)
{
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	UtilTrackEvent aEvent = new UtilTrackEvent(value, egc.getDataModel());
	
	egc.getAdapter().setX(aEvent, x);
	
	egc.getAdapter().setY(aEvent, y);
	
	egc.getTrack().getFtsTrack().requestEventCreation((float)aEvent.getTime(),
																										value.getValueInfo().getName(),
																										value.getDefinedPropertyCount()*2,
																										value.getDefinedPropertyNamesAndValues());
}

/***********************************************************************************************
************************************************************************************************/

public void selectionPointDoubleClicked(int x, int y, int modifiers) 
{
	super.selectionPointDoubleClicked(x, y, modifiers);
  
  TrackEvent evt = (TrackEvent) gc.getRenderManager().firstObjectContaining(x, y);
	if (evt == null)
		((SequenceGraphicContext)gc).getTrackEditor().showListDialog(); //double click on backGround
		else 
      if(!((SequenceGraphicContext)gc).getAdapter().isEditable(evt))
        ((SequenceGraphicContext)gc).getTrackEditor().showListDialog();//double click on event
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
public void directionChoosen(int theDirection, int modifiers) 
{
	if((modifiers & SHORTCUT)!=0 && (modifiers & MouseEvent.ALT_MASK)!=0)
	{
		itsSelectionResizer.setDirection(theDirection);
		mountIModule(itsSelectionResizer, startingPoint.x, startingPoint.y);
		resizing = true;
	}
	else
	{
		itsSelectionMover.setDirection(theDirection);
		mountIModule(itsSelectionMover, startingPoint.x, startingPoint.y);
	}
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
public void dragEnd(int x, int y, MouseEvent evt)
{
	int deltaY = y-startingPoint.y;
	int deltaX = x-startingPoint.x;
	
	if(resizing)
	{
		resizeSelection(deltaX, deltaY);
		resizing = false;	    
	}
	else
		moveSelection(deltaX, deltaY);
	
	mountIModule(itsSelecter);
	gc.getGraphicDestination().repaint();    
}

public void updateStartingPoint(int deltaX, int deltaY)
{
	startingPoint.x+=deltaX;
	startingPoint.y+=deltaY;
}

void resizeSelection(int deltaX, int deltaY)
{
	TrackEvent aEvent;
	
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	
	// starts a serie of undoable transitions
	((UndoableData) egc.getDataModel()).beginUpdate();
	
	if( deltaX != 0)
		for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
		{
			aEvent = (TrackEvent) e.nextElement();
			
			if (egc.getAdapter().getLenght( aEvent) + deltaX > 0)
				egc.getAdapter().setLenght( aEvent, egc.getAdapter().getLenght( aEvent) + deltaX);
		}
			else
				if( deltaY != 0)
					for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
					{
						aEvent = (TrackEvent) e.nextElement();
						
						if ( egc.getAdapter().getHeigth( aEvent) - deltaY >= 0)
							egc.getAdapter().setHeigth( aEvent, egc.getAdapter().getHeigth( aEvent) - deltaY);
						else
							egc.getAdapter().setHeigth( aEvent, 0);
					}          
						
						egc.getTrack().getFtsTrack().requestNotifyEndUpdate();
}

void moveSelection(int deltaX, int deltaY)
{  
	TrackEvent aEvent;
	TrackEvent newEvent;
	SequenceGraphicContext egc = (SequenceGraphicContext) gc;
	Adapter a = egc.getAdapter();
  
	if(deltaX != 0) 
	{	    
		((UndoableData) egc.getDataModel()).beginUpdate();
		
		if(a.isHorizontalMovementAllowed())
    {
			if(!a.isHorizontalMovementBounded())
				egc.getTrack().getFtsTrack().requestEventsMove(egc.getSelection().getSelected(), deltaX, a);
			else
			{
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
    }
    if(egc.getMarkersSelection() != null)
      egc.getMarkersTrack().requestEventsMove( egc.getMarkersSelection().getSelected(), deltaX, a); 
	}
  if(deltaY!=0)
  {
    ((UndoableData) egc.getDataModel()).beginUpdate();
    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
    {
      aEvent = (TrackEvent) e.nextElement();
      a.setY(aEvent, a.getY(aEvent)+deltaY);
    }    
  }
  egc.getTrack().getFtsTrack().requestNotifyEndUpdate();
}

//---Fields
MaxVector times = new MaxVector();
MaxVector movingEvents = new MaxVector();

DirectionChooser itsDirectionChooser;
SelectionMover itsSelectionMover;
SequenceSelectionResizer itsSelectionResizer;
boolean resizing = false;
}
