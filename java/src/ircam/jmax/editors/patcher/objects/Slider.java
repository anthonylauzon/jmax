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
import java.lang.Math;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "slider" graphic object
//

class Slider extends GraphicObject implements FtsIntValueListener
{
  //
  // The graphic throttle contained into a 'slider' object.
  //

  static final int THROTTLE_LATERAL_OFFSET = 2;
  static final int THROTTLE_HEIGHT = 5;
  protected final static int BOTTOM_OFFSET = 5;
  protected final static int UP_OFFSET = 5;

  private int value = 0;
  private int rangeMax;
  private int rangeMin;

  private static SliderDialog inspector = null;

  Slider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);

    rangeMin = ((FtsSliderObject)ftsObject).getMinValue();
    rangeMax = ((FtsSliderObject)ftsObject).getMaxValue();

    if (rangeMax == 0)
      {
	rangeMax = 127;
	((FtsSliderObject)ftsObject).setMaxValue(rangeMax);
      }

    if (getWidth() < 20)
      setWidth( 20);

    /* Probabily usefull only for new object */

    int h = BOTTOM_OFFSET + (rangeMax - rangeMin) + UP_OFFSET;

    if (getHeight() < BOTTOM_OFFSET +  UP_OFFSET)
      setHeight( h);
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
    if (w < 10)
      return;

    super.setWidth(w);
  }

  public void setHeight(int h)
  {
    if (h < 10)
      return;

    super.setHeight(h);
  }

  public void fromDialogValueChanged( int theCurrentInt, int theMaxInt, int theMinInt)
  {
    setMaxValue( theMaxInt);
    setMinValue( theMinInt);

    ((FtsSliderObject)ftsObject).setValue((theCurrentInt < rangeMin) ?
					  rangeMin:
					  ((theCurrentInt >= rangeMax) ? rangeMax:theCurrentInt)
					  );
  }

  public void valueChanged(int v) 
  {
    value = ( v < rangeMin) ? rangeMin: ((v >= rangeMax) ? rangeMax : v);

    updateRedraw();
  }

  public void inspect()
  {
    //Point aPoint = itsSketchPad.getSketchWindow().getLocation();
    Point aPoint = itsSketchPad.getEditorContainer().getContainerLocation();

    if ( inspector == null) 
      inspector = new SliderDialog();

    inspector.setLocation( aPoint.x + getX(), aPoint.y + getY() - 25);

    inspector.ReInit( String.valueOf( rangeMax),
		      String.valueOf( rangeMin),
		      String.valueOf( value),
		      this,
		      //itsSketchPad.getSketchWindow()
		      itsSketchPad.getEditorContainer().getFrame());
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    int newValue;

    if ( getY() + getHeight() - BOTTOM_OFFSET >= mouse.y && getY() + UP_OFFSET < mouse.y) 
      newValue = (((( getY() + getHeight()) - mouse.y - BOTTOM_OFFSET) * (rangeMax - rangeMin)) /
		  (getHeight() - BOTTOM_OFFSET - UP_OFFSET));

    else if( getY() + getHeight() - BOTTOM_OFFSET < mouse.y)
      newValue = rangeMin;
    else 
      newValue = rangeMax;

    ((FtsSliderObject)ftsObject).setValue(newValue);
  }

  public void paint( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
    
    /* Paint the box */

    if( !isSelected()) 
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    g.fill3DRect( x+1, y+1, w-2,  h-2, true);

    /* Paint the throttle */

    int pixels = h - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
    int pos = y + BOTTOM_OFFSET + pixels - (pixels * value) / (rangeMax - rangeMin);

    g.setColor( Settings.sharedInstance().getSelectedColor());

    if (! isSelected()) 
      g.fillRect( x + THROTTLE_LATERAL_OFFSET + 1, pos + 1,
		  w - 2*THROTTLE_LATERAL_OFFSET - 2, THROTTLE_HEIGHT - 2);

    g.setColor( Color.black);

    g.drawRect(x + THROTTLE_LATERAL_OFFSET, pos, w - 2*THROTTLE_LATERAL_OFFSET - 1, THROTTLE_HEIGHT - 1);

    super.paint(g);
  }

  public void updatePaint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    //g.setClip(x+2, y+2, w-4, h-4);

    /* Paint the box */
    g.setColor( Settings.sharedInstance().getUIColor());
    g.fillRect( x+2, y+2, w-4,  h-4);

    /* Paint the throttle */
    int pixels = h - BOTTOM_OFFSET - UP_OFFSET - THROTTLE_HEIGHT;
    int pos = y + BOTTOM_OFFSET + pixels - (pixels * value) / (rangeMax - rangeMin);

    /*g.setColor( Settings.sharedInstance().getSelectedColor());
      g.fillRect( x + THROTTLE_LATERAL_OFFSET + 1, pos + 1,
      w - 2*THROTTLE_LATERAL_OFFSET - 2, THROTTLE_HEIGHT - 2);*/

    g.setColor( Color.black);
    g.drawRect(x + THROTTLE_LATERAL_OFFSET, pos, w - 2*THROTTLE_LATERAL_OFFSET - 1, THROTTLE_HEIGHT - 1);
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
}

