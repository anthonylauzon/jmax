package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The "slider" graphic object
//

class ErmesObjSlider extends ErmesObject implements FtsPropertyHandler {

  private ErmesObjThrottle itsThrottle;
  private int itsInteger = 0;

  boolean itsMovingThrottle = false;

  private int itsRangeMax = PREFERRED_RANGE_MAX;
  private int itsRangeMin = PREFERRED_RANGE_MIN;

  int itsRange = itsRangeMax - itsRangeMin;
  int itsPixelRange = itsRangeMax - itsRangeMin;
  float itsStep =  itsRange/itsPixelRange;

  static ErmesObjSliderDialog itsSliderDialog = null;

  protected final static int BOTTOM_OFFSET = 5;
  protected final static int UP_OFFSET = 5;
  private final static int PREFERRED_RANGE_MAX = 127;
  private final static int PREFERRED_RANGE_MIN = 0;

  ErmesObjSlider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);

    itsFtsObject.watch( "value", this);

    Object value;

    value = itsFtsObject.get( "minValue");

    if (value instanceof Integer)
      itsRangeMin = ((Integer) value).intValue();
    else
      itsRangeMin = 0;

    value = itsFtsObject.get( "maxValue");

    if (value instanceof Integer)
      itsRangeMax = ((Integer) value).intValue();
    else
      itsRangeMax = 128;

    itsRange = itsRangeMax - itsRangeMin;
    itsStep = ( float)itsRange/itsPixelRange;

    if (getWidth() < 20)
      setWidth( 20);

    int h = BOTTOM_OFFSET + itsRange + UP_OFFSET;
    if (getHeight() < h)
      setHeight( h);

    recomputeInOutletsPositions();

    itsThrottle = new ErmesObjThrottle( this);
  }

  public void setMinValue( int theValue) 
  {
    itsRangeMin = theValue;
    itsFtsObject.put( "minValue", itsRangeMin);
  }

  public int getMinValue() 
  {
    return itsRangeMin;
  }

  public void setMaxValue( int theValue) 
  {
    itsRangeMax = theValue;
    itsFtsObject.put( "maxValue", itsRangeMax);
  }

  public int getMaxValue() 
  {
    return itsRangeMax;
  }

  public void resizeBy( int theDeltaH, int theDeltaV)
  {
    super.resizeBy( theDeltaH, theDeltaV);
    itsPixelRange = getHeight() - (UP_OFFSET + BOTTOM_OFFSET);
    
    itsStep = (float)itsRange / itsPixelRange;
    itsThrottle.Resize( itsThrottle.itsWidth + theDeltaH, itsThrottle.itsHeight);
    itsThrottle.MoveAbsolute( itsThrottle.itsX, (int)(getY() + getHeight() - BOTTOM_OFFSET - 2 - itsInteger/itsStep));
  }

  public void FromDialogValueChanged( int theCurrentInt, int theMaxInt, int theMinInt)
  {
    setMaxValue( theMaxInt);
    setMinValue( theMinInt);

    itsRange = itsRangeMax - itsRangeMin;
    itsStep = (float)itsRange / itsPixelRange;

    itsInteger = (theCurrentInt < itsRangeMin) ? itsRangeMin:( (theCurrentInt >= itsRangeMax) ? itsRangeMax:theCurrentInt);
    sendValue( new Integer( itsInteger));
  }

  public void propertyChanged( FtsObject obj, String name, Object value) 
  {
    if ( itsMovingThrottle) 
      return;

    int temp = ((Integer) value).intValue();
    
    if ( itsInteger != temp) 
      {
	itsInteger = temp;
	int clippedValue = ( temp < itsRangeMin) ? itsRangeMin: ((temp >= itsRangeMax) ? itsRangeMax : temp);
	clippedValue -= itsRangeMin;
      
	if ( itsThrottle != null) 
	  {
	    itsThrottle.Move( itsThrottle.itsX, (int) (getY() + getHeight() - BOTTOM_OFFSET - 2 -clippedValue/itsStep));
	  }

	Graphics g = itsSketchPad.getGraphics();

	if ( itsSketchPad.itsRunMode)
	  Paint_movedThrottle( g);
	else
	  Paint_specific( g);

	g.dispose();
      }
  }

  public void inspect()
  {
    Point aPoint = GetSketchWindow().getLocation();

    if ( itsSliderDialog == null) 
      itsSliderDialog = new ErmesObjSliderDialog();

    itsSliderDialog.setLocation( aPoint.x + getX(), aPoint.y + getY() - 25);
    itsSliderDialog.ReInit( String.valueOf( itsRangeMax), String.valueOf( itsRangeMin), String.valueOf( itsInteger), this, itsSketchPad.GetSketchWindow());
  }

  public void MouseDown_specific( MouseEvent evt, int x, int y)
  {
    if( itsSketchPad.itsRunMode || evt.isControlDown())
      {
	if( IsInThrottle( x,y))
	  {
	    itsMovingThrottle = true;
	    return;
	  }

	if ( getY() + getHeight() - BOTTOM_OFFSET >= y && getY() + UP_OFFSET < y) 
	  {
	    itsInteger = (int)((( getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep);

	    itsThrottle.Move( itsThrottle.itsX, y - 2);
	    itsMovingThrottle = true;
	  }
	else if( getY() + getHeight() - BOTTOM_OFFSET < y)
	  {
	    itsInteger = itsRangeMin;
	    itsThrottle.Move( itsThrottle.itsX, getY() + getHeight() - BOTTOM_OFFSET - 2);
	  }
	else if( getY() + UP_OFFSET >= y)
	  {
	    itsInteger = itsRangeMax;
	    itsThrottle.Move( itsThrottle.itsX, getY() + UP_OFFSET - 2);
	  }
	
	sendValue( new Integer( itsInteger));

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific( g);
	g.dispose();
      }
    else 
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  private void sendValue( Integer theValue) 
  {
    itsFtsObject.put( "value", theValue, this);
  }

  public boolean MouseDrag_specific( MouseEvent evt,int x, int y)
  {
    if( (itsSketchPad.itsRunMode || evt.isControlDown()) && itsMovingThrottle )
      {
	if( getY() + getHeight() - BOTTOM_OFFSET >= y && getY() + UP_OFFSET <=y )
	  {
	    //compute the value and send to FTS
	    if ( itsInteger == (int)( ((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep) )
	      return false;
	
	    itsInteger = (int)(((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep);

	    sendValue( new Integer( itsInteger + itsRangeMin));
	
	    itsThrottle.Move( itsThrottle.itsX, y - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getY() + getHeight() - BOTTOM_OFFSET < y)
	  {
	    sendValue( new Integer( itsRangeMin));
	    itsInteger = itsRangeMin;
	    itsThrottle.Move( itsThrottle.itsX, getY() + getHeight() - BOTTOM_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getY() + UP_OFFSET > y)
	  {
	    sendValue( new Integer( itsRangeMax));
	    itsInteger = itsRangeMax;
	    itsThrottle.Move( itsThrottle.itsX, getY() + UP_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	return true;
      }
    return false;
  }

  void MouseUp( MouseEvent evt,int x, int y)
  {
    if( itsSketchPad.itsRunMode || evt.isControlDown() || itsMovingThrottle)
      {
	itsMovingThrottle = false;
	itsFtsObject.put( "value", itsInteger+itsRangeMin);
	Fts.sync();
      }
    else 
      super.MouseUp( evt, x, y);
  }

  public boolean IsInThrottle( int theX, int theY)
  {
    return itsThrottle.getBounds().contains( theX,theY);
  }
  
  public boolean NeedPropertyHandler()
  {
    return true;
  }

  public boolean isUIController() 
  {
    return true;
  }

  public void Paint_movedThrottle( Graphics g) 
  {
    itsThrottle.eraseAndPaint( g);
  }

  public void Paint_specific( Graphics g) 
  {
    if ( g == null) 
      return; 

    if( !itsSelected) 
      g.setColor( itsUINormalColor);
    else
      g.setColor( itsUISelectedColor);

    g.fill3DRect( getX()+1, getY()+1, getWidth()-2,  getHeight()-2, true);

    g.setColor( Color.black);
    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
    
    if( !itsSketchPad.itsRunMode) 
      g.fillRect( getX()+getWidth()-DRAG_DIMENSION,getY()+getHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);

    itsThrottle.paintNoErase( g);
  }

  public void MoveBy( int theDeltaH, int theDeltaV) 
  {
    super.MoveBy( theDeltaH, theDeltaV);
    itsThrottle.MoveByAbsolute( theDeltaH, theDeltaV);
  }

  static Dimension minimumSize = new Dimension();

  public Dimension getMinimumSize() 
  {
    minimumSize.setSize( 15, 30);

    return minimumSize;
  }
}
