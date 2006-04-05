//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.table;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.toolkit.*;

import javax.swing.*;

public class PositionDrawer extends TableInteractionModule implements XORPainter {

  public PositionDrawer(TablePositionListener theListener) 
  {
    super();
    
    itsXORHandler = new XORHandler(this);
    itsListener = theListener;
  }
  
  public void activate(int x, int y)
  {
    interactionBeginAt(x, y);
    itsListener.startFollowPosition();
    active = true;
  }

  public void mousePressed(MouseEvent e)
  {
    if(active)
    {
      itsXORHandler.end();
      itsListener.stopFollowPosition( e.getX(), e.getY(), e);
      active = false;
    }
  }
  public void mouseMoved(MouseEvent e)
  {
    if(active)
      itsXORHandler.moveTo(e.getX(), e.getY());
    else
      activate(e.getX(), e.getY());
  }
  public void mouseEntered(MouseEvent e) 
  {
    activate(e.getX(), e.getY());
  }
  public void mouseExited(MouseEvent e) 
  {
    if(active)
    {
      itsXORHandler.moveTo(e.getX(), e.getY());
      itsXORHandler.end();
      active = false;
    } 
  }
  
  /**
   * used to set the starting point of the interaction.
   */
  public void interactionBeginAt(int x, int y) 
  {  
    movingPoint.setLocation(x, y);
    itsXORHandler.beginAt(x, y);
  }

  public void mouseDragged(MouseEvent e){}
  public void mouseReleased(MouseEvent e){}

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
    Rectangle rect = gc.getGraphicDestination().getBounds();
    
    g.setColor(((TableDisplay)gc.getGraphicDestination()).getRenderer().getBackColor());
    g.setXORMode(((TableDisplay)gc.getGraphicDestination()).getRenderer().getForeColor()); //there's an assumption here on the color of the background.
      
    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);
    
    g.drawLine(movingPoint.x, rect.y, movingPoint.x, rect.height);
    g.drawLine(/*rect.x*/0, movingPoint.y, rect.width, movingPoint.y);
    
    /*((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);	
    g.setFont(Tabler.displayFont);
    g.drawString(""+movingPoint.x, movingPoint.x+1, rect.y+10);
    g.drawString(""+movingPoint.y, rect.width-20, movingPoint.y-2);
    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);	*/
    
    g.setColor( Color.black);
    g.setPaintMode();
    g.dispose();  
  }


  //--- Fields
  TablePositionListener itsListener;

  Point startSelection = new Point();
  Point movingPoint = new Point();
  XORHandler itsXORHandler;
  boolean active = false;

}




