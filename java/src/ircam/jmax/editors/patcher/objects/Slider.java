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
import javax.swing.*;
import java.util.*;
import java.lang.Math;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "slider" graphic object
//

public class Slider extends GraphicObject implements FtsIntValueListener
{
  //
  // The graphic throttle contained into a 'slider' object.
  //

  static final int THROTTLE_LATERAL_OFFSET = /*2*/3;
  static final int THROTTLE_HEIGHT = 3;
  private static final int MINIMUM_DIMENSION = 15;
  protected final static int BOTTOM_OFFSET = 5;
  protected final static int UP_OFFSET = 5;
  
  static final int VERTICAL_OR = 0;
  static final int HORIZONTAL_OR = 1;
  
  private int value = 0;
  private int rangeMax;
  private int rangeMin;
  private int orientation;

  public Slider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);

    rangeMin = ((FtsSliderObject)ftsObject).getMinValue();
    rangeMax = ((FtsSliderObject)ftsObject).getMaxValue();

    if(value<rangeMin) 
      value = rangeMin;
    else if(value>rangeMax) 
      value = rangeMax;

    if(getWidth() <= 0)
      setWidth( 20);
    else if(getWidth() <= MINIMUM_DIMENSION)
      setWidth(MINIMUM_DIMENSION);

    /* Probabily usefull only for new object */

    int h = BOTTOM_OFFSET + (rangeMax - rangeMin) + UP_OFFSET;

    if (getHeight() < BOTTOM_OFFSET +  UP_OFFSET)
      setHeight( h);
    
    orientation = ((FtsSliderObject)ftsObject).getOrientation();

    if((orientation!=HORIZONTAL_OR)&&(orientation!=VERTICAL_OR))
      setOrientation(VERTICAL_OR);
  }

  public void setMinValue( int theValue) 
  {
    rangeMin = theValue;
    ((FtsSliderObject)ftsObject).setMinValue(rangeMin);
  }

  public int getMinValue() 
  {
    return rangeMin;
  }

  public void setMaxValue( int theValue) 
  {
    rangeMax = theValue;
    ((FtsSliderObject)ftsObject).setMaxValue(rangeMax);
  }

  public int getMaxValue() 
  {
    return rangeMax;
  }

  public void setWidth(int w)
  {
    if (w < MINIMUM_DIMENSION)
      //return;
      w=MINIMUM_DIMENSION;
    
    super.setWidth(w);
  }

  public void setHeight(int h)
  {
    if (h < MINIMUM_DIMENSION)
      //return;
      h=MINIMUM_DIMENSION;

    super.setHeight(h);
  }

  public void setRange(int theMaxInt, int theMinInt)
  {
    setMaxValue( theMaxInt);
    setMinValue( theMinInt);

    if(value < rangeMin)
      value = rangeMin;
    else if(value>rangeMax) 
      value = rangeMax;

    redraw();
  }

  public void setOrientation(int or)
  {
    orientation = or;
    ((FtsSliderObject)ftsObject).setOrientation(orientation);
  }

  public int getOrientation()
  {
    return orientation;
  }

  public void valueChanged(int v) 
  {
    value = ( v < rangeMin) ? rangeMin: ((v >= rangeMax) ? rangeMax : v);

    updateRedraw();
  }

  public void inspect()
  {
    Point aPoint = itsSketchPad.getEditorContainer().getContainerLocation();
    SliderDialog dialog = new SliderDialog(itsSketchPad.getEditorContainer().getFrame(), this);
    dialog.setBounds( aPoint.x + getX(), aPoint.y + getY() - 25, 200, 100);
    dialog.setVisible( true);
    dialog = null;//for the gc
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    int newValue;

    if(orientation==VERTICAL_OR)
      newValue = ((( getY() + getHeight() - mouse.y - BOTTOM_OFFSET) * (rangeMax - rangeMin)) /
		  (getHeight() - BOTTOM_OFFSET - UP_OFFSET-THROTTLE_HEIGHT)) + rangeMin;
    else
      newValue = (((mouse.x - (getX()+ UP_OFFSET)) * (rangeMax - rangeMin)) /
		  (getWidth() - BOTTOM_OFFSET - UP_OFFSET-THROTTLE_HEIGHT)) + rangeMin;
    
    if(newValue>rangeMax) newValue = rangeMax;
    else if(newValue<rangeMin) newValue = rangeMin;

    if(newValue!=value)
	((FtsSliderObject)ftsObject).setValue(newValue);
  }

  public void paint( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    
    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);

    /* Paint the box */

    if( !isSelected()) 
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getUIColor().darker());

    g.fill3DRect( x+1, y+1, w-2,  h-2, true);

    /* Paint the throttle */
    g.setColor( Color.black);  
    int pixels, pos;
    if(orientation==VERTICAL_OR)
    {
      pixels = h - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
      pos = y + BOTTOM_OFFSET + pixels  - (pixels * (value-rangeMin)) / (rangeMax - rangeMin);
      //pos = y + h - BOTTOM_OFFSET - (value-rangeMin) * (h - BOTTOM_OFFSET - UP_OFFSET)/(rangeMax - rangeMin);

      g.drawRect(x + THROTTLE_LATERAL_OFFSET, pos, w - 2*THROTTLE_LATERAL_OFFSET - 1, THROTTLE_HEIGHT - 1);
    }
    else
    {
      pixels = w - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
      pos = x + UP_OFFSET + (pixels * (value-rangeMin)) / (rangeMax - rangeMin);
      g.drawRect(pos, y + THROTTLE_LATERAL_OFFSET, THROTTLE_HEIGHT - 1, h - 2*THROTTLE_LATERAL_OFFSET - 1); 
    }
    super.paint(g);

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);

    /* Paint the box */
    g.setColor( Settings.sharedInstance().getUIColor());
    g.fillRect( x+2, y+2, w-4,  h-4);

    /* Paint the throttle */
    g.setColor( Color.black);
    int pixels, pos;
    if(orientation==VERTICAL_OR)
    {
      pixels = h - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
      pos = y + BOTTOM_OFFSET + pixels - (pixels * (value-rangeMin)) / (rangeMax - rangeMin);
      g.drawRect(x + THROTTLE_LATERAL_OFFSET, pos, w - 2*THROTTLE_LATERAL_OFFSET - 1, THROTTLE_HEIGHT - 1);
    }
    else
    {
      pixels = w - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
      pos = x + UP_OFFSET + (pixels * (value-rangeMin)) / (rangeMax - rangeMin);
      g.drawRect(pos, y + THROTTLE_LATERAL_OFFSET, THROTTLE_HEIGHT - 1, h - 2*THROTTLE_LATERAL_OFFSET - 1); 
    }

    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
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

 public void popUpUpdate(boolean onInlet, boolean onOutlet, SensibilityArea area)
  {
    super.popUpUpdate(onInlet, onOutlet, area);
    ObjectPopUp.addMenu(SliderPopUpMenu.getInstance());
  }
  public void popUpReset()
  {
    super.popUpReset();
    ObjectPopUp.removeMenu(SliderPopUpMenu.getInstance());
  }

  public void changeOrientation()
  {
    if(orientation == VERTICAL_OR) setOrientation(HORIZONTAL_OR);
    else setOrientation(VERTICAL_OR);
    
    updateDimension();
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
