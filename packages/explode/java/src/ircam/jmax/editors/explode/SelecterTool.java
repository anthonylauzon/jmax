
package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The base class of all the tools that use a selection
 * to establish the "target" of their action and that have the
 * Selecter as the basic (default) interaction module.
 * It takes care of handling the selection process.
 * Examples of this kind of tools are: 
 * the Arrow tool, the resizer tool, the mover tool.
 * @see ArrowTool
 * @see ResizerTool
 */
public abstract class SelecterTool extends Tool implements GraphicSelectionListener{
  
  public SelecterTool(String theName, ImageIcon theImageIcon)
  {
    super(theName, theImageIcon);

    itsSelecter = new Selecter(this);
  }

  /**
   * the default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsSelecter;
  }


  /**
   * called by the Selecter UI module at mouse down
   */
  public void selectionPointChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aScrEvent = (ScrEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
    if (aScrEvent != null) 
      { //click on event

	startingPoint.setLocation(x,y);

	if (!ExplodeSelection.getSelection().isInSelection(aScrEvent)) 
	  {
	    if ((modifiers & InputEvent.SHIFT_MASK) == 0) //without shift
	      {
		ExplodeSelection.getSelection().deselectAll();
	      } 
	    ExplodeSelection.getSelection().select(aScrEvent);
	  }

	singleObjectSelected(x, y, modifiers);
      }
    else //click on empty
      {
      if ((modifiers & InputEvent.SHIFT_MASK) == 0)
	{
	  if (ExplodeSelection.getSelection().size() != 0)
	    {
	      ExplodeSelection.getSelection().deselectAll(); 
	    }
	}
      
    }
  }

  /**
   * called by the selecter UI module
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    if (w ==0) w=1;// at least 1 pixel wide
    if (h==0) h=1;
    
    selectArea(x, y, w, h);
    
    
    multipleObjectSelected();
  }

  /**
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
    selectArea(gc.getRenderManager(), ExplodeSelection.getSelection(), x, y,  w,  h);
  }

  
  /**
   * Graphically selects all the objects in a given rectangle, given a Render
   * and a Selection
   */
  public static void selectArea(RenderManager r, SelectionHandler s, int x, int y, int w, int h) 
  {
    ScrEvent aScrEvent;

    s.select(r.objectsIntersecting(x, y, w, h));
  }

  /**
   * a single object has been selected, in coordinates x, y */
  abstract void singleObjectSelected(int x, int y, int modifiers);

  /** 
   * a group of objects was selected */
  abstract void multipleObjectSelected();

  //--- Fields
  Selecter itsSelecter;

  Point startingPoint = new Point();

}

