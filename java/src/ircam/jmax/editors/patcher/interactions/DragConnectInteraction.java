package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an outlet to an inlet */


class DragConnectInteraction extends Interaction
{
  boolean dragged = false;
  Point dragStart = new Point();
  ErmesObject src;
  int outlet;
  ErmesObject dst;
  int inlet;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingLocations(true);
    filter.setAutoScrolling(true);
  }

  // Utility method

  private void doConnection(ErmesSketchPad editor, ErmesObject src, int outlet, ErmesObject dst, int inlet)
  {
    try
      {
	FtsConnection fc;
	ErmesConnection connection;

	fc = Fts.makeFtsConnection(src.getFtsObject(),
				   outlet, 
				   dst.getFtsObject(),
				   inlet);
		    
	connection = new ErmesConnection(editor,
					 src,
					 outlet,
					 dst,
					 inlet,
					 fc);
		
	editor.getDisplayList().add(connection);
	editor.getDisplayList().sortDisplayList();
	connection.updateDimensions();
	connection.redraw();
      }
    catch (FtsException e)
      {
	// Just don't do the connection in case of troubles.
      }
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onOutlet(squeack))
      {
	OutletSensibilityArea area = (OutletSensibilityArea) dobject;

	src    = area.getObject();
	outlet = area.getNumber();

	dragStart.x = src.getOutletAnchorX(outlet);
	dragStart.y = src.getOutletAnchorY(outlet);
	editor.resetHighlightedOutlet();
	dragged = false;
      }
    else if (Squeack.isUp(squeack))
      {
	if (dragged )
	  {
	    // Dragged: 
	    //  do the connection if we have a destination

	    if (destinationChoosen)
	      {
		editor.resetHighlightedInlet(); 

		doConnection(editor, src, outlet, dst, inlet);
	      }

	    // clean up

	    editor.getDisplayList().noDrag();
	    editor.getDisplayList().redrawDragLine();
	    destinationChoosen = false;

	    editor.endInteraction();
	  }
	else
	  {
	    // Not dragged, 
	    // If there is an highlighted inlet, do the connection.
	    // Otherwise highlight this outlet.

	    if (editor.hasHighlightedInlet())
	      {
		doConnection(editor, src, outlet,
			     editor.getHighlightedInletObject(), editor.getHighlightedInlet());
		editor.endInteraction();
	      }
	    else
	      {
		editor.unlockHighlightedOutlet();
		editor.setHighlightedOutlet(src, outlet);
		editor.lockHighlightedOutlet();
		editor.endInteraction();
	      }
	  }
      }
    else if (Squeack.isDrag(squeack) && Squeack.onInlet(squeack))
      {
	dragged = true;

	if (! destinationChoosen)
	  {
	    InletSensibilityArea area = (InletSensibilityArea) dobject;

	    dst   = area.getObject();
	    inlet = area.getNumber();

	    editor.setHighlightedInlet(dst, inlet);
	    destinationChoosen = true;
	  }


	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isDrag(squeack))
      {
	dragged = true;

	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



