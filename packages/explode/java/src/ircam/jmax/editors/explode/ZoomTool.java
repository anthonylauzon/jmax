package ircam.jmax.editors.explode;

import java.awt.event.*;
import java.awt.*;
import com.sun.java.swing.ImageIcon;

/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ZoomTool extends ScrTool implements PositionListener {

  /** constructor */
  public ZoomTool(GraphicContext theGc, ImageIcon theImageIcon) 
  {
    super("zoomer", theImageIcon);

    gc = theGc;
    itsMouseTracker = new MouseTracker(this);
  }


  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }


  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }

  
  /**
   * PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
 
    if ((modifiers & InputEvent.ALT_MASK) == 0) //zoom out
      {
	gc.getAdapter().setXZoom((int) (gc.getAdapter().getXZoom() * 100 * 2));

      }
    else 
      {                                       //zoom in
	if (gc.getAdapter().getXZoom() <= 0.01) return;
	gc.getAdapter().setXZoom((int) (gc.getAdapter().getXZoom() * 100 / 2));

      }

    gc.getGraphicDestination().repaint();
  }
  

  //-------------- Fields

  MouseTracker itsMouseTracker;
}



