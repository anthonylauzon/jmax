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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "Display" graphic object.
//

public class Fork extends GraphicObject
{
  private static final int DEFAULT_DISTANCE = 25;
  static final int DEFAULT_WIDTH = DEFAULT_DISTANCE + 2 * ObjectGeometry.INOUTLET_PAD;
  private static final int MINIMUM_WIDTH = DEFAULT_WIDTH;
  static final int CONST_HEIGHT = 12;
  static final int DEFAULT_OUTLETS = 2;

  private int nOutlets = 0;

  public Fork(FtsGraphicObject theFtsObject) 
  {
    super(theFtsObject);

    nOutlets = theFtsObject.getNumberOfOutlets();
    if(nOutlets==0)
	{
	    nOutlets = DEFAULT_OUTLETS;
	    ((FtsForkObject)ftsObject).requestSetOutlets(nOutlets);
	}

    setInletDistance(DEFAULT_DISTANCE);
    setOutletDistance(DEFAULT_DISTANCE);
  }

  public void setDefaults()
  {
    super.setWidth( DEFAULT_WIDTH);
    super.setHeight(CONST_HEIGHT);
    setInletDistance(DEFAULT_DISTANCE);
    setOutletDistance(DEFAULT_DISTANCE);
  }

  // redefined from base class

  public void setWidth( int theWidth)
  {
    if( theWidth <= 0)
      theWidth = DEFAULT_WIDTH;
    else
      {
	int minWidth = ObjectGeometry.HIGHLIGHTED_INOUTLET_WIDTH * nOutlets;
      
	if(theWidth < minWidth)
	  theWidth = minWidth;
      }

    super.setWidth( theWidth);
  }

  public void setHeight(int h)
  {
    super.setHeight(CONST_HEIGHT);
  }

  public void setWidthShift( int theWidth) 
  {
    int minWidth = 2 * getOutletDistance() + 2 * ObjectGeometry.INOUTLET_PAD - 1;

    if (theWidth < minWidth)
      {
	theWidth = minWidth;
	nOutlets = 2;
      }
    else
      nOutlets = (theWidth - 2 * ObjectGeometry.INOUTLET_PAD) / getOutletDistance() + 1;
      
    ((FtsForkObject)ftsObject).requestSetOutlets(nOutlets);

    theWidth = (nOutlets - 1) * getOutletDistance() + 2 * ObjectGeometry.INOUTLET_PAD;

    ftsObject.setWidth(theWidth);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public int getOutletAnchorX(int outlet)
  {
    return getX() + ObjectGeometry.INOUTLET_PAD + outlet * getOutletDistance();
  }

  public int getOutletAnchorY(int outlet)
  {
    return getY() + getHeight();
  }

  public int getInletAnchorX(int inlet)
  {
    return getX() + ObjectGeometry.INOUTLET_PAD;
  }

  public int getInletAnchorY(int inlet)
  {
    return getY() - 1;
  }

  public void paintInlets(Graphics g)
  {
    /* just one inlet any way!! */
    int x, y;
    
    x = getInletAnchorX(0);
    y = getInletAnchorY(0);
    
    if (itsSketchPad.isHighlightedInlet(this, 0))
      {
	g.drawOval( x - 2, y - 5 + ObjectGeometry.INLET_OVERLAP + ObjectGeometry.INLET_OFFSET, 4, 4);
      }
    else
      {
	g.fillRect( x - 1, y - 4 + ObjectGeometry.INLET_OVERLAP + ObjectGeometry.INLET_OFFSET, 3, 3);
      }
  }

  public void paintOutlets(Graphics g)
  {
    for ( int i = 0; i < nOutlets; i++)
      {
	int x, y;

	x = getOutletAnchorX(i);
	y = getOutletAnchorY(i);

	if (itsSketchPad.isHighlightedOutlet(this, i))
	  {
	    g.drawOval( x - 2, y - 4 + ObjectGeometry.OUTLET_OVERLAP + ObjectGeometry.OUTLET_OFFSET, 4, 4);
	  }
	else
	  {
	    g.fillRect( x - 1, y - 3 + ObjectGeometry.OUTLET_OVERLAP + ObjectGeometry.OUTLET_OFFSET, 3, 3);
	  }
      }
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int x_out = getOutletAnchorX(0);
    
    if (!isSelected())
      {
	g.setColor( Color.lightGray);
	g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
	
	g.setColor( Color.black);
	
	paintInlets(g);
	paintOutlets(g);

	g.drawLine(x_out, y + h/2, getOutletAnchorX(nOutlets-1), y + h/2);
	g.drawLine(x_out, y + 1, x_out, y + h - 2);
	
	for(int i=1; i<nOutlets; i++)
	  {
	    x_out = getOutletAnchorX(i);
	    g.drawLine(x_out, y + h/2, x_out, y + h - 2);
	  }
      }
    else
      {
	g.setColor( Color.lightGray);
	g.fillRect( getX(), getY(), getWidth(), getHeight());
	
	g.setColor( Color.black);
	
	paintInlets(g);
	paintOutlets(g);

	g.drawLine(x_out, y + h/2, getOutletAnchorX(nOutlets-1), y + h/2);
	g.drawLine(x_out, y + h/2 + 1, getOutletAnchorX(nOutlets-1), y + h/2 + 1);

	g.drawLine(x_out, y + 1, x_out, y + h - 2);
	g.drawLine(x_out + 1, y + 1, x_out + 1, y + h - 2);
	
	for(int i=1; i<nOutlets; i++)
	  {
	    x_out = getOutletAnchorX(i);
	    g.drawLine(x_out, y + h/2, x_out, y + h - 2);
	    g.drawLine(x_out + 1, y + h/2, x_out + 1, y + h - 2);
	  }
      }
  }
}







