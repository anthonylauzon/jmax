package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;

/**
 * The class implementing the behaviour of the pencil in the Table editor.
 * It uses a FreeHandDrawer Interaction Module
 * @see TableTool
 */ 
public class PencilTool extends TableTool implements DynamicDragListener {

  /**
   * Constructor.
   */
  public PencilTool(ImageIcon theImageIcon) 
  {
    super("pencil", theImageIcon);

    itsFreeHand = new FreeHandDrawer(this);
  }


  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsFreeHand;
  }


  /**
   * DynamicDragListener interface */
  public void dragStart(int x, int y)
  {
    Graphics g = getGc().getGraphicDestination().getGraphics();
    getGc().getDataModel().beginUpdate();
    setPoint(x, y);
    g.dispose();
  }

  /**
   * DynamicDragListener interface */
  public void dynamicDrag(int x, int y)
  {
    //setPoint(x, y);
  }
  
  /**
   * DynamicDragListener interface */
  public void dragEnd(int x, int y)
  {
    setPoint(x, y);
    ((TableGraphicContext) gc).getDataModel().endUpdate();
  }

  /** set the value of a point in the model */
  private void setPoint(int x, int y)
  {
    TableGraphicContext tgc = (TableGraphicContext)gc;
    
    tgc.getDataModel().setValue(tgc.getAdapter().getInvX(x), tgc.getAdapter().getInvY(y));
  }

  //-------------- Fields

  FreeHandDrawer itsFreeHand;
}



