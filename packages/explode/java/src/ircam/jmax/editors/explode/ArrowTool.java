
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
public class ArrowTool extends ScrTool implements PositionListener, SelectionListener{

  /**
   * Constructor. 
   */
  public ArrowTool(GraphicContext theGc) 
  {
    super("arrow", new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/selecter.gif"));
    
    gc = theGc;
    
    itsMouseTracker = new MouseTracker(this, gc);
    itsSelecter = new Selecter(this, gc);
  }

  
  /**
   * called when this tool has been choosen
   */
  public void activate() 
  {
    mountIModule(itsMouseTracker);
  }


  /**
   * called when this tool is unmounted
   */
  public void deactivate(){}

  
  /**
   * called by the UI modules
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aScrEvent = gc.getRenderer().eventContaining(x, y);
    
    if ((modifiers & InputEvent.SHIFT_MASK) == 0)
      gc.getSelection().deselectAll();
    
    itsSelecter.interactionBeginAt(x, y);
    mountIModule(itsSelecter);
    
    if (aScrEvent == null)
      {
	gc.getGraphicDestination().repaint();
      }
    else 
      {
	System.err.println("hit an object");
      }
  }


  /**
   * called by the UI modules
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
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
    selectArea(gc.getRenderer(), gc.getSelection(), x, y,  w,  h);
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

  GraphicContext gc;

}





