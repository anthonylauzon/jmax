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
    int yMax = gc.getAdapter().getY( (gc.isIvec()) ? gc.getVerticalMaximum() : gc.getVerticalMaximum()/100);
    int yMin = gc.getAdapter().getY( (gc.isIvec()) ? gc.getVerticalMinimum() :  gc.getVerticalMinimum()/100);

    g.setColor( outRangeColor);
    g.fillRect(r.x, r.y, r.width, r.height);
    g.setColor( backColor);
    g.fillRect(r.x, yMax, r.width, (yMin-yMax));
    g.setColor( borderRangeColor);
    g.drawLine( r.x, yMax, r.x+r.width, yMax);
    g.drawLine( r.x, yMin, r.x+r.width, yMin);
    
    g.setColor( foreColor);
    
    int zero = gc.getAdapter().getY(0);
    if(gc.getAdapter().getXZoom() >= 0.5)
      {
	if((gc.getFtsObject().getVisibleSize()==0)||(gc.getFtsObject().getLastUpdatedIndex()==0)) return;
	
	int index = gc.getAdapter().getInvX( r.x);
	int visibleSize = gc.getAdapter().getInvX( r.x+r.width);
	
	int tableSize = gc.getFtsObject().getSize();
	
	int firstVisible = gc.getFirstVisibleIndex();
	int visibleScope = gc.getVisibleHorizontalScope();

	if( itsMode == SOLID)
	  for (int i = index; (i < visibleSize)&&(i<tableSize); i++)
	    drawSolidPoint(g, gc.getAdapter().getX(i), 
			   gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i)), zero);	    
	else
	  for (int i = index; (i < visibleSize)&&(i<tableSize); i++)
	    drawHollowPoint(g, gc.getAdapter().getX(i), 
			    gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i)));
      }
    else
      {
	int pixSize = gc.getFtsObject().getPixelsSize();	      
	if( itsMode == SOLID)
	  for (int i = 0 ; i < pixSize; i++)
	    drawSolidPoint( g, i, gc.getAdapter().getY(gc.getFtsObject().getPixel(i)), zero);
	else
	  for (int i = 0; i < pixSize; i++)
	    drawHollowPoint( g, i, gc.getAdapter().getY(gc.getFtsObject().getPixel(i)));	
      }
    
    g.setColor( Color.red);
    g.drawLine( r.x, zero, r.x + r.width, zero);
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
  Color borderRangeColor = new Color(229, 229, 229);
  Color outRangeColor = new Color(237, 237, 237);
  Color foreColor = Color.lightGray;
}



