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
  GraphicObject src;
  int outlet;
  GraphicObject dst;
  int inlet;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingInOutletLocations(true);
    filter.setAutoScrolling(true);
  }

  // Utility method

  private void doConnection(ErmesSketchPad editor, GraphicObject src, int outlet, GraphicObject dst, int inlet)
  {
    if (src == dst)
      {
	editor.showMessage("Cannot connect an object to itself !");
	return;
      }

    try
      {
	FtsConnection fc;
	GraphicConnection connection;

	fc = Fts.makeFtsConnection(src.getFtsObject(),
				   outlet, 
				   dst.getFtsObject(),
				   inlet);
		    
	connection = new GraphicConnection(editor,
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
    if (Squeack.isDown(squeack) && Squeack.onOutlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	src    = (GraphicObject) area.getTarget();
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
	    // Not dragged, start a move connection

	    editor.getEngine().setInteraction(Interactions.moveConnectInteraction);
	    editor.getEngine().getCurrentInteraction().gotSqueack(editor, squeack, area, mouse, oldMouse);
	  }
      }
    else if (Squeack.isDrag(squeack) && Squeack.onInlet(squeack))
      {
	dragged = true;

	if ((! destinationChoosen) || dst != (GraphicObject) area.getTarget() || inlet != area.getNumber())
	  {
	    dst   = (GraphicObject) area.getTarget();
	    inlet = area.getNumber();

	    editor.setHighlightedInlet(dst, inlet);
	    destinationChoosen = true;
	  }


	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y,
					    dst.getInletAnchorX(inlet), dst.getInletAnchorY(inlet));
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



