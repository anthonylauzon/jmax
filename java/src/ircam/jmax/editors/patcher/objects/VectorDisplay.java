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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The display graphic object.
//

class VectorDisplay extends GraphicObject implements FtsDisplayListener
{
  private static final int minWidth = 18;
  private static final int minHeight = 18;

  private int maxWidth = 0;
  private static final int maxHeight = 500;

  /* silent agreement with client */
  private static final int defaultWidth = 130;
  private static final int defaultHeight = 130;

  int size = 0;
  int range = 0;

  public VectorDisplay(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    int w = getWidth();
    int h = getHeight();

    if(w < 0 || h < 0)
      {
	w = defaultWidth;
	h = defaultHeight;
      }

    maxWidth = FtsVectorDisplayObject.MAX_SIZE + 2;

    setWidth(w);
    setHeight(h);
  }

  public void display()
  {
    redraw();
  }

  public void setWidth( int w) 
  {
    if (w < minWidth)
      w = minWidth;
    else if (w > maxWidth)
      w = maxWidth;

    size = w - 2;

    ((FtsVectorDisplayObject)ftsObject).setSize(size);
    
    super.setWidth(w);
  }

  public void setHeight( int h)
  {
    if (h < minHeight)
      h = minHeight;
    else if (h > maxHeight)
      h = maxHeight;

    range = h - 2;

    ((FtsVectorDisplayObject)ftsObject).setRange(range);

    super.setHeight(h);
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {          
  }

  protected SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    if ((mouseY >= getY() + getHeight() - ObjectGeometry.V_RESIZE_SENSIBLE_HEIGHT)
	&& (mouseX >= getX() + getWidth() / 2))
      {
	return SensibilityArea.get(this, Squeack.VRESIZE_HANDLE);
      }
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }

  private void paintVector(Graphics g, int x, int y, int[] values, int n)
  {
    int from = values[0];
    
    /* security for dynamic resize */
    if(from > range)
      from = range;
    
    int fromX = x;
    int fromY = y - from;
    
    for(int i=1; i<n; i++)
      {
	int to = values[i];
	
	/* security for dynamic resize */
	if(to > range)
	  to = range;
	
	int toX = x + i;
	int toY = y - to;
	
	g.drawLine(fromX, fromY, toX, toY);
	
	fromX = toX;
	fromY = toY;
      }
  }

  public void paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    int n = ((FtsVectorDisplayObject)ftsObject).getNValues();
    int[] values = ((FtsVectorDisplayObject)ftsObject).getValues();
    int zero = ((FtsVectorDisplayObject)ftsObject).getZero();

    int orgX = x + 1;
    int orgY = y + h - 2;
    Color backgroundColor = new Color((float)1.0, (float)0.98, (float)0.9);
    Color zeroColor = new Color((float)0.9, (float)0.88, (float)0.8);
    int i;

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);

    if (isSelected())
      g.setColor(backgroundColor.darker());
    else
      g.setColor(backgroundColor);

    g.fillRect( x, y, w - 1, h - 1);

    if(zero > 0)
      {
	if (isSelected())
	  g.setColor(zeroColor.darker());
	else
	  g.setColor(zeroColor);
	
	g.drawLine(orgX, orgY - zero, orgX + size, orgY - zero);
      }

    g.setColor(Color.black);
    g.drawRect( x, y, w - 1, h - 1);
    
    paintInlets(g);

    if(n > size)
      n = size;

    if(n > 1)
      paintVector(g, orgX, orgY, values, n);

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
  }

  public void updatePaint(Graphics g) 
  {
    int n = ((FtsVectorDisplayObject)ftsObject).getNValues();
    int[] values = ((FtsVectorDisplayObject)ftsObject).getValues();
    int i;

    if(n > size)
      n = size;

    if(n > 1)
      paintVector(g, getX() + 1, getY() + getHeight() - 2, values, n);
  }
}
