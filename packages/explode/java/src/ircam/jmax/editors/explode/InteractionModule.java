
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;

/**
 * the base classe of modules that handles the user interaction.
 * Specialization of these modules are used by the tools, 
 * in order to separate the low-level interaction (example rubber-banding 
 * of a selection rect) 
 * from the semantic (example selection of the objects in the rect).
 * This base class acts like a multiple adapter for the derived class.
 */
public class InteractionModule implements MouseListener, MouseMotionListener, KeyListener{


  /**
   * constructor. It needs to know the source of events, and
   * the destination of the drawing operations
   */
  public InteractionModule(Component theSource, Component theDestination) 
  {
    itsEventSource = theSource;
    itsGraphicDestination = theDestination;
  }

  /**
   * called when this modules must take the interaction.
   * don't call this function directly: call ScrTool.mountIModule instead
   */ 
  public void takeInteraction() 
  {
    
    if (currentActiveModule != null)
      currentActiveModule.unBindFromProducer();
    
    currentActiveModule = this;
    bindToProducer(itsEventSource);
  }


  /**
   * make this UI Module listen the event coming from the given event producer.
   */
  public void bindToProducer(Component eventProducer) 
  {  
    if (eventProducer != null) 
      {
	itsEventSource = eventProducer;

	itsEventSource.addMouseListener(this);
	itsEventSource.addMouseMotionListener(this);
	itsEventSource.addKeyListener(this);
      }
  }


  /**
   * stop listening events from the current producer
   */
  public void unBindFromProducer() 
  {
    if (itsEventSource != null) 
      {   
	itsEventSource.removeMouseListener(this);
	itsEventSource.removeMouseMotionListener(this);
	itsEventSource.removeKeyListener(this);	
      }
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

  Component itsEventSource;
  Component itsGraphicDestination;

}


