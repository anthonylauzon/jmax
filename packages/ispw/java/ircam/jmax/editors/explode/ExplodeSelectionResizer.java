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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * an interaction module used to resize a selection of explode objects.
 * At the end of a resize operation, it communicates the new position to the listener
 */
public class ExplodeSelectionResizer extends SelectionResizer {

  /**
   * constructor. 
   */
  public ExplodeSelectionResizer(DragListener theListener) 
  {
    super(theListener);
    
  }
  
  /**
   * overrides SelectionResizer.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    int deltaX = egc.getAdapter().getInvX(e.getX()) - egc.getAdapter().getInvX(itsStartingPoint.x);
    int deltaY = egc.getAdapter().getInvY(e.getY()) - egc.getAdapter().getInvY(itsStartingPoint.y);
    
    egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), " dx "+deltaX);

    super.mouseDragged(e);
  }



  /**
   * from the XORPainter interface. The actual drawing function.
   */
  public void XORDraw(int dx, int dy) 
  {
    previousX = dx;
    previousY = dy;

    Graphics g = gc.getGraphicDestination().getGraphics();
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    g.setColor(Color.darkGray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.
    
    Adapter a = egc.getAdapter();
    ObjectRenderer er = egc.getRenderManager().getObjectRenderer();
    
    ScrEvent aScrEvent;
    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aScrEvent = (ScrEvent) e.nextElement();
	a.setX(tempEvent, a.getX(aScrEvent));
	a.setY(tempEvent, a.getY(aScrEvent));

	a.setLenght(tempEvent, a.getLenght(aScrEvent)+dx);
	er.render(tempEvent, g, false, gc);
      }
	 
    g.setPaintMode();
    g.setColor(Color.black);
    g.dispose();
  }

  //---- Fields

  ScrEvent tempEvent = new ScrEvent(null);
}
