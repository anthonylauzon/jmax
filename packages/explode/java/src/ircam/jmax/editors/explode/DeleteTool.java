//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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
  public DeleteTool(ImageIcon theImageIcon) 
  {
    super("eraser", theImageIcon);
    
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
	((UndoableData) egc.getDataModel()).beginUpdate();

	if (egc.getSelection().isInSelection(aEvent))
	  {
	    Enumeration e;
	    MaxVector v = new MaxVector();

	    // copy the selected elements in another MaxVector (cannot remove
	    // elements of a Vector inside a loop based on an enumeration of this vector, it simply does'nt work...)
	    for ( e = egc.getSelection().getSelected();
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
	    egc.getSelection().deselectAll();
	  }

	((UndoableData) egc.getDataModel()).endUpdate();
      }
  } 
  


  //---- Fields

  MouseTracker itsMouseTracker;

}
