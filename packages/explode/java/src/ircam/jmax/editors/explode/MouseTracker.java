package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;


/**
 * a VERY simple interaction module:
 * just waits the choice of a point in a component via mouse-click.
 * It communicates the choice to a "Position listener"
 */
public class MouseTracker extends InteractionModule {
  PositionListener itsListener;

  public MouseTracker(PositionListener theListener, Component theGraphicObject) {
    super(theGraphicObject, theGraphicObject);
    itsListener = theListener;
  }


  //----------- Mouse interface ------------
  
  public void mousePressed(MouseEvent e) {
    
    itsListener.positionChoosen(e.getX(), e.getY());

  } 
}


