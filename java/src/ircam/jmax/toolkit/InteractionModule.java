 //
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

/**
 * The base classe of modules that handles the user interaction.
 * Specialization of these modules are used by the tools, 
 * in order to separate the low-level interaction (example rubber-banding 
 * of a selection rect) 
 * from the semantic (selection of the objects in the rect).
 * This base class acts like a multiple adapter for the derived class.
 */
public class InteractionModule implements MouseListener, MouseMotionListener, KeyListener{


  /**
   * constructor. 
   */
  public InteractionModule() 
  {
  }


  /**
   * called when this modules must take the interaction.
   * don't call this function directly: call Tool.mountIModule instead
   */ 
  protected void takeInteraction(GraphicContext theGc) 
  {

    if (currentActiveModule != null)
      currentActiveModule.unBindFromProducer(); //of the old gc

    gc = theGc;    
    currentActiveModule = this;

    bindToProducer(gc.getGraphicSource()); //in the new gc

  }


  /**
   * make this UI Module listen the event coming from the given event producer.
   */
  protected void bindToProducer(Component eventProducer) 
  {  
    if (eventProducer != null) 
      {
	gc.getGraphicSource().addMouseListener(this);
	gc.getGraphicSource().addMouseMotionListener(this);
	gc.getGraphicSource().addKeyListener(this);
      }
  }


  /**
   * stop listening events from the current producer
   */
  protected void unBindFromProducer() 
  {
    if (gc.getGraphicSource() != null) 
      {   
	gc.getGraphicSource().removeMouseListener(this);
	gc.getGraphicSource().removeMouseMotionListener(this);
	gc.getGraphicSource().removeKeyListener(this);	
      }
  }

  /**
   * set the starting point for this interaction module
   */
  public void interactionBeginAt(int x, int y) {}


  /** temporarly suspend listening the events from the producer
   * (does not change the current interaction module)
   */
  public static void suspend()
  {
    currentActiveModule.unBindFromProducer();
    suspended = true;
  }

  /** is there a suspended module? */
  public static boolean isSuspended()
  {
    return suspended;
  }

  /** resume the activitiy of the current Active module
   */
  public static void resume()
  {
    currentActiveModule.bindToProducer(currentActiveModule.gc.getGraphicSource());
    suspended = false;
  }

  //----------- Mouse interface ------------
  public void mouseClicked(MouseEvent e) {}
  
  public void mouseEntered(MouseEvent e) {} 
  
  public void mouseExited(MouseEvent e) {} 
  
  public void mousePressed(MouseEvent e) {} 
  
  public void mouseReleased(MouseEvent e) {}
  
  //------------- Motion interface ----------
  public void mouseDragged(MouseEvent e) {} 
  
  public void mouseMoved(MouseEvent e) {} 

  //------------- key interface ----------
  public void keyTyped(KeyEvent e) {}

  public void keyPressed(KeyEvent e) {}

  public void keyReleased(KeyEvent e) {}


  //--------------- Fields
  static InteractionModule currentActiveModule = null;
  static boolean suspended = false;

  protected GraphicContext gc;

}


