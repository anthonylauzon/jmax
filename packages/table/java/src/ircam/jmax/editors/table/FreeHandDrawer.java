package ircam.jmax.editors.table;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import com.sun.java.swing.*;

import ircam.jmax.toolkit.*;

/**
 * The interaction module used by the PencilTool in the Table package.
 * It communicates with its client via the DynamicDragListener interface.
 * @see PencilTool */
public class FreeHandDrawer extends TableInteractionModule {

  /**
   * Constructor. */
  public FreeHandDrawer(DynamicDragListener theListener) 
  {
    super();
    
    itsListener = theListener;
  }
  

  /**
   * Overrides InteractionModule.mousePressed().
   * Starts the interaction and communicates to the listener.
   */
  public void mousePressed(MouseEvent e) 
  {
    previousX = e.getX();
    previousY = e.getY();
    itsListener.dragStart(previousX, previousY);
    active = true;
  }


  /** Overrides InteractionModule mouseMoved() to write the position
   * of the pencil in the statusbar.
   * @see InteractionModule
   * @see CoordinateWriter
   * @see TableGraphicContext
   */
  public void mouseMoved(MouseEvent e)
  {

    getGc().getCoordWriter().postCoordinates(e.getX(), e.getY());
 
  }

  /**
   * Overrides InteractionModule mouseDragged() to draw the segment between
   * two successives mouseDragged  and communicate to the Listener
   */
  public void mouseDragged(MouseEvent e) 
  {
    if (! active) return;
    int x = e.getX();
    int y = e.getY();

    itsListener.dynamicDrag(x, y);
    LinerTool.drawLine(previousX, previousY, x, y, getGc());
    previousX = x;
    previousY = y;
    
  } 

  /**
   * Overrides InteractionModule mouseReleased() to end the
   * interaction and to communicate with the listener.
   */
  public void mouseReleased(MouseEvent e) 
  {
    if (! active) return;
    itsListener.dragEnd(e.getX(), e.getY());
    active = false;
  }

  //---- Fields
  DynamicDragListener itsListener;
  int previousX;
  int previousY;
  boolean active = false;
}
