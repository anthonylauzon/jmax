package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an outlet to an inlet */


class MoveConnectInteraction extends Interaction
{
  Point moveStart = new Point();
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

	editor.showMessage("Cannot connect");
      }
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isUp(squeack) && Squeack.onOutlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	src    = (ErmesObject) area.getTarget();
	outlet = area.getNumber();

	moveStart.x = src.getOutletAnchorX(outlet);
	moveStart.y = src.getOutletAnchorY(outlet);
	editor.resetHighlightedOutlet();
      }
    else if (Squeack.isDown(squeack) && Squeack.isShift(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }
      }
    else if (Squeack.isDown(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }

	// clean up

	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().noDrag();
	destinationChoosen = false;

	editor.endInteraction();
      }
    else if (Squeack.isMove(squeack) && Squeack.onInlet(squeack))
      {
	if ((! destinationChoosen) || dst != (ErmesObject) area.getTarget() || inlet != area.getNumber())
	  {
	    dst   = (ErmesObject) area.getTarget();
	    inlet = area.getNumber();

	    editor.setHighlightedInlet(dst, inlet);
	    destinationChoosen = true;
	  }


	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y,
					    dst.getInletAnchorX(inlet), dst.getInletAnchorY(inlet));
	editor.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isMove(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



