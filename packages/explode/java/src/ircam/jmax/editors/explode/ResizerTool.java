package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.Enumeration;
import com.sun.java.swing.ImageIcon;


/**
 * the tool used to resize a selection of events.
 * It uses two UI modules: a MouseTracker to establish the starting point
 * of the moving operation, and a SelectionResizer to actually resize the
 * objects.
 */
public class ResizerTool extends ScrTool implements PositionListener, DragListener {

  /**
   * constructor.
   */
  public ResizerTool(GraphicContext theGc, ImageIcon theIcon) 
  {
    super("Resizer", theIcon);
    
    gc = theGc;
    itsMouseTracker = new MouseTracker(this);
    itsSelectionResizer = new SelectionResizer(this);

    startingPoint = new Point();
  }


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
    ScrEvent aScrEvent = gc.getRenderer().eventContaining(x, y);
    
    if (aScrEvent != null && ExplodeSelection.getSelection().isInSelection(aScrEvent)) 
      {
	startingPoint.setLocation(x, y);

	mountIModule(itsSelectionResizer, x, y);
      }
  }


  /**
   * drag listening, called by the SelectionResizer UI Module.
   * Resizes all the selected objects,
   * and then mount its default UI Module
   */
  public void dragEnd(int x, int y) 
  {
    ScrEvent aEvent;

    int deltaX = x-startingPoint.x;

    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();

	if (gc.getAdapter().getLenght(aEvent)+deltaX > 0)
	  gc.getAdapter().setLenght(aEvent, gc.getAdapter().getLenght(aEvent)+deltaX);
      }

    mountIModule(itsMouseTracker);
    gc.getGraphicDestination().repaint();    
  }

  //------------ Fields
  MouseTracker itsMouseTracker;
  SelectionResizer itsSelectionResizer;

  Point startingPoint;
}



