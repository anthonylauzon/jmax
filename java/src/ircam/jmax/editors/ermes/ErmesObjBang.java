package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.frobber.*;

//
// The "bang" graphic object.
//

class ErmesObjBang extends ErmesObject implements FtsPropertyHandler {

  private boolean itsFlashing = false;
  private Color itsFlashColor = Color.yellow;
  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 10;

  public ErmesObjBang( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    itsFtsObject.watch( "value", this);

    int width = getWidth();
    if (width == -1)
      setWidth( DEFAULT_WIDTH);
    else if (width <= MINIMUM_WIDTH)
      setWidth( width);
  }

  // redefined from base class
  protected void setWidth( int theWidth)
  {
    if (theWidth < MINIMUM_WIDTH)
      theWidth = MINIMUM_WIDTH;

    super.setWidth( theWidth);
    super.setHeight( theWidth);

    recomputeInOutletsPositions();
  }

  // redefined from base class
  protected void setHeight( int theHeight)
  {
    setWidth( theHeight);
  }

  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown() ) 
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

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    int flash = ((Integer) value).intValue();

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
      g.setColor( itsUISelectedColor);
    else
      g.setColor( itsUINormalColor);

    g.fillOval( getX() + 5, getY() + 5, getWidth() - 10, getHeight() - 10);
  }

  public void Paint_specific( Graphics g) 
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if ( itsSelected)
      g.setColor( itsUISelectedColor);
    else
      g.setColor( itsUINormalColor);

    g.fillRect( x + 1, y + 1, w - 2, h - 2);
    g.fill3DRect( x + 2, y + 2, w - 4, h - 4, true);

    Paint_update(g);

    g.setColor(Color.black);
    g.drawRect( x, y, w - 1, h - 1);
    g.drawOval( x + 4, y + 4, w - 8, h - 8);

    if ( !itsSketchPad.itsRunMode)
      g.fillRect( x + w - DRAG_DIMENSION, y + h - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }


  //--------------------------------------------------------
  // minimum and preferred sizes
  //--------------------------------------------------------
  public Dimension getMinimumSize() 
  {
    return null;
  }
}
