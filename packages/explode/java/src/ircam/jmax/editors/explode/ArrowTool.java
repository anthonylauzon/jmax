
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import com.sun.java.swing.ImageIcon;

/**
 * the tool used to perform the different operations associated
 * with the "arrow" tool, that is:
 * selection, area_selection, deselection
 * The default (initial) interface module for this tool is the MouseTracker.
 */ 
public class ArrowTool extends ScrTool implements PositionListener, DirectionListener, DragListener, GraphicSelectionListener{

  /**
   * Constructor. 
   */
  public ArrowTool(GraphicContext theGc) 
  {
    super("arrow", new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/selecter.gif"));
    
    gc = theGc;
    
    itsMouseTracker = new MouseTracker(this);
    itsSelecter = new Selecter(this);
    itsDirectionChooser = new DirectionChooser(this);
    itsSelectionMover = new SelectionMover(this, MoverTool.HORIZONTAL_MOVEMENT);
  }

  
  /**
   * the default InteractionModule
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }


  /**
   * called when this tool is unmounted
   */
  public void deactivate(){}

  
  /**
   * called by the MouseTracker UI module
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aScrEvent = gc.getRenderer().eventContaining(x, y);
    
    if (aScrEvent != null)
      if (ExplodeSelection.getSelection().isInSelection(aScrEvent)) 
	{ //hey! wanna move?
	  startingPoint.setLocation(x,y);
	  mountIModule(itsDirectionChooser, x, y);
	} 
      else
	{
	  selectionChoosen(x, y, 1, 1);
	}
    else {
      if ((modifiers & InputEvent.SHIFT_MASK) == 0)
	{
	  ExplodeSelection.getSelection().deselectAll(); 
	  gc.getGraphicDestination().repaint();
	}
      
      mountIModule(itsSelecter, x, y);
    }
  }

  /**
   * called by the DirectionChooser UI module
   */
  public void directionChoosen(int theDirection) 
  {
    itsSelectionMover.setDirection(theDirection);
    mountIModule(itsSelectionMover, startingPoint.x, startingPoint.y);

  }
  

  /**
   * called by the selecter UI module
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    if (w ==0) w=1;// at least 1 pixel wide
    if (h==0) h=1;
    
    selectArea(x, y, w, h);
    
    gc.getGraphicDestination().repaint();
    
    mountIModule(itsMouseTracker);
  }

  /**
   * called by the SelectionMover UI module
   */
  public void dragEnd(int x, int y)
  {
    ScrEvent aEvent;

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();
	
	gc.getAdapter().setX(aEvent, gc.getAdapter().getX(aEvent)+deltaX);
	gc.getAdapter().setY(aEvent, gc.getAdapter().getY(aEvent)+deltaY);
      }

    mountIModule(itsMouseTracker);
    gc.getGraphicDestination().repaint();    
  }


  /**
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
    selectArea(gc.getRenderer(), ExplodeSelection.getSelection(), x, y,  w,  h);
  }

  
  /**
   * Selects all the objects in a given rectangle, given a Render
   * and a Selection
   */
  public static void selectArea(Renderer r, SelectionHandler s, int x, int y, int w, int h) 
  {
    ScrEvent aScrEvent;

    for (Enumeration e = r.eventsIntersecting(x, y, w, h); e.hasMoreElements();) 
      {
	aScrEvent = (ScrEvent)e.nextElement() ;
	s.select(aScrEvent);
      }
  }



  //---Fields

  MouseTracker itsMouseTracker;
  Selecter itsSelecter;
  DirectionChooser itsDirectionChooser;
  SelectionMover itsSelectionMover;

  Point startingPoint = new Point();
}





