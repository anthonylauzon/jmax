package ircam.jmax.editors.explode;

import java.awt.event.*;
import java.awt.*;
import com.sun.java.swing.ImageIcon;

/**
 * The tool used to add an event in the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ScrAddingTool extends ScrTool implements PositionListener {

  /**
   * Constructor. It needs to know the graphic source of events,
   * the Adapter, the model. All these informations are in the
   * given graphic context.
   */
  public ScrAddingTool(GraphicContext theGc) 
  {
    super("adder", new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/adder.gif"));

    gc = theGc;
    itsMouseTracker = new MouseTracker(this, gc);
  }


  /**
   * called when this tool becomes the active tool
   */
  public void activate() 
  {
    mountIModule(itsMouseTracker);
  }


  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }

  
  /**
   *PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aEvent = new ScrEvent();

    gc.getAdapter().setX(aEvent, x);
    gc.getAdapter().setY(aEvent, y);

    gc.getDataModel().addEvent(aEvent);
  } 
  

  //-------------- Fields
  GraphicContext gc;
  MouseTracker itsMouseTracker;
}



