package ircam.jmax.editors.explode;

import java.awt.Component;
import com.sun.java.swing.ImageIcon;


/**
 * the tool used to perform an erase operation.
 */
public class DeleteTool extends ScrTool implements PositionListener {
  
  /**
   * Constructor. 
   */
  public DeleteTool(GraphicContext theGc, ImageIcon theImageIcon) 
  {
    super("eraser", theImageIcon);
    
    gc = theGc;    
    itsMouseTracker = new MouseTracker(this);
  }

  
  /**
   * the default Interaction Module
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }
  

  /**
   * called when this tool is unmounted
   */
  public void deactivate() {}

  
  /**
   * called by the MouseTracker
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aEvent = gc.getRenderer().eventContaining(x, y);
    
    if (aEvent != null)
      gc.getDataModel().removeEvent(aEvent);
  } 
  
  //---- Fields

  MouseTracker itsMouseTracker;

}
