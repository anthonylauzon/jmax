package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;


/**
 * a VERY simple interaction module:
 * it just waits the choice of a point in a graphicContext via mouse-clicks.
 * It communicates the choice to a "Position listener"
 */
public class MouseTracker extends InteractionModule {

  /**
   * constructor.
   */
  public MouseTracker(PositionListener theListener, GraphicContext gc) 
  {
    super(gc.getGraphicEventSource(), gc.getGraphicDestination());
    itsListener = theListener;
  }


  //Mouse interface
  
  public void mousePressed(MouseEvent e) 
  {  
    itsListener.positionChoosen(e.getX(), e.getY(), e.getModifiers());
  } 

  //------------- fields
  PositionListener itsListener;
}


