
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.ImageIcon;

/**
 * the tool used to perform the different operations associated
 * with the "arrow" tool, that is:
 * selection, area_selection, deselection, moving, resizing etc.
 * The default (initial) interface module for this tool is the MouseTracker.
 */ 
public class ArrowTool extends ScrTool implements PositionListener, SelectionListener{

  Component itsGraphicObject;
  RenderProvider itsRenderProvider;
  MouseTracker itsMouseTracker;
  Selecter itsSelecter;
  SelectionHandler itsSelectionHandler;

  /**
   * Constructor. It needs to know the panel and the SelectionHandler
   * on which to operate 
   */
  public ArrowTool(Component theGraphicObject, RenderProvider theRenderProvider, SelectionHandler theSelectionHandler) {
    super("arrow", new ImageIcon("/u/worksta/maggi/projects/max/images/tool_bang.gif"));
    itsGraphicObject = theGraphicObject;
    itsSelectionHandler = theSelectionHandler;
    itsRenderProvider = theRenderProvider;
    itsMouseTracker = new MouseTracker(this, itsGraphicObject);
    itsSelecter = new Selecter(this, itsGraphicObject);
  }

  public void activate() {

    mountIModule(itsMouseTracker);
  }

  public void deactivate(){}

  //position listening (MouseTracker InterfaceModule)
  public void positionChoosen(int x, int y, int modifiers) {
    ScrEvent aScrEvent = itsRenderProvider.getRenderer().eventContaining(x, y);
    
    if (aScrEvent == null)
      {
	if ((modifiers & InputEvent.SHIFT_MASK) == 0)
	  itsSelectionHandler.deselectAll();
	itsGraphicObject.repaint();
	itsSelecter.interactionBeginAt(x, y);
	mountIModule(itsSelecter);
      }
    else 
      {
	System.err.println("vorresti muovere?");
      }
  }

  //selection listening (Selecter IM)
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    itsSelectionHandler.selectArea(x, y, w, h);
    itsGraphicObject.repaint();
    mountIModule(itsMouseTracker);
  }

}





