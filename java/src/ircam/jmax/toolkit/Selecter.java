//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

/**
 * A rubber-banding interaction module.
 * It communicates the point of selection (eventually, the double-click action), and
 * takes care of drawing a "selection rectangle" in a 
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

    if (e.getClickCount() > 1) itsListener.selectionPointDoubleClicked(x, y, e.getModifiers());
    interactionBeginAt(x, y);
    itsListener.selectionPointChoosen(x, y, e.getModifiers());
    /*active = true;*/
    InteractionSemaphore.lock();    
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
    InteractionSemaphore.lock();
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
    if (!active) 
      {
	InteractionSemaphore.unlock();
	return; //abnormal condition
      }
    int x = e.getX();
    int y = e.getY();

    itsXORHandler.end();

    tempRect.setBounds(startSelection.x, startSelection.y, x-startSelection.x, y-startSelection.y);
    normalizeRectangle(tempRect);
        
    itsListener.selectionChoosen(tempRect.x, tempRect.y, tempRect.width, tempRect.height);
    active = false;
    itsRunningG.dispose();
    InteractionSemaphore.unlock();
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
    Graphics g = gc.getGraphicDestination().getGraphics();

    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);

    tempRect.setBounds( startSelection.x, startSelection.y, movingPoint.x-startSelection.x, movingPoint.y-startSelection.y);
    normalizeRectangle(tempRect);

    if((tempRect.width>=2)||(tempRect.height>=2)) 
      g.drawRect(tempRect.x, tempRect.y, tempRect.width, tempRect.height);

    g.setPaintMode();
    g.setColor(Color.black);
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
  protected Graphics itsRunningG;
}




