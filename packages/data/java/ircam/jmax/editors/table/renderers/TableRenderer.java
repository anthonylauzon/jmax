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
import ircam.jmax.JMaxApplication;

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
    setMode(SOLID);//the default
    addLayer(this);
    addLayer( new TopLayer( theGc));
    addLayer( new SelectionLayer( theGc));
  }

  public void drawSolidPoint( Graphics g, int x, int y, int zero)
  {
    int width = (int)( gc.getAdapter().getXZoom());
    int height = Math.abs(zero-y);
    
    if (width < 1) width = 1;
    
    int startY = (y <= zero) ? y : zero; //remember: y are (graphically) INVERTED!  
    
    g.fillRect( x, startY, width, height);
  }

  public void drawHollowPoint( Graphics g, int x, int y)
  {
    int width = (int)( gc.getAdapter().getXZoom());
    int height = (int) gc.getAdapter().getYZoom();
    
    if (width < 1) width = 1;    
    if (height == 0) height = 1;

    g.fillRect(x,  y, width, height);
  }
  /**
   * From the Layer interface: renders the content of the table (except the
   * zero line and the surrounding gray, that are rendered by the TopLayer */
  public void render(Graphics g, int order)
  {
    render(g, gc.getGraphicDestination().getBounds(), order);
  }

  /** 
   * Layer interface */
  public void render(Graphics g, Rectangle r, int order)
  {    
    g.setColor( backColor);
    g.fillRect(r.x, r.y, r.width, r.height);
   
    g.setColor( foreColor);
    
    if(gc.getAdapter().getXZoom()>0.5)
      {
	if((gc.getFtsObject().getVisibleSize()==0)||(gc.getFtsObject().getLastUpdatedIndex()==0)) return;
	int index = gc.getFirstVisibleIndex();
	int visibleSize = gc.getVisibleHorizontalScope();
	int tableSize = gc.getFtsObject().getSize();

	if( itsMode == SOLID)
	  {
	    int zero = gc.getAdapter().getY(0);
	    for (int i = 0; (i < visibleSize)&&(index+i<tableSize); i++)
	      drawSolidPoint(g, (int)(i*gc.getAdapter().getXZoom()), 
			     gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(index+i)), zero);
	  }
	else
	  for (int i = 0; (i < visibleSize)&&(index+i<tableSize); i++)
	    drawHollowPoint(g, (int)(i*gc.getAdapter().getXZoom()), 
			    gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(index+i)));
      }
    else
      {
	int pixSize = gc.getFtsObject().getPixelsSize();	      
	if( itsMode == SOLID)
	  {
	    int zero = gc.getAdapter().getY(0);
	    for (int i = 0; i < pixSize; i++)
	      drawSolidPoint( g, i, gc.getAdapter().getY(gc.getFtsObject().getPixel(i)), zero);
	  }
	else
	  for (int i = 0; i < pixSize; i++)
	    drawHollowPoint( g, i, gc.getAdapter().getY(gc.getFtsObject().getPixel(i)));
      }
    
    g.setColor( Color.red);
    g.drawLine( 0, gc.getAdapter().getY(0), r.width, gc.getAdapter().getY(0));
  }



  /**
   * set the HOLLOW or SOLID mode */
  public void setMode(int mode)
  {
    itsMode = mode;
  }
  public int getMode()
  {
    return itsMode;
  }
  
  //--- Fields

  TableGraphicContext gc;
  int itsMode;

  TopLayer itsTopLayer;
  public final static int HOLLOW = 0;
  public final static int SOLID = 1;
  
  Color backColor = new Color(247, 247, 247);
  Color foreColor = new Color( 0, 0, 0, 60);
}

