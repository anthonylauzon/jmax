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
    filter.setFollowingInOutletLocations(true);
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

	editor.showMessage("Cannot connect");
      }
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onInlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	dst   = (ErmesObject) area.getTarget();
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
	    // Not dragged, start a moveReverseConnection interaction

	    editor.getEngine().setInteraction(Interactions.moveReverseConnectInteraction);
	    editor.getEngine().getCurrentInteraction().gotSqueack(editor, squeack, area, mouse, oldMouse);
	  }
      }
    else if (Squeack.isDrag(squeack) && Squeack.onOutlet(squeack))
      {
	dragged = true;

	if ((! destinationChoosen) || src != (ErmesObject) area.getTarget() || outlet != area.getNumber())
	  {
	    src    = (ErmesObject) area.getTarget();
	    outlet = area.getNumber();
	    editor.setHighlightedOutlet(src, outlet);
	    destinationChoosen = true;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, 
					    src.getOutletAnchorX(inlet), src.getOutletAnchorY(inlet));
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



