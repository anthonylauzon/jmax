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

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
//import javax.swing.*;
import javax.swing.JPopupMenu;

import java.util.*;
import java.awt.image.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.jmax.toolkit.*;

public class VectorDisplay extends GraphicObject 
{
  private static final int minWidth = 18;
  private static final int minHeight = 18;

  private int maxWidth = 0;
  private static final int maxHeight = 500;

  /* silent agreement with client */
  static final int DEFAULT_WIDTH = 130;
  static final int DEFAULT_HEIGHT = 130;

  private static final Color displayYellow = new Color((float)1.0, (float)0.98, (float)0.9);
  private static final Color markerYellow = new Color((float)0.9, (float)0.88, (float)0.8);

  int size = 0;
  int range = 0;

  private BufferedImage buff;
  private Graphics2D buffG;

  public VectorDisplay(FtsGraphicObject theFtsObject)
  {
    super(theFtsObject);

    maxWidth = FtsVectorDisplayObject.MAX_SIZE + 2;

    updateOffScreenBuffer();
  }

  public void setDefaults()
  {    
    setWidth(  DEFAULT_WIDTH);
    setHeight( DEFAULT_HEIGHT);
  }

  void updateOffScreenBuffer()
  {
    int w = getWidth() - 2;
    if( w <= 0) w = DEFAULT_WIDTH - 2;
    
    int h = getHeight() - 2;
    if( h <= 0) h = DEFAULT_HEIGHT - 2;
    
    buff = new BufferedImage( w, h, BufferedImage.TYPE_INT_RGB);
    buffG = buff.createGraphics();
    buffG.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
  }

  public void display()
  {
    drawContent( buffG, 0, 0, getWidth()-2, getHeight()-2);
    updateRedraw();
  }

  public void setCurrentBounds(int x, int y, int w, int h)
  {
    super.setCurrentBounds( x, y, w, h);
    
    if(w > 2)
      {      
	size = w-2;
	((FtsVectorDisplayObject)ftsObject).setSize( size);
      }
    if(h > 2)
      {
	range = h-2;
	((FtsVectorDisplayObject)ftsObject).setRange( range);
      }
    
    if( w <= 0) w = DEFAULT_WIDTH;
    if( h <= 0) h = DEFAULT_HEIGHT;
   
    updateOffScreenBuffer();
  }

  public void setWidth(int w) 
  {
    if( w <= 0)
      w = DEFAULT_WIDTH;
    else if (w < minWidth)
      w = minWidth;
    else if (w > maxWidth)
      w = maxWidth;

    size = w - 2;

    ((FtsVectorDisplayObject)ftsObject).setSize(size);
    
    super.setWidth(w);

    updateOffScreenBuffer();
  }

  public void setHeight(int h)
  {
    if( h <= 0)
      h = DEFAULT_HEIGHT;
    else if (h < minHeight)
      h = minHeight;
    else if (h > maxHeight)
      h = maxHeight;

    range = h - 2;

    ((FtsVectorDisplayObject)ftsObject).setRange(range);

    super.setHeight(h);

    updateOffScreenBuffer();
  }

  public void redefined()
  {
    setDefaults();
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

  public Color getBackgroundColor()
  {
    if (isSelected())
      return displayYellow.darker();
    else
      return displayYellow;
  }

  public Color getLineColor()
  {
    return Color.black;
  }

  public void drawMarkers(Graphics g, int x, int y)
  {
    int zero = ((FtsVectorDisplayObject)ftsObject).getZero();

    if(zero > 0)
      {
	if (isSelected())
	  g.setColor(markerYellow.darker());
	else
	  g.setColor(markerYellow);
	
	g.drawLine(x, y - zero, x + size - 1, y - zero);
      }    
  }

  public void drawWrappedVector(Graphics g, int x, int y, int[] values, int n, int wrap)
  {
    int from;
    int fromX, fromY;
    int m, i, j;

    m = FtsVectorDisplayObject.MAX_SIZE - wrap;
    if(m > n)
      m = n;

    from = values[wrap];
    
    if(from > range)
      from = range;
    
    fromX = x;
    fromY = y - from;

    g.setColor(getLineColor());
    
    for(i=1, j=wrap+1; i<m; i++, j++)
      {
	int to = values[j];
	
	if(to > range)
	  to = range;
	
	int toX = x + i;
	int toY = y - to;
	
	g.drawLine(fromX, fromY, toX, toY);
	
	fromX = toX;
	fromY = toY;
      }

    for(j=0; i<n; i++, j++)
      {
	int to = values[j];
	
	if(to > range)
	  to = range;
	
	int toX = x + i;
	int toY = y - to;
	
	g.drawLine(fromX, fromY, toX, toY);
	
	fromX = toX;
	fromY = toY;
      }
  }

  public void drawVector(Graphics g, int x, int y, int[] values, int n)
  {
    int from = values[0];
    
    if(from > range)
      from = range;
    
    int fromX = x;
    int fromY = y - from;

    g.setColor(getLineColor());
    
    for(int i=1; i<n; i++)
      {
	int to = values[i];
	
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
    

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);

    drawContent( g, x+1, y+1, w-2, h-2);

    g.setColor(Color.black);
    g.drawRect( x, y, w - 1, h - 1);
    
    paintInlets(g);

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
  }

  void drawContent( Graphics g, int x, int y, int w, int h)
  {
    int n = ((FtsVectorDisplayObject)ftsObject).getNValues();
    int[] values = ((FtsVectorDisplayObject)ftsObject).getValues();
    
    // draw background 
    g.setColor(getBackgroundColor());
    g.fillRect( x, y, w, h);

    drawMarkers(g, x, y+h-2);

    if(n > size)
      n = size;
    
    if(n > 1)
      {
	int wrap = ((FtsVectorDisplayObject)ftsObject).getWrap();
	if(wrap > 0)
	  drawWrappedVector(g, x, y+h-2, values, n, wrap);
	else
	  drawVector(g, x, y+h-2, values, n);
    }
  }

  public void updatePaint(Graphics g) 
  {
    g.drawImage( buff, getX()+1, getY()+1, itsSketchPad);  
  }

  public JPopupMenu getRunModePopUpMenu()
  {
      VecDispRModePopUp.update(this);
      return VecDispRModePopUp.popup;
  }

  public ObjectControlPanel getControlPanel()
  {
    ObjectControlPanel panel = new VecDispControlPanel();
    panel.update( this);
    return panel;
  }

  public boolean isInspectable()
  {
    return true;
  }
}
