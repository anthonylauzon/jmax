package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.Enumeration;
import com.sun.java.swing.ImageIcon;


/**
 * the tool used to move a selection of events.
 * It uses two UI modules: a MouseTracker to establish the starting point
 * of the moving operation, and a SelectionMover to actually move the
 * objects.
 */
public class MoverTool extends ScrTool implements PositionListener, DragListener {

  /**
   * constructor.
   */
  public MoverTool(GraphicContext theGc, ImageIcon theIcon, int theDirection) 
  {
    super("mover", theIcon);
    
    gc = theGc;
    itsMouseTracker = new MouseTracker(this, gc);
    itsSelectionMover = new SelectionMover(this, gc, theDirection);

    startingPoint = new Point();
  }


  /**
   * called when this tool becomes the active tool.
   * It mounts its default UI Module, the MouseTracker
   */
  public void activate() 
  {
    mountIModule(itsMouseTracker);
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
    ScrEvent aScrEvent = gc.getRenderer().eventContaining(x, y);
    
    if (aScrEvent != null && gc.getSelection().isInSelection(aScrEvent)) 
      {
	startingPoint.setLocation(x, y);

	itsSelectionMover.interactionBeginAt(x, y);
	mountIModule(itsSelectionMover);
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

    for (Enumeration e = gc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();
	
	gc.getAdapter().setX(aEvent, gc.getAdapter().getX(aEvent)+deltaX);
	gc.getAdapter().setY(aEvent, gc.getAdapter().getY(aEvent)+deltaY);
      }

    mountIModule(itsMouseTracker);
    gc.getGraphicDestination().repaint();    
  }

  //------------ Fields
  MouseTracker itsMouseTracker;
  SelectionMover itsSelectionMover;
  GraphicContext gc;

  public static int VERTICAL_MOVEMENT = 1;
  public static int HORIZONTAL_MOVEMENT = 2;

  Point startingPoint;
}



