package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.Enumeration;
import com.sun.java.swing.ImageIcon;


/**
 * the tool used to move a selection of events.
 * It uses two UI modules: a MouseTracker to establish the starting point
 * of the moving operation, and a SelectionMover to actually move the
 * objects.
 */
public class MoverTool extends Tool implements PositionListener, DragListener {

  /**
   * constructor.
   */
  public MoverTool(GraphicContext theGc, ImageIcon theIcon, int theDirection) 
  {
    super("mover", theIcon);
    
    gc = theGc;
    itsMouseTracker = new MouseTracker(this);
    itsSelectionMover = new ExplodeSelectionMover(this, theDirection);

    startingPoint = new Point();
  }


  /** the default interaction module */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }

  /**
   * called when this module is "unmounted"
   */
  public void deactivate() 
  {
  }


  /**
   * position listening. It mounts the SelectionMover if the 
   * point choosen is on a selected event. 
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aScrEvent = (ScrEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
    if (aScrEvent != null && ExplodeSelection.getSelection().isInSelection(aScrEvent)) 
      {

	startingPoint.setLocation(x, y);

	mountIModule(itsSelectionMover, x, y);

      }
  }


  /**
   * drag listening, called by the SelectionMover UI Module.
   * Moves the selected objects in the new location,
   * and then mount its default UI Module
   */
  public void dragEnd(int x, int y) 
  {
    ScrEvent aEvent;

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    // starts a serie of undoable transitions
    egc.getDataModel().beginUpdate();

    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();
	
	egc.getAdapter().setX(aEvent, egc.getAdapter().getX(aEvent)+deltaX);
	egc.getAdapter().setY(aEvent, egc.getAdapter().getY(aEvent)+deltaY);
      }

    egc.getDataModel().endUpdate();

    mountIModule(itsMouseTracker);
    gc.getGraphicDestination().repaint();    
  }

  //------------ Fields
  MouseTracker itsMouseTracker;
  SelectionMover itsSelectionMover;

  public static int HORIZONTAL_MOVEMENT = 1;
  public static int VERTICAL_MOVEMENT = 2;

  Point startingPoint;
}



