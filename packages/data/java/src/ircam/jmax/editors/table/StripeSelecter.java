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

package ircam.jmax.editors.table;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.toolkit.*;


/**
 * A specific shape of selection for the Table: the vertical stripe.
 * This class uses the functionalities of the Selecter Interactio Module, just
 * redefining the XORDraw method (that is, the way the selection is shown).
 * @see Selecter
 */ 
public class StripeSelecter extends Selecter {

  /**
   * Constructor. */
  public StripeSelecter(GraphicSelectionListener l)
  {
   super(l); 
  }

  public void mouseDragged(MouseEvent e)
  {
      if(startSelection.x <=  e.getX())
	  ((TableGraphicContext) gc).getCoordWriter().postXRange(startSelection.x, e.getX());
      else
	  ((TableGraphicContext) gc).getCoordWriter().postXRange(e.getX(), startSelection.x);
      super.mouseDragged(e);
  }

  /**
   * overrides Selecter.XORDraw. The actual drawing routine
   */
  public void XORDraw(int dx, int dy) 
  {
    TableAdapter ta = ((TableGraphicContext)gc).getAdapter();
    //take care of the "quantization" introduced by the zoom:
    int x1 = ta.getX(ta.getInvX(movingPoint.x));
    int x2 = ta.getX(ta.getInvX(movingPoint.x+dx));

    if (x1 <0) x1=0; 

    tempRect.setBounds(x1, 2/*0*/, x2-x1,gc.getGraphicDestination().getSize().height-4/**/);
    normalizeRectangle(tempRect);

    Graphics g = gc.getGraphicDestination().getGraphics();
    drawGrayRect(g, tempRect);
    g.dispose();

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);
  }


  /**
   * draws the selection */
  private static void drawGrayRect(Graphics g, Rectangle r)
  {
    g.setColor(Color.lightGray);
    g.setXORMode(Color.black); 
    g.fillRect(r.x, r.y, r.width, r.height);
    
    g.setPaintMode();
    g.setColor(Color.black);
  }

  //--- Fields
  Rectangle tempRect = new Rectangle(); 

}

