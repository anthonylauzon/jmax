package ircam.jmax.editors.explode;

import java.awt.event.*;

public class DirectionChooser extends InteractionModule {
 

  public DirectionChooser(DirectionListener theListener)
  {
    super();
    itsListener = theListener;

  } 


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
