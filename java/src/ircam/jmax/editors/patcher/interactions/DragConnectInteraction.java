package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling drag-select; 
  started by a click on the background;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class DragConnectInteraction extends Interaction
{
  Point dragStart = new Point();
  ErmesObject srcObject;
  int outlet;
  ErmesObject dstObject;
  int inlet;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingLocations(true);
    filter.setAutoScrolling(true);
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.DOWN | Squeack.OUTLET))
      {
	OutletSensibilityArea area = (OutletSensibilityArea) dobject;

	srcObject   = area.getObject();
	outlet      = area.getNumber();
	dragStart.x = srcObject.getConnectionStartX(outlet);
	dragStart.y = srcObject.getConnectionStartY(outlet);
      }
    else if (Squeack.isUp(squeack))
      {
	//  do the connection if we have a destination

	if (destinationChoosen)
	  {
	    try
	      {
		FtsConnection fc;
		ErmesConnection connection;

		fc = Fts.makeFtsConnection(srcObject.getFtsObject(),
					   outlet, 
					   dstObject.getFtsObject(),
					   inlet);
	    
		connection = new ErmesConnection(editor,
						 srcObject,
						 outlet,
						 dstObject,
						 inlet,
						 fc);
		
		editor.getDisplayList().add(connection);
		connection.updateDimensions();
		connection.redraw();
	      }
	    catch (FtsException e)
	      {
		// Just don't do the connection in case of troubles.
	      }
	  }

	// clean up

	editor.getDisplayList().noDrag();
	editor.getDisplayList().redrawDragLine();
	editor.setCursor(Cursor.getDefaultCursor());
	destinationChoosen = false;

	editor.endInteraction();
      }
    else if (squeack == (Squeack.DRAG | Squeack.INLET))
      {
	if (! destinationChoosen)
	  {
	    InletSensibilityArea area = (InletSensibilityArea) dobject;

	    dstObject = area.getObject();
	    inlet    = area.getNumber();
	  }

	destinationChoosen = true;
	editor.setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isDrag(squeack))
      {
	destinationChoosen = false;
	editor.setCursor(Cursor.getDefaultCursor());
	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



