package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;

//
// The "toggle" graphic object.
//
class ErmesObjToggle extends ErmesObject implements FtsIntValueListener {

  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 15;

  private static final Color itsCrossColor = new Color(0, 0, 128);

  private boolean itsToggled = false;

  public ErmesObjToggle( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
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

  // redefined from base class

  public void resizeBy( int theDeltaW, int theDeltaH) 
  {
    setWidth( getWidth() + theDeltaW);
  }

  public void mouseDown( MouseEvent evt,int x, int y) 
  {
    itsToggled = !itsToggled;

    ((FtsIntValueObject)itsFtsObject).setValue(itsToggled ? 1 : 0);

    redraw();
  }

  public void valueChanged(int value) 
  {
    boolean temp = (value == 1);

    if ( itsToggled != temp) 
      {
	itsToggled = temp;

	Graphics g = itsSketchPad.getGraphics();
	Paint( g);
	g.dispose();
      }
  }

  public void Paint(Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( !isSelected())
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    g.fill3DRect( x + 1, y + 1, w - 2, h - 2, true);

    if (itsToggled) 
      {
	g.setColor( itsCrossColor);
	g.drawLine( x + 4, y + 4, x + w - 6, y + h - 6);
	g.drawLine( x + w - 6, y + 4, x + 4,y + h - 6);
      }

    super.Paint( g);
  }
}
