package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;

//
// The "bang" graphic object.
//

class ErmesObjBang extends ErmesObject implements FtsPropertyHandler {

  boolean itsFlashing = false;
  Color itsFlashColor = Color.yellow;

  public ErmesObjBang( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);
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


  public void Init() 
  {
    super.Init();
    itsFtsObject.watch( "value", this);
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

    g.fillOval( getItsX() + 5, getItsY() + 5, getItsWidth() - 10, getItsHeight() - 10);
  }

  public void Paint_specific( Graphics g) 
  {
    int x = getItsX();
    int y = getItsY();
    int w = getItsWidth();
    int h = getItsHeight();

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
  static Dimension preferredSize = new Dimension(20,20);

  static Dimension minimumSize = new Dimension(15, 15);

  public Dimension getMinimumSize() 
  {
    return minimumSize;
  }

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
