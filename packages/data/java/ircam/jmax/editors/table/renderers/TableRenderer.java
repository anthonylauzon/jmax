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
    setMode(FILLED);//the default
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

  public void drawPixSolidPoint( Graphics g, int x, double topy, double bottomy, int zero)
  {
    int width = (int)( gc.getAdapter().getXZoom());
    if (width < 1) width = 1;
    int height, y;
    
    if( topy*bottomy > 0)
    {
        if( topy > 0)
        {
            y = gc.getAdapter().getY( topy);
            height =  zero-y;
        }
        else
        {
            height = gc.getAdapter().getY( bottomy)-zero;
            y = zero;
        }
    }
    else
    {
        y = gc.getAdapter().getY( topy);
        height = gc.getAdapter().getY( bottomy) - y;
    }

    g.fillRect( x, y, width, height);
  }

  public void drawBoundPoint( Graphics g, int x, int y, int nextx, int nexty)
  {
    g.drawLine( x, y, nextx, nexty);
  }

  public void drawHollowPoint( Graphics g, int x, int y)
  {
    int width = (int)( gc.getAdapter().getXZoom());
    int height = (int) gc.getAdapter().getYZoom();
    
    if (width < 1) width = 1;    
    if (height == 0) height = 1;

    g.fillRect(x,  y, width, height);
  }

  public void drawPixHollowPoint( Graphics g, int x, double topy, double bottomy)
  {
    int width = (int)( gc.getAdapter().getXZoom());
    int height = (int) gc.getAdapter().getYZoom();
    
    if (width < 1) width = 1;    
    if (height == 0) height = 1;

    g.fillRect(x,  gc.getAdapter().getY( topy), width, height);
    g.fillRect(x,  gc.getAdapter().getY( bottomy), width, height);
  }
  
  public void drawPixBoundPoint( Graphics g, int x, double topy, double bottomy, int nextx, double nexttopy, double nextbottomy, int zero)
  {
    int ty = gc.getAdapter().getY( topy);
    int by = gc.getAdapter().getY( bottomy);
    int nty = gc.getAdapter().getY( nexttopy);
    int nby = gc.getAdapter().getY( nextbottomy);

    g.drawLine( x, ty, nextx, nty);
    g.drawLine( x, by, nextx, nby);
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
    g.fillRect(r.x-10, yMax, r.width+20, (yMin-yMax));
    g.setColor( borderRangeColor);
    g.drawLine( r.x, yMax, r.x+r.width, yMax);
    g.drawLine( r.x, yMin, r.x+r.width, yMin);
    
    g.setColor( foreColor);
    
    int zero = gc.getAdapter().getY(0);
    if(gc.getAdapter().getXZoom() > 0.501)
      {
	if((gc.getFtsObject().getVisibleSize()==0)||(gc.getFtsObject().getLastUpdatedIndex()==0)) return;
	
	int index = gc.getAdapter().getInvX( r.x);
	int visibleSize = gc.getAdapter().getInvX( r.x+r.width);
	
	int tableSize = gc.getFtsObject().getSize();
	
	int firstVisible = gc.getFirstVisibleIndex();
	int visibleScope = gc.getVisibleHorizontalScope();

	if( itsMode == FILLED)
	  for (int i = index; (i < visibleSize)&&(i<tableSize); i++)
	    drawSolidPoint(g, gc.getAdapter().getX(i), 
			   gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i)), zero);	    
	else 
	  if( itsMode == POINTS)
	    for (int i = index; (i < visibleSize)&&(i<tableSize); i++)
	      drawHollowPoint(g, gc.getAdapter().getX(i), 
			      gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i)));
	  else
	    {//LINES
	      int i;              
              if(index>0)
                drawBoundPoint(g, gc.getAdapter().getX(index-1), 
			       gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(index-1)),
			       gc.getAdapter().getX(index), 
			       gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(index)));
              
	      for (i = index; (i < visibleSize)&&(i<tableSize-1); i++)
		drawBoundPoint(g, gc.getAdapter().getX(i), 
			       gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i)),
			       gc.getAdapter().getX(i+1), 
			       gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i+1)));
	      if( i == tableSize-1)
		{
		  int x = gc.getAdapter().getX(i);
		  int y = gc.getAdapter().getY( gc.getFtsObject().getVisibleValue(i));
		  drawBoundPoint(g, x, y, x, y);
		}
	    }
      }
    else
      {
	int pixSize = gc.getFtsObject().getPixelsSize();	      
	if( itsMode == FILLED)
            for (int i = 0 ; i < pixSize-1; i++)
                drawPixSolidPoint( g, i, gc.getFtsObject().getTopPixel(i), 
                                   gc.getFtsObject().getBottomPixel(i), zero);
        else
            if( itsMode == POINTS)
                for (int i = 0; i < pixSize-1; i++)
                    drawPixHollowPoint( g, i, gc.getFtsObject().getTopPixel(i), 
                                        gc.getFtsObject().getBottomPixel(i));
            else//LINES
            {
	      for (int i = 0; i < pixSize-2; i++)
		 drawPixBoundPoint(g, i, 
                                   gc.getFtsObject().getTopPixel(i),
			           gc.getFtsObject().getBottomPixel(i),
                                   i+1,
			           gc.getFtsObject().getTopPixel(i+1),
                                   gc.getFtsObject().getBottomPixel(i+1), zero);

              drawPixBoundPoint(g, pixSize-2, 
                                gc.getFtsObject().getTopPixel( pixSize-2), 
                                gc.getFtsObject().getBottomPixel( pixSize-2), 
                                pixSize-1,
                                gc.getFtsObject().getTopPixel( pixSize-1), 
                                gc.getFtsObject().getBottomPixel( pixSize-1), zero);
            }
      }
    
    g.setColor( Color.red);
    g.drawLine( r.x, zero, r.x + r.width, zero);
  }

  /**
   * set the POINTS, FILLED or LINES mode */
  public void setMode(int mode)
  {
    itsMode = mode;
  }
  public int getMode()
  {
    return itsMode;
  }

  public void setBackColor(Color bc)
  {
    backColor = bc;
  }
  public Color getBackColor()
  {
    return backColor;
  }
  public void setForeColor(Color fc)
  {
    foreColor = fc;
  }
  public Color getForeColor()
  {
    return foreColor;
  }
  
  //--- Fields

  TableGraphicContext gc;
  int itsMode;

  TopLayer itsTopLayer;
  public final static int POINTS = 0;
  public final static int FILLED = 1;
  public final static int LINES = 2;
  
  Color backColor = new Color(220, 220, 255);
  Color borderRangeColor = new Color(200, 200, 255);
  Color outRangeColor = new Color(237, 237, 237);
  Color foreColor = new Color(101, 153, 255);
}



