package ircam.jmax.toolkit;

import java.awt.*;
import com.sun.java.swing.event.*;

/**
 * The context for a generic editing session.
 * it stores the source of graphic events, the graphic destination for 
 * the drawing operations, the RendererManager, the Frame.
 */
public class GraphicContext {

  public GraphicContext()
  {
  }

  /**
   * sets the source of events, that is: the components that generates
   * the awt events.
   */
  public void setGraphicSource(Component theGraphicSource) 
  {
    itsGraphicSource = theGraphicSource;
  }


  /**
   * gets the source
   */
  public Component getGraphicSource() 
  {
    return itsGraphicSource;
  }


  /**
   * set the destination, that is the component that will receive the
   * graphic (drawing) operations
   */
  public void setGraphicDestination(Component theGraphicDestination) 
  {
    itsGraphicDestination = theGraphicDestination;
  }


  /**
   * get the destination
   */
  public Component getGraphicDestination() 
  {
    return itsGraphicDestination;
  }


  /**
   * sets the RenderManager to be used in this context
   */
  public void setRenderManager(RenderManager theManager) 
  {
    itsRenderManager = theManager;
  }
  
  /**
   * gets the RenderManager
   */
  public RenderManager getRenderManager() 
  {
    return itsRenderManager;
  }

  /**
   * set the status bar (should this become standard?)
   */
  public void setStatusBar(StatusBar theStatusBar) 
  {
    itsStatusBar = theStatusBar;
  }

  /**
   * get the status bar
   */
  public StatusBar getStatusBar() 
  {
    return itsStatusBar;
  }
  
  /** sets the awt Frame this context refers to */
  public void setFrame(Frame f) 
  {
    itsFrame = f;
  }

  /** gets the awt Frame this context refers to */
  public Frame getFrame() 
  {
    if (itsFrame != null) 
      return itsFrame;
    else return getFrame(itsGraphicDestination);
  }


  /**
   * utility static function:
   * returns the Frame containing a component (why this function is not in AWT?)
   */
  public static Frame getFrame(Component c) {
    for (Container p = c.getParent(); p != null; p = p.getParent()) {
      if (p instanceof Frame) {
	return (Frame) p;
      }
    }
    return null;
  }

  //-------------------------------------

  protected Component itsGraphicSource;
  protected Component itsGraphicDestination;

  StatusBar itsStatusBar;

  protected RenderManager itsRenderManager;
  protected Frame itsFrame;
}












