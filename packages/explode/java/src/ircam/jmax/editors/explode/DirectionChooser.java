package ircam.jmax.editors.explode;

import java.awt.event.*;

/**
 * A simple interaction module to detect the direction of movement
 * of the mouse, and to  communicate it to a DirectionListener.
 * This class does not handle mouseDown, so it must be mounted
 * when a mouseDown already happened.
 * Warning: this module continuosly send direction informations, so
 * do not left it active more then needed.
 */
public class DirectionChooser extends InteractionModule {
 

  /**
   * constructor with the direction listener
   */
  public DirectionChooser(DirectionListener theListener)
  {
    super();
    itsListener = theListener;
  } 


  /**
   * sets the point to be considered as origin of the movement
   */
  public void interactionBeginAt(int x, int y)
  {
    itsX = x;
    itsY = y;
  }

  //Mouse interface
  
  public void mouseMoved(MouseEvent e) 
  {  
    if (Math.abs(e.getX()-itsX) > Math.abs(e.getY()-itsY))    
	itsListener.directionChoosen(MoverTool.HORIZONTAL_MOVEMENT);
	else itsListener.directionChoosen(MoverTool.VERTICAL_MOVEMENT);
  } 

  //--- Fields
  DirectionListener itsListener;
  int itsX;
  int itsY;
}
