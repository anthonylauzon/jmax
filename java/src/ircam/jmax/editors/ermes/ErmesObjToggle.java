package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The "toggle" graphic object.
//
class ErmesObjToggle extends ErmesObject implements FtsPropertyHandler {

  private static final int DEFAULT_WIDTH = 20;
  private static final int MINIMUM_WIDTH = 15;

  private static final Color itsCrossColor = new Color(0, 0, 128);

  private boolean itsToggled = false;

  public ErmesObjToggle( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
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

  // redefined from base class
  void resizeBy( int theDeltaW, int theDeltaH) 
  {
    setWidth( getWidth() + theDeltaW);

    recomputeInOutletsPositions();
  }

  public void MouseDown_specific( MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	itsToggled = !itsToggled;
	itsFtsObject.put( "value", (itsToggled ? 1 : 0) );

	DoublePaint();
      } 
    else
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void propertyChanged( FtsObject obj, String name, Object value) 
  {
    boolean temp = (((Integer)value).intValue() == 1);

    if ( itsToggled != temp) 
      {
	itsToggled = temp;
	Paint_specific( itsSketchPad.getGraphics());
      }
  }

  public boolean isUIController() 
  {
    return true;
  }

  public void Paint_specific(Graphics g) 
  {
    if (g == null)
      return;

    if ( !itsSelected)
      g.setColor( itsUINormalColor);
    else
      g.setColor(itsUISelectedColor);

    g.fillRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2);
    g.fill3DRect( getX()+2, getY()+2, getWidth()-4, getHeight()-4, true);

    g.setColor( Color.black);
    g.drawRect( getX()+0, getY()+ 0, getWidth()-1, getHeight()-1);

    if (itsToggled) 
      {
	g.setColor(itsCrossColor);
	g.drawLine( getX()+4, getY()+4, getX()+getWidth()-6, getY()+ getHeight()-6);
	g.drawLine( getX()+getWidth()-6, getY()+4, getX()+ 4,getY()+ getHeight()-6);
      }

    g.setColor(Color.black);
    if ( !itsSketchPad.itsRunMode)
      g.fillRect( getX()+getWidth()-DRAG_DIMENSION, getY()+getHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }


  // ----------------------------------------
  // old stuff
  // ----------------------------------------
  public Dimension getMinimumSize() 
  {
    new Throwable( this.getClass().getName()).printStackTrace();
    return new Dimension(  400, 400);
  }
}
