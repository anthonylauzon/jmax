//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.table;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;

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
    itsListener.dragStart(e.getX(), e.getY());
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

    itsListener.dynamicDrag(e.getX(), e.getY());
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
  boolean active = false;
}
