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


class DragConnectInteraction extends SubInteraction
{
  Point dragStart = new Point();
  ErmesObject srcObject;
  int outlet;
  ErmesObject dstObject;
  int inlet;

  DragConnectInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingLocations(true);
    filter.setAutoScrolling(true);
  }


  boolean destinationChoosen = false;

  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
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
	    
		connection = new ErmesConnection(engine.getSketch(),
						 srcObject,
						 outlet,
						 dstObject,
						 inlet,
						 fc);
		
		engine.getDisplayList().addConnection(connection);
		connection.redraw();
	      }
	    catch (FtsException e)
	      {
		// Just don't do the connection in case of troubles.
	      }
	  }

	// clean up

	engine.getDisplayList().noDrag();
	engine.getDisplayList().redrawDragLine();
	engine.getSketch().setCursor(Cursor.getDefaultCursor());
	destinationChoosen = false;

	end();
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
	engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
	engine.getDisplayList().dragLine();
	engine.getDisplayList().redrawDragLine();
	engine.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	engine.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isDrag(squeack))
      {
	destinationChoosen = false;
	engine.getSketch().setCursor(Cursor.getDefaultCursor());
	engine.getDisplayList().dragLine();
	engine.getDisplayList().redrawDragLine();
	engine.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	engine.getDisplayList().redrawDragLine();
      }
  }
}



