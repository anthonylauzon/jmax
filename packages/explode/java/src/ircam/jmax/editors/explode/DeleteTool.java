package ircam.jmax.editors.explode;

import java.awt.Component;
import com.sun.java.swing.ImageIcon;


/**
 * the tool used to perform an erase operation.
 * The default (initial) UI Module is a MouseTracker
 */
public class DeleteTool extends ScrTool implements PositionListener {
  
  /**
   * Constructor. 
   */
  public DeleteTool(GraphicContext theGc) 
  {
    super("eraser", new ImageIcon("/u/worksta/maggi/projects/max/packages/explode/images/eraser.gif"));
    
    gc = theGc;    
    itsMouseTracker = new MouseTracker(this, gc);
  }

  
  /**
   * called when this tool becomes the active tool
   */
  public void activate() 
  {
    mountIModule(itsMouseTracker);
  }
  

  /**
   * called when this tool is unmounted
   */
  public void deactivate() {}

  
  /**
   * called by the UI modules
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aEvent = gc.getRenderer().eventContaining(x, y);
    
    if (aEvent != null)
      gc.getDataModel().removeEvent(aEvent);
  } 
  
  //---- Fields

  MouseTracker itsMouseTracker;
  GraphicContext gc;

}
