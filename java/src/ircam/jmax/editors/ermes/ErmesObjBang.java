//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

//
// The "bang" graphic object.
//

class ErmesObjBang extends ErmesObject implements FtsIntValueListener
{
  private boolean itsFlashing = false;
  private Color itsFlashColor = Color.yellow;
  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 15;
  private static final int CIRCLE_ORIGIN = 3;

  public ErmesObjBang( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    int width = getWidth();
    if (width == -1)
      setWidth( DEFAULT_WIDTH);
    else if (width <= MINIMUM_WIDTH)
      setWidth( MINIMUM_WIDTH);
  }

  // redefined from base class
  protected void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class
  protected void setHeight( int theHeight)
  {
  }

  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.isLocked() || evt.isControlDown() ) 
      {
	itsFtsObject.sendMessage( 0, "bang", null);
      } 
    else
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  static Color bangColors[] = 
  {
    Color.yellow,
    Color.blue,
    Color.cyan,
    Color.green,
    Color.magenta,
    Color.orange,
    Color.pink,
    Color.red,
    Color.white,
    Color.black
  };

  public void valueChanged(int value) 
  {
    int flash = value;

    if (flash <= 0)
      itsFlashing = false;
    else if (flash >= bangColors.length) 
      {
	itsFlashing = true;
	itsFlashColor = Color.yellow;
      } 
    else 
      {
	itsFlashing = true;
	itsFlashColor = bangColors[flash - 1];
      }

    Graphics g = itsSketchPad.getGraphics();
    Paint_update( g);
    g.dispose();
  }


  public boolean isUIController() 
  {
    return true;
  }

  private void Paint_update( Graphics g) 
  {
    if (itsFlashing)
      g.setColor( itsFlashColor);
    else if ( itsSelected)
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else
      g.setColor( Settings.sharedInstance().getUIColor());

    g.fillOval( getX() + CIRCLE_ORIGIN + 1,
		getY() + CIRCLE_ORIGIN + 1,
		getWidth() - 2*(CIRCLE_ORIGIN+1) - 1,
		getHeight() - 2*(CIRCLE_ORIGIN+1) - 1);
  }

  public void Paint_specific( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( itsSelected)
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else
      g.setColor( Settings.sharedInstance().getUIColor());

    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    Paint_update(g);

    g.setColor(Color.black);
    g.drawOval( x + CIRCLE_ORIGIN, 
		y + CIRCLE_ORIGIN, 
		w - 2*CIRCLE_ORIGIN - 1,
		h - 2*CIRCLE_ORIGIN - 1);

    super.Paint_specific( g);
  }
}
