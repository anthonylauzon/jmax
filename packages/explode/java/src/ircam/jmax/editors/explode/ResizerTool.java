package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;


/**
 * the tool used to resize a selection of events.
 * It uses a SelectionResizer to actually resize the
 * objects.
 */
public class ResizerTool extends SelecterTool implements DragListener {

  /**
   * constructor.
   */
  public ResizerTool(ImageIcon theIcon) 
  {
    super("Resizer", theIcon);
    
    itsSelectionResizer = new ExplodeSelectionResizer(this);

  }


  /**
   * called when this module is "unmounted"
   */
  public void deactivate() 
  {
  }

  /**
   * a single object has been selected, in coordinates x, y:
   * Mount the resizer interaction module.
   * Overrides the abstract SelecterTool.singleObjectSelected */
  public void singleObjectSelected(int x, int y, int modifiers) 
  {
    mountIModule(itsSelectionResizer, x, y);
  }

  /** 
   * a group of objects was selected 
   *overrides the abstract SelecterTool.multipleObjectSelected */
  void multipleObjectSelected()
  {
  }

  /**
   * drag listening, called by the SelectionResizer UI Module.
   * Resizes all the selected objects,
   * and then mount its default UI Module
   */
  public void dragEnd(int x, int y) 
  {
    ScrEvent aEvent;

    int deltaX = x-startingPoint.x;
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    // starts a serie of undoable transitions
    ((UndoableData) egc.getDataModel()).beginUpdate();
    
    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();

	if (egc.getAdapter().getLenght(aEvent)+deltaX > 0)
	  egc.getAdapter().setLenght(aEvent, egc.getAdapter().getLenght(aEvent)+deltaX);
      }

    ((UndoableData) egc.getDataModel()).endUpdate();
    
    mountIModule(itsSelecter);

  }


  //------------ Fields
  SelectionResizer itsSelectionResizer;

}



