
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The class used to implement the behaviour of the "line" tool of the 
 * Table editor. It uses a LineDrawer interaction module.
 */
public class LinerTool extends TableTool implements LineListener{
  
  /**
   * constructor */
  public LinerTool(ImageIcon theImageIcon)
  {
    super("liner", theImageIcon);

    itsLiner = new LineDrawer(this);
  }

  /**
   * The default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsLiner;
  }


  /**
   * LineListener interface. 
   * Called by the lineDrawer UI module at mouse down.
   */
  public void lineStart(int x, int y, int modifiers) 
  {
    getGc().getDataModel().beginUpdate();
    startingPoint.setLocation(x, y);
  }

  /**
   * LineListener interface. Called by the LineDrawer UI module.
   */
  public void lineEnd(int x, int y) 
  {
    drawLine(startingPoint.x, startingPoint.y, x, y, getGc());
    getGc().getDataModel().endUpdate();
  }

  /**
   * An utility function to draw a line between two given points in the given
   * graphic context. The points are expressed in screen coordinates.
   * This function is also used by the FreeHandDrawer interaction module.
   */
  public static void drawLine(int x1, int y1, int x2, int y2, TableGraphicContext tgc)
  {
    TableAdapter ta = tgc.getAdapter();
    prepareBuffer(tgc.getDataModel());

    if (x1 > x2) 
      {
	int temp;
	temp = y1;
	y1 = y2;
	y2 = temp;
	temp = x1;
	x1 = x2;
	x2 = temp;
      }

    int start = ta.getInvX(x1); 
    int end = ta.getInvX(x2);

    float coeff;
    if (y1 != y2) 
      coeff = ((float)(y1 - y2))/(x2 - x1);
    else coeff = 0;

    coeff *= ta.getXZoom();

    if (end >= tgc.getDataModel().getSize()) end = tgc.getDataModel().getSize()-1;
    for (int i = start; i < end; i+=1)
      {
	buffer[i-start] = ta.getInvY((int) (y1-Math.abs(i-start)*coeff));
      }

    tgc.getDataModel().setValues(buffer, start, end-start);
  }

  /**
   * Utility private function to allocate a buffer used during the line operations.
   * Currently, the computation is done in a private vector that is communicated to 
   * the data model. This is expensive (the memory allocated is the double), but
   * it allows a performance gain.*/
  private static void prepareBuffer(TableDataModel td)
  {
    if (buffer == null || buffer.length < td.getSize())
      {
	buffer = new int[td.getSize()];
      }
  }

  //--- Fields
  LineDrawer itsLiner;

  static int buffer[];
  Point startingPoint = new Point();
  
}

