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

package ircam.jmax.editors.table.renderers;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.MaxApplication;

/**
 * The main class for a table representation.
 * This Render acts also like a Layer (the foreground layer of the table)
 */
public class TableRenderer extends AbstractRenderer implements Layer{
  
  /**
   * Constructor.
   */
  public TableRenderer(TableGraphicContext theGc) 
  {  
    gc = theGc;
    setMode(HOLLOW);//the default
    addLayer(this);
    addLayer( new TopLayer( theGc));
    addLayer( new SelectionLayer( theGc));
  }


  /**
   * This method is the substitution of the ObjectRenderer mechanism
   * for editors based on arrays of (int)values instead of objects. 
   * This mechanism can be represented as an ObjectRenderer that draws Points, but
   * this means building a Point each single value painting...*/
  private void render(Graphics g, int x, int y)
  {
    int zero = gc.getAdapter().getY(0);
    int width = (int)( gc.getAdapter().getXZoom());
    int height;

    if (width < 1) width = 1;
    
    // erase the old point
    g.setColor(Color.white);
    g.fillRect(x, 0, width, gc.getGraphicDestination().getSize().height);

    // redraw it
    g.setColor(Color.black);
    
    if (itsMode == SOLID)
      {
	int startY = (y <= zero)?y:zero; //remember: y are (graphically) INVERTED!  
	height = Math.abs(zero-y);

	g.fillRect(x, startY, width, height);
      }
    else
      {
	height = (int) gc.getAdapter().getYZoom();
	if (height == 0) height = 1;

	g.fillRect(x,  y, width, height);
      }
  
    // draw the red line
    g.setColor(Color.red);
    g.setXORMode(Color.white);
    g.fillRect(x, zero, width, 1);
    g.setPaintMode();
  }

  /**
   * Renders a single value in the table */
  public void renderPoint(Graphics g, int index)
  {
    if (index < 0 || index >= gc.getDataModel().getSize()) return;
    int val = gc.getDataModel().getValue(index);

    render(g, gc.getAdapter().getX(index), gc.getAdapter().getY(val));
  }

  public void renderPoint(Graphics g, int index, int value)
  {
    //render(g, gc.getAdapter().getX(index), gc.getAdapter().getY(value));
    render(g, (int)(index*gc.getAdapter().getXZoom()), gc.getAdapter().getY(value));
  }
  /**
   * From the Layer interface: renders the content of the table (except the
   * zero line and the surrounding gray, that are rendered by the TopLayer */
  public void render(Graphics g, int order)
  {
      /*g.setColor(Color.black);

	for (int i = 0; i< gc.getDataModel().getSize(); i++)
	{
	renderPoint(g, i);
	}*/
      render(g, gc.getGraphicDestination().getBounds(), order);
  }

  /** 
   * Layer interface */
  public void render(Graphics g, Rectangle r, int order)
  {    
      //g.setColor(Color.white);
      //g.fillRect(r.x, r.y, r.width, r.height);
      if(gc.getAdapter().getXZoom()>0.5)
	  {
	      if((gc.getFtsObject().getVisibleSize()==0)||(gc.getFtsObject().getLastUpdatedIndex()==0)) return;
	      int index = gc.getFirstVisibleIndex();
	      int visibleSize = gc.getVisibleHorizontalScope();
	      int tableSize = gc.getFtsObject().getSize();

	      for (int i = 0; (i < visibleSize)&&(index+i<tableSize); i++)
		  renderPoint(g, i, gc.getFtsObject().getVisibleValue(index+i));
	  }
      else
	  {
	      int pixSize = gc.getFtsObject().getPixelsSize();	      
	      for (int i = 0; i < pixSize; i++)
		  render(g, i, gc.getAdapter().getY(gc.getFtsObject().getPixel(i)));
	  }
      /*g.setColor(Color.red);
	g.setXORMode(Color.white);
	g.fillRect(0, gc.getAdapter().getY(0), r.width, 1);
	g.setPaintMode();*/
  }



  /**
   * set the HOLLOW or SOLID mode */
  public void setMode(int mode)
  {
    itsMode = mode;
  }

  
  //--- Fields

  TableGraphicContext gc;
  int itsMode;

  TopLayer itsTopLayer;
  public final static int HOLLOW = 0;
  public final static int SOLID = 1;
}
