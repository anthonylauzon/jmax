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

package ircam.jmax.editors.table.renderers;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;
import java.awt.*;
import javax.swing.event.*;

/**
 * The Layer used to draw the selection in the table.
 * The selected range is drawed as a gray rectangle in XOR.
 * This Layer is a listener of the selection active state:
 * When a change occurs, the selection is redrawn in the correct color. */
public class SelectionLayer extends AbstractLayer{

  public SelectionLayer(TableGraphicContext theTgc)
  {
    tgc = theTgc;

    // make the selection change its color when the owner changes
    tgc.getSelection().setOwner( new SelectionOwner() {
      public void selectionDisactivated()
	{
	  Graphics g = tgc.getGraphicDestination().getGraphics();

	  if (g == null) return; 
	      
	  eraseSelection(g);
	  render(g, 1);
	  g.dispose();
	}
      public void selectionActivated()
	{
	   Graphics g = tgc.getGraphicDestination().getGraphics();
	   eraseSelection(g);
	   render(g, 1);
	   g.dispose();
	}
    });

  }

  /**
   * Layer interface. */
  public void render(Graphics g, int order)
  {
    
      if (tgc.getSelection().getCaretPosition() != TableSelection.NO_CARET)
      {
	int x = tgc.getAdapter().getX(tgc.getSelection().getCaretPosition());
	g.setColor(Color.black);
	g.setXORMode(Color.white);
	g.drawLine(x, 0, x, tgc.getGraphicDestination().getSize().height);
	g.setPaintMode();
      }
    else 
      {
	int x1 = tgc.getAdapter().getX(tgc.getSelection().getFirstSelected());
	int x2 = tgc.getAdapter().getX(tgc.getSelection().getLastSelected());
	
	tempRect.setBounds(x1, 0, x2-x1, tgc.getGraphicDestination().getSize().height);
	Selecter.normalizeRectangle(tempRect);
	
	boolean selected_active = (tgc.getDataModel() == TableSelection.getCurrent().getModel());
	/*if (selected_active) 
	    g.setColor(Color.gray);
	    else*/ 
	g.setColor(Color.lightGray);
	
	g.setXORMode(Color.black);
	
	drawGrayRect(g, tempRect);
	g.setPaintMode();
	
	lastSelectionDraw = tempRect;
	lastActiveState = selected_active;
      }
  }


  /**
   * actual drawing routine */
  public void drawGrayRect(Graphics g, Rectangle r)
  {
      g.fillRect(r.x, r.y, r.width, r.height);
  }

  /**
   * Routine to quickly erase the previous selection (used for ex. when
   * the selection change state form active to inactive) */
  public void eraseSelection(Graphics g)
  {

      if (lastSelectionDraw == null || 
	lastSelectionDraw.isEmpty() || 
	lastSelectionDraw.width == 0) 
      {
	return;
      }

    if (lastActiveState) g.setColor(Color.gray);
    else g.setColor(Color.lightGray);
    g.setXORMode(Color.white); 

    drawGrayRect(g, lastSelectionDraw);
    g.setPaintMode();
    lastSelectionDraw = null;
  }

  //--- Fields
  TableGraphicContext tgc;
  Rectangle tempRect = new Rectangle();
  Rectangle lastSelectionDraw;
  boolean lastActiveState;
}












