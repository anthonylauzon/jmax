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
package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

/**
 * A rubber-banding interaction module.
 * It takes care of drawing a "selection rectangle" in a 
 * component, and communicate the result to a GraphicSelectionListener
 */
public class Selecter extends InteractionModule implements XORPainter {

  /**
   * Constructor. This class uses a XORHandler to draw the shape
   * of the rubber banding rectangle
   */
  public Selecter(GraphicSelectionListener theListener) 
  {
    super();
    
    itsXORHandler = new XORHandler(this);
    itsListener = theListener;
  }
  

  /**
   * overrides InteractionModule.mousePressed()
   */
  public void mousePressed(MouseEvent e) 
  {
    int x = e.getX();
    int y = e.getY();

    interactionBeginAt(x, y);
    itsListener.selectionPointChoosen(x, y, e.getModifiers());
  } 


  /**
   * used to set the starting point of the interaction.
   */
  public void interactionBeginAt(int x, int y) 
  {  
    startSelection.setLocation(x, y);
    movingPoint.setLocation(x, y);
    itsXORHandler.beginAt(x, y);
    itsRunningG = gc.getGraphicDestination().getGraphics();
    active = true;
  }

  /**
   * overrides InteractionModule.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    if (!active) return;//!!
    itsXORHandler.moveTo(e.getX(), e.getY());
  }

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    if (!active) return; //abnormal condition
    int x = e.getX();
    int y = e.getY();

    itsXORHandler.end();

    tempRect.setBounds(startSelection.x, startSelection.y, x-startSelection.x, y-startSelection.y);
    normalizeRectangle(tempRect);
    
    
    itsListener.selectionChoosen(tempRect.x, tempRect.y, tempRect.width, tempRect.height);
    active = false;
    itsRunningG.dispose();
  }


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

    itsRunningG.setColor(Color.gray);
    itsRunningG.setXORMode(Color.white); //there's an assumption here on the color of the background.

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);

    tempRect.setBounds(startSelection.x, startSelection.y, movingPoint.x-startSelection.x, movingPoint.y-startSelection.y);
    normalizeRectangle(tempRect);

    itsRunningG.drawRect(tempRect.x, tempRect.y, tempRect.width, tempRect.height);

    itsRunningG.setPaintMode();
    itsRunningG.setColor(Color.black);
  }


  /**
   * utility function. Gets rid of the "negative width/lenght" problems
   * in rectangles.
   */
  public static void normalizeRectangle(Rectangle r) 
  {  
    /* sets the origin */
    if (r.width < 0) r.x = r.x+r.width;
    
    if (r.height < 0) r.y = r.y+r.height;

    /* sets width and height */
    if (r.width<0) r.width = -r.width;
    if (r.height<0) r.height = -r.height;
  }

  //--- Fields
  protected GraphicSelectionListener itsListener;

  protected Point startSelection = new Point();
  protected Point movingPoint = new Point();
  XORHandler itsXORHandler;

  boolean active = false;
  Rectangle tempRect = new Rectangle();
  Graphics itsRunningG;
}




