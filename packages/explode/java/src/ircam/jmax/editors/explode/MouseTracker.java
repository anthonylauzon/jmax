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
  public MouseTracker(PositionListener theListener) 
  {
    super();
    itsListener = theListener;
  }


  //Mouse interface
  
  public void mousePressed(MouseEvent e) 
  {  
    itsListener.positionChoosen(e.getX(), e.getY(), e.getModifiers());
  } 

  //Mouse interface
  
  public void mouseMoved(MouseEvent e) 
  {  

    gc.getStatusBar().post(ScrToolbar.getTool(), ""+
			      (gc.getAdapter().getInvX(e.getX()))+
			      "       "+
			      (gc.getAdapter().getInvY(e.getY())));
  } 

  public void interactionBeginAt(int x, int y)
  {
    //useless for this kind of interaction module
  }

  //------------- fields
  PositionListener itsListener;

}









