package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;

import java.util.*;
import java.awt.Component;

import javax.swing.ImageIcon;
import javax.swing.undo.*;

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
	    Enumeration e;
	    MaxVector v = new MaxVector();

	    // copy the selected elements in another MaxVector
	    for ( e = ExplodeSelection.getSelection().getSelected();
		  e.hasMoreElements();)
	      {
		v.addElement(e.nextElement());
	      }
	    
	    // remove them
	    for (int i = 0; i< v.size(); i++)
	      egc.getDataModel().removeEvent((ScrEvent)(v.elementAt(i)));

	    v = null;
	  }
	else
	  {
	    egc.getDataModel().removeEvent(aEvent);
	    ExplodeSelection.getSelection().deselectAll();
	  }

	egc.getDataModel().endUpdate();
      }
  } 
  


  //---- Fields

  MouseTracker itsMouseTracker;

}
