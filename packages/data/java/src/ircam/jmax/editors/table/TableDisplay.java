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

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.renderers.*;
import ircam.jmax.editors.table.menus.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A JPanel that is able to show the content of a TableDataModel.*/
public class TableDisplay extends PopupToolbarPanel
{

  /**
   * Constructor. */
  public TableDisplay(TablePanel tp)
  {
    super();
    panel = tp;
    setBackground(Color.white);
    popup = new TablePopupMenu(this);
  }

  /**
   * Constructor with a table render */
  public TableDisplay(TableRenderer tr, TableGraphicContext gc, TablePanel tp)
  {
    super();
    this.tr = tr;
    panel = tp;
    this.gc = gc;
    setBackground(Color.white);
    popup = new TablePopupMenu(this);
  }

  public JPopupMenu getMenu()
  {
      return popup;
  }

  /**
   * Specify which render to use for this TableDisplay */
  public void setRenderer(TableRenderer tr)
  {
    this.tr = tr;
  }

  public void setGraphicContext(TableGraphicContext tgc)
  {
      gc = tgc;
  }
  public TableGraphicContext getGraphicContext()
  {
      return gc;
  }
  /**
   * The paint routine just delegates to the RenderManager
   * the representation of its data.*/
  public void paintComponent(Graphics g) 
  {
      tr.render(g, g.getClipBounds());
  }
  
  /**
   * paints all the points in a given (closed) range */
  public void rangePaint(int index1, int index2)
  {
      Graphics g = getGraphics();

      for (int i = index1; i<= index2; i++)
	  tr.renderPoint(g, i);
    
      g.dispose();
  }
  
    /**
     * paints a single point */
  public void singlePaint(int index)
  {
      Graphics g = getGraphics();
      
      tr.renderPoint(g, index);
      g.dispose();
  }
  
  //--- Fields
  TableRenderer tr;
  TablePopupMenu popup;
  TableGraphicContext gc;
  public TablePanel panel;
}

