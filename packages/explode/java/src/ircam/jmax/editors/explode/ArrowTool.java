
package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import com.sun.java.swing.ImageIcon;
import com.sun.java.swing.undo.*;


/**
 * the tool used to perform the different operations associated
 * with the "arrow" tool, that is:
 * selection, area_selection, deselection, moving.
 */ 
public class ArrowTool extends SelecterTool implements DirectionListener, DragListener{

  /**
   * Constructor. 
   */
  public ArrowTool(GraphicContext theGc, ImageIcon theImageIcon) 
  {
    super("arrow", theImageIcon);
    
    gc = theGc;
    
    itsDirectionChooser = new DirectionChooser(this);
    itsSelectionMover = new ExplodeSelectionMover(this, SelectionMover.HORIZONTAL_MOVEMENT);
  }

  
  /**
   * called when this tool is unmounted
   */
  public void deactivate(){}

  
  /**
   * a single object has been selected, in coordinates x, y.
   * Starts a move or a clone operation (if ALT is pressed).
   * overrides the abstract SelecterTool.singleObjectSelected */
  void singleObjectSelected(int x, int y, int modifiers)
  {
    if ((modifiers & InputEvent.ALT_MASK) == 0)
      itsMoveMode = SIMPLE;
    else itsMoveMode = CLONE;
    
    mountIModule(itsDirectionChooser, x, y);
  }

  /** 
   * a group of objects was selected 
   *overrides the abstract SelecterTool.multipleObjectSelected */
  void multipleObjectSelected()
  {
  }


  /**
   * called by the DirectionChooser UI module
   */
  public void directionChoosen(int theDirection) 
  {
    itsSelectionMover.setDirection(theDirection);
    mountIModule(itsSelectionMover, startingPoint.x, startingPoint.y);

  }
  
  /**
   * called by the DirectionChooser UI module
   */
  public void directionAbort()
  {
    mountIModule(itsSelecter);
  }

  /**
   * drag listener called by the SelectionMover UI module,
   * at the end of its interaction.
   * Moves all the selected elements
   */
  public void dragEnd(int x, int y)
  {
    ScrEvent aEvent;
    ScrEvent newEvent;

    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    if (itsMoveMode == CLONE) 
      {
	// starts a serie of undoable add transition
	egc.getDataModel().beginUpdate();
	for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
	  {
	    aEvent = (ScrEvent) e.nextElement();
	    newEvent = new ScrEvent(aEvent.getDataModel(),
				    aEvent.getTime(),
				    aEvent.getPitch(),
				    aEvent.getVelocity(),
				    aEvent.getDuration(),
				    aEvent.getChannel());

	    egc.getAdapter().setX(newEvent, egc.getAdapter().getX(aEvent)+deltaX);
	    egc.getDataModel().addEvent(newEvent);

	  }
	egc.getDataModel().endUpdate();
      }

    else
      {
	// starts a serie of undoable moves
	
	egc.getDataModel().beginUpdate();
	
	for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
	  {
	    aEvent = (ScrEvent) e.nextElement();
	    
	    egc.getAdapter().setX(aEvent, egc.getAdapter().getX(aEvent)+deltaX);
	    egc.getAdapter().setY(aEvent, egc.getAdapter().getY(aEvent)+deltaY);
	  }
	
	egc.getDataModel().endUpdate();
	
      }
    
    mountIModule(itsSelecter);
    gc.getGraphicDestination().repaint();    
  }


  //---Fields

  DirectionChooser itsDirectionChooser;
  SelectionMover itsSelectionMover;

  final static int SIMPLE = 0;
  final static int CLONE = 1;
  int itsMoveMode;
}





