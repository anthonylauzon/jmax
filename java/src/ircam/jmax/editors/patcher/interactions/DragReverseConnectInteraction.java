package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an inlet to an outlet */

class DragReverseConnectInteraction extends Interaction
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
    if (Squeack.isDown(squeack) && Squeack.onInlet(squeack))
      {
	InletSensibilityArea area = (InletSensibilityArea) dobject;

	dst   = area.getObject();
	inlet = area.getNumber();

	dragStart.x = dst.getInletAnchorX(inlet);
	dragStart.y = dst.getInletAnchorY(inlet);
	editor.resetHighlightedInlet();
	dragged = false;
      }
    else if (Squeack.isUp(squeack))
      {
	if (dragged )
	  {
	    // Dragged: 
	    //  do the connection if we have a destination

	    //  do the connection if we have a destination

	    if (destinationChoosen)
	      {
		editor.resetHighlightedOutlet(); 

		doConnection(editor, src, outlet, dst, inlet);
	      }

	    // clean up

	    editor.getDisplayList().noDrag();
	    editor.getDisplayList().redrawDragLine();
	    editor.setCursor(Cursor.getDefaultCursor());
	    destinationChoosen = false;

	    editor.endInteraction();
	  }
	else
	  {
	    // Not dragged, 
	    // If there is an highlighted inlet, do the connection.
	    // Otherwise highlight this outlet.

	    if (editor.hasHighlightedOutlet())
	      {
		doConnection(editor,
			     editor.getHighlightedOutletObject(), editor.getHighlightedOutlet(),
			     dst, inlet);

		editor.endInteraction();
	      }
	    else
	      {
		editor.unlockHighlightedInlet();
		editor.setHighlightedInlet(dst, inlet);
		editor.lockHighlightedInlet();

		editor.endInteraction();
	      }
	  }
      }
    else if (Squeack.isDrag(squeack) && Squeack.onOutlet(squeack))
      {
	dragged = true;

	if (! destinationChoosen)
	  {
	    OutletSensibilityArea area = (OutletSensibilityArea) dobject;

	    src    = area.getObject();
	    outlet = area.getNumber();
	    editor.setHighlightedOutlet(src, outlet);
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
	    editor.resetHighlightedOutlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



