package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

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
  public void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);
  }

  // redefined from base class
  public void setHeight( int theHeight)
  {
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      itsFtsObject.sendMessage( 0, "bang", null);
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


  private void Paint_update( Graphics g) 
  {
    if (itsFlashing)
      g.setColor( itsFlashColor);
    else if ( isSelected())
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else
      g.setColor( Settings.sharedInstance().getUIColor());

    g.fillOval( getX() + CIRCLE_ORIGIN + 1,
		getY() + CIRCLE_ORIGIN + 1,
		getWidth() - 2*(CIRCLE_ORIGIN+1) - 1,
		getHeight() - 2*(CIRCLE_ORIGIN+1) - 1);
  }

  public void paint( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( isSelected())
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

    super.paint( g);
  }
}
