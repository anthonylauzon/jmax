
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
    drawLine(startingPoint.x, startingPoint.y, x, y);
    getGc().getDataModel().endUpdate();
  }

  /**
   * An utility function to draw a line between two given points in the given
   * graphic context. The points are expressed in screen coordinates.
   */
  public void drawLine(int x1, int y1, int x2, int y2)
  {
    TableAdapter ta = getGc().getAdapter();

    if (x1 > x2) //the line is given 'a l'inverse'
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

    getGc().getDataModel().interpolate(start, end, ta.getInvY(y1), ta.getInvY(y2));
  }

  //--- Fields
  LineDrawer itsLiner;

  Point startingPoint = new Point();
  
}

