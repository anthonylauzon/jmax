
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;

/**
 * the base classe of modules that handles the user interaction.
 * Specialization of these modules are used by the tools, 
 * in order to separate the low-level interaction (example rubber-banding 
 * of a selection rect) 
 * from the semantic (selection of the objects in the rect).
 * This base class acts like a multiple adapter for the derived class.
 */
public abstract class InteractionModule implements MouseListener, MouseMotionListener, KeyListener{


  /**
   * constructor. 
   */
  public InteractionModule() 
  {
  }


  /**
   * called when this modules must take the interaction.
   * don't call this function directly: call ScrTool.mountIModule instead
   */ 
  public void takeInteraction(GraphicContext theGc) 
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
  public void bindToProducer(Component eventProducer) 
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
  public void unBindFromProducer() 
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
  abstract public void interactionBeginAt(int x, int y);

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

  GraphicContext gc;

}


