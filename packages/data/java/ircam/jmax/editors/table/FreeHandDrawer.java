//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
    itsListener.dragStart(e.getX(), e.getY(), e);
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

    //getGc().getCoordWriter().postCoordinates(e.getX(), e.getY());
 
  }

  /**
   * Overrides InteractionModule mouseDragged() to draw the segment between
   * two successives mouseDragged  and communicate to the Listener
   */
  public void mouseDragged(MouseEvent e) 
  {
    if (! active) return;

    itsListener.dynamicDrag(e.getX(), e.getY(), e);
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
