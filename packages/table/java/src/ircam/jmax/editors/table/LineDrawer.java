//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.table;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.toolkit.*;

import javax.swing.*;

/**
 * A line-drawer interaction module, used by the LinerTool.
 * @see LinerTool
 */
public class LineDrawer extends TableInteractionModule implements XORPainter{

  /**
   * Constructor. This class uses a XORHandler to draw the line's "shadow"
   */
  public LineDrawer(LineListener theListener) 
  {
    super();
    
    itsXORHandler = new XORHandler(this);
    itsListener = theListener;
  }
  

  /**
   * overrides InteractionModule mousePressed()
   */
  public void mousePressed(MouseEvent e) 
  {
    int x = e.getX();
    int y = e.getY();

    interactionBeginAt(x, y);
    itsListener.lineStart(x, y, e.getModifiers());
  } 

  /**
   * overrides InteractionModule mouseMoved()
   * to show the position in the status bar*/
  public void mouseMoved(MouseEvent e)
  {
    getGc().getCoordWriter().postCoordinates(e.getX(), e.getY());
  }


  /**
   * used to set the starting point of the interaction.
   */
  public void interactionBeginAt(int x, int y) 
  {  
    startSelection.setLocation(x, y);
    movingPoint.setLocation(x, y);
    itsXORHandler.beginAt(x, y);
    active = true;
  }

  /**
   * overrides InteractionModule mouseDragged().
   * Draws the shadow of the line and the coordinates.
   */
  public void mouseDragged(MouseEvent e) 
  {
    if (!active) return;//!!
    itsXORHandler.moveTo(e.getX(), e.getY());
    getGc().getCoordWriter().postCoordinates(e.getX(), e.getY());
  }

  /**
   * overrides InteractionModule mouseReleased().
   * Ends the interaction and communicate to the listener.
   */
  public void mouseReleased(MouseEvent e) 
  {
    if (!active) return; //abnormal condition

    itsXORHandler.end();

    itsListener.lineEnd(e.getX(), e.getY());
    active = false;
  }

  /**
   * Overrides InteractionModule unBindFromProducer() to set
   * the active state to false.
   */ 
  protected void unBindFromProducer() 
  {
    super.unBindFromProducer();
    active = false;
  }


  /**
   * from the XORPainter interface
   */
  public void XORErase() 
  {
    XORDraw(0, 0);
  }

  /**
   * from the XOR painter interface. The actual drawing routine
   */
  public void XORDraw(int dx, int dy) 
  {
    Graphics g = gc.getGraphicDestination().getGraphics();

    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);

    g.drawLine(startSelection.x, startSelection.y, movingPoint.x, movingPoint.y);
    g.setPaintMode();
    g.setColor(Color.black);
    g.dispose();
  }


  //--- Fields
  LineListener itsListener;

  Point startSelection = new Point();
  Point movingPoint = new Point();
  XORHandler itsXORHandler;
  CoordinateWriter itsWriter;

  boolean active = false;

}




