package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an inlet to an outlet */

class MoveReverseConnectInteraction extends Interaction
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
      }
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isUp(squeack) && Squeack.onInlet(squeack))
      {
	dst   = (ErmesObject) area.getTarget();
	inlet = area.getNumber();

	moveStart.x = dst.getInletAnchorX(inlet);
	moveStart.y = dst.getInletAnchorY(inlet);
	editor.resetHighlightedInlet();
      }
    else if (Squeack.isDown(squeack) && Squeack.isShift(squeack))
      {
	//  do the connection if we have a destination

	if (destinationChoosen)
	  {
	    editor.resetHighlightedOutlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }
      }
    else if (Squeack.isDown(squeack))
      {
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
    else if (Squeack.isMove(squeack) && Squeack.onOutlet(squeack))
      {
	if ((! destinationChoosen) || src != (ErmesObject) area.getTarget() || outlet != area.getNumber())
	  {
	    src    = (ErmesObject) area.getTarget();
	    outlet = area.getNumber();
	    editor.setHighlightedOutlet(src, outlet);
	    destinationChoosen = true;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, 
					    src.getOutletAnchorX(inlet), src.getOutletAnchorY(inlet));
	editor.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isMove(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedOutlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



