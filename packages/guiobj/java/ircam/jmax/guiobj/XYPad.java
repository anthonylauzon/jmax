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
import javax.swing.*;
import java.util.*;
import java.lang.Math;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "Pad" graphic object
//

public class XYPad extends GraphicObject 
{
  //
  // The Cursor contained into a 'Pad' object.
  //

  static final int DOT_DIAMETER = 3;
  protected final static int BOTTOM_OFFSET = 4;
  protected final static int UP_OFFSET = 2;
  protected final static int LEFT_OFFSET = 2;
  protected final static int RIGHT_OFFSET = 4;
  private static final int MINIMUM_DIMENSION = 15;
  private static final int DEFAULT_RANGE = 127;
  private static final int DEFAULT_HEIGHT = BOTTOM_OFFSET + UP_OFFSET + DEFAULT_RANGE;
  private static final int DEFAULT_WIDTH = LEFT_OFFSET + RIGHT_OFFSET + DEFAULT_RANGE;
  private static final Color SELECTED_TRANSPARENCY = new Color( 0, 0, 0, 48); 
  private static final Color NORMAL_TRANSPARENCY = new Color( 0, 0, 0, 16); 

  private int xValue;
  private int yValue;
  private int xRangeMax;
  private int xRangeMin;
  private int yRangeMax;
  private int yRangeMin;

  public static XYPadControlPanel controlPanel = new XYPadControlPanel();

  public XYPad(FtsGraphicObject theFtsObject)
  {
    super(theFtsObject);

    xRangeMin = ((FtsXYPadObject)ftsObject).getXMinValue();
    xRangeMax = ((FtsXYPadObject)ftsObject).getXMaxValue();
    yRangeMin = ((FtsXYPadObject)ftsObject).getYMinValue();
    yRangeMax = ((FtsXYPadObject)ftsObject).getYMaxValue();
    xValue = ((FtsXYPadObject)ftsObject).getXValue();
    yValue = ((FtsXYPadObject)ftsObject).getYValue();

    if(xValue<xRangeMin) 
      xValue = xRangeMin;
    else if(xValue>xRangeMax) 
      xValue = xRangeMax;
    
    if(yValue<yRangeMin) 
      yValue = yRangeMin;
    else if(yValue>yRangeMax) 
      yValue = yRangeMax;
  }

  public void setDefaults()
  {
    int w = LEFT_OFFSET + (xRangeMax - xRangeMin) + RIGHT_OFFSET;
    setWidth( w);

    int h = BOTTOM_OFFSET + (yRangeMax - yRangeMin) + UP_OFFSET;
    setHeight( h);
  }

  public void setCurrentXYValues(int x, int y)
  {
    xValue = x;   
    yValue = y;   
    redraw();
  }

  public void setXMinValue( int theValue) 
  {
    xRangeMin = theValue;
    ((FtsXYPadObject)ftsObject).setXMinValue(xRangeMin);
  }

 public void setYMinValue( int theValue) 
  {
    yRangeMin = theValue;
    ((FtsXYPadObject)ftsObject).setYMinValue(yRangeMin);
  }

  public void setCurrentXMinValue( int value)
  {
    xRangeMin = value;

    if( value < xRangeMin)
      value = xRangeMin;
  }

  public void setCurrentYMinValue( int value)
  {
    yRangeMin = value;

    if( value < yRangeMin)
      value = yRangeMin;
  }

  public int getXMinValue() 
  {
    return xRangeMin;
  }

  public int getYMinValue() 
  {
    return yRangeMin;
  }

  public void setXMaxValue( int theValue) 
  {
    xRangeMax = theValue;
    ((FtsXYPadObject)ftsObject).setXMaxValue(xRangeMax);
  }

  public void setYMaxValue( int theValue) 
  {
    yRangeMax = theValue;
    ((FtsXYPadObject)ftsObject).setYMaxValue(yRangeMax);
  }

  public void setCurrentXMaxValue( int value)
  {
    xRangeMax = value;
    
    if( value > xRangeMax) 
      value = xRangeMax;

    redraw();
  }
  
  public void setCurrentYMaxValue( int value)
  {
    yRangeMax = value;
    
    if( value > yRangeMax) 
      value = yRangeMax;

    redraw();
  }

  public int getXMaxValue() 
  {
    return xRangeMax;
  }

  public int getYMaxValue() 
  {
    return yRangeMax;
  }

  public void setWidth(int w)
  {
    if(w <= 0)
      w = DEFAULT_WIDTH;
    else if(w < MINIMUM_DIMENSION)
      w = MINIMUM_DIMENSION;

    super.setWidth(w);
  }

  public void setHeight(int h)
  {
    if(h <= 0)
      h = DEFAULT_RANGE;
    else if (h < MINIMUM_DIMENSION)
      h = MINIMUM_DIMENSION;

    super.setHeight(h);
  }

  public void setXRange(int theMaxInt, int theMinInt)
  {
    if(( theMaxInt == xRangeMax)&&( theMinInt == xRangeMin)) return;
    
    setXMaxValue( theMaxInt);
    setXMinValue( theMinInt);

    if(xValue < xRangeMin)
      xValue = xRangeMin;
    else if( xValue > xRangeMax) 
      xValue = xRangeMax;

    redraw();
  }

