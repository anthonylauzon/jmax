package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;
import java.util.*;
import java.awt.Component;

import com.sun.java.swing.ImageIcon;
import com.sun.java.swing.undo.*;

/**
 * the tool used to perform an erase operation.
 */
public class DeleteTool extends Tool implements PositionListener {
  
  /**
   * Constructor. 
   */
  public DeleteTool(GraphicContext theGc, ImageIcon theImageIcon) 
  {
    super("eraser", theImageIcon);
    
    gc = theGc;    
    itsMouseTracker = new VerboseMouseTracker(this);
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
    ScrEvent aEvent = (ScrEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
    if (aEvent != null) 
      {
	ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;
	// starts an undoable transition
	egc.getDataModel().beginUpdate();

	if (ExplodeSelection.getSelection().isInSelection(aEvent))
	  {
	    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
	      {
		aEvent = (ScrEvent) e.nextElement();
		egc.getDataModel().removeEvent(aEvent);
	      }
	  }
	else
	  egc.getDataModel().removeEvent(aEvent);

	egc.getDataModel().endUpdate();
      }
  } 
  


  //---- Fields

  MouseTracker itsMouseTracker;

}