  public void setYRange(int theMaxInt, int theMinInt)
  {
    if(( theMaxInt == yRangeMax)&&( theMinInt == yRangeMin)) return;
    
    setYMaxValue( theMaxInt);
    setYMinValue( theMinInt);

    if(yValue < yRangeMin)
      yValue = yRangeMin;
    else if( yValue > yRangeMax) 
      yValue = yRangeMax;

    redraw();
  }

  public void redefined()
  {
    setDefaults();
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    int newXValue;
    int newYValue;
    int usableWidth = getWidth() - LEFT_OFFSET - RIGHT_OFFSET;
    int usableHeight = getHeight() - BOTTOM_OFFSET - UP_OFFSET;
    int xOffset = getX() + LEFT_OFFSET;
    int yOffset = getY() + BOTTOM_OFFSET;

    newXValue = ((((mouse.x - xOffset) * (xRangeMax - xRangeMin)) / usableWidth) + xRangeMin);
    newYValue = ((((mouse.y - yOffset) * (yRangeMin - yRangeMax)) / usableHeight) + yRangeMax);
    
    newXValue = (newXValue < xRangeMin) ? xRangeMin: ((newXValue >= xRangeMax) ? xRangeMax : newXValue);
    newYValue = (newYValue < yRangeMin) ? yRangeMin: ((newYValue >= yRangeMax) ? yRangeMax : newYValue);

    if ( (newXValue != xValue) || (newYValue != yValue)){
      ((FtsXYPadObject)ftsObject).setXYValues( newXValue, newYValue);
    }
    redraw();
  }

  public void paint( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int xRange = (xRangeMax - xRangeMin);
    int yRange = (yRangeMax - yRangeMin);
    int xP = ((xValue - xRangeMin) * (w - LEFT_OFFSET - RIGHT_OFFSET)) / xRange;
    int yP = ((yRangeMax - yValue) * (h - UP_OFFSET - BOTTOM_OFFSET)) / yRange;
    
    // Paint the box 
    if( isSelected()) 
      {
	g.setColor(SELECTED_TRANSPARENCY);
	g.fillRect( x+1, y+1, w-2,  h-2);
      }
    else 
      {
	g.setColor(NORMAL_TRANSPARENCY);
	g.fillRect( x+1, y+1, w-2,  h-2);
      }   
    
    // Paint the Cursor 
    if(itsSketchPad.isLocked())
      g.setColor(Color.lightGray);   
    else
      g.setColor(Color.white);
      
    //draw cursor marks
    g.drawLine( x + LEFT_OFFSET + xP,  y + UP_OFFSET, x + LEFT_OFFSET + xP, y + h - BOTTOM_OFFSET);
    g.drawLine( x + LEFT_OFFSET,  y + UP_OFFSET + yP, x + w - RIGHT_OFFSET ,  y + UP_OFFSET + yP);
  
    //draw cursor
    g.setColor( Color.black);
    g.drawRect(x + LEFT_OFFSET - 1 + xP, y + UP_OFFSET - 1 + yP, DOT_DIAMETER, DOT_DIAMETER); 

    super.paint(g);
  }
  
  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    int xRange = (xRangeMax - xRangeMin);
    int yRange = (yRangeMax - yRangeMin);
    int xP = ((xValue - xRangeMin) * (w - LEFT_OFFSET - RIGHT_OFFSET)) / xRange;
    int yP = ((yRangeMax - yValue) * (h - UP_OFFSET - BOTTOM_OFFSET)) / yRange;
    
      
    /* Paint the box */ 
    /*g.setColor( Settings.sharedInstance().getUIColor());
    g.fillRect( x+2, y+2, w-4,  h-4);
    */    
    // g.draw3DRect( x+1, y+1, w-2,  h-2, true);    

    /* Paint the Cursor */ 
    //g.setColor( Settings.sharedInstance().getUIColor().brighter());
    g.setColor(Color.lightGray);
    g.drawLine( x + LEFT_OFFSET + xP,  y + UP_OFFSET, x + LEFT_OFFSET + xP, y + h - BOTTOM_OFFSET);
    g.drawLine( x + LEFT_OFFSET,  y + UP_OFFSET + yP, x + w - RIGHT_OFFSET ,  y + UP_OFFSET + yP); 
    
    g.setColor( Color.black);
    g.drawRect(x + LEFT_OFFSET - 1 + xP, y + UP_OFFSET - 1 + yP, DOT_DIAMETER, DOT_DIAMETER); 
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

 public ObjectControlPanel getControlPanel()
 {
   return this.controlPanel;
 }

 public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    ObjectPopUp.addSeparation();    
    getControlPanel().update(this);
    ObjectPopUp.getInstance().add((JPanel)getControlPanel());
    ObjectPopUp.getInstance().revalidate();
    ObjectPopUp.getInstance().pack();    
  }
  public void popUpReset()
  {
    super.popUpReset();
    getControlPanel().done();
    ObjectPopUp.getInstance().remove((JPanel)getControlPanel());
    ObjectPopUp.removeSeparation();
  }

  void updateDimension()
  {
    int w = getWidth();
    int h = getHeight();
    setWidth(h);
    setHeight(w);
    itsSketchPad.repaint();//???
  }
}
