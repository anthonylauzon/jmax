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

  ErmesObjThrottle itsThrottle;
  int itsInteger = 0;

  boolean itsMovingThrottle = false;
  int itsDelta = 0;

  private int itsRangeMax = PREFERRED_RANGE_MAX;
  private int itsRangeMin = PREFERRED_RANGE_MIN;

  int itsRange = itsRangeMax - itsRangeMin;
  int itsPixelRange = itsRangeMax - itsRangeMin;
  float itsStep =  itsRange/itsPixelRange;

  static ErmesObjSliderDialog itsSliderDialog = null;

  final static int BOTTOM_OFFSET = 5;
  final static int UP_OFFSET = 5;
  final static int PREFERRED_RANGE_MAX = 127;
  final static int PREFERRED_RANGE_MIN = 0;

  public ErmesObjSlider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);
  }

  public void Init()
  {
    Object value;

    super.Init();

    itsFtsObject.watch("value", this);
    makeCurrentRect(itsFtsObject);
    itsThrottle = new ErmesObjThrottle(this, getItsX(), getItsY());   

    value = itsFtsObject.get("minValue");

    if (value instanceof Integer)
      itsRangeMin = ((Integer) value).intValue();

    value = itsFtsObject.get("maxValue");

    if (value instanceof Integer)
      itsRangeMax = ((Integer) value).intValue();

    itsRange = itsRangeMax - itsRangeMin;
    itsStep = ( float)itsRange / itsPixelRange;

    value = itsFtsObject.get("value");

    if (value instanceof Integer)
      itsInteger = ((Integer) value).intValue();

    resizeBy( 0,0);
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
    itsPixelRange = getItsHeight() - (UP_OFFSET + BOTTOM_OFFSET);
    
    itsStep = (float)itsRange / itsPixelRange;
    itsThrottle.Resize( itsThrottle.getPreferredSize().width + theDeltaH, itsThrottle.getPreferredSize().height);
    itsThrottle.MoveAbsolute( itsThrottle.itsX, (int)(getItsY() + getItsHeight() - BOTTOM_OFFSET - 2 - itsInteger/itsStep));
  }

  public void FromDialogValueChanged( int theCurrentInt, int theMaxInt, int theMinInt)
  {
    setMaxValue( theMaxInt);
    setMinValue( theMinInt);
    itsRange = itsRangeMax - itsRangeMin;
    itsStep = (float)itsRange / itsPixelRange;

    int temp = theCurrentInt;        
    int clippedValue = ( temp < itsRangeMin) ? itsRangeMin:( (temp >= itsRangeMax) ? itsRangeMax:temp);
    itsInteger = clippedValue;
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
	    itsThrottle.Move( itsThrottle.itsX, (int) (getItsY() + getItsHeight() - BOTTOM_OFFSET - 2 -clippedValue/itsStep));
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

    itsSliderDialog.setLocation( aPoint.x + getItsX(), aPoint.y + getItsY() - 25);
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

	if ( getItsY() + getItsHeight() - BOTTOM_OFFSET >= y && getItsY() + UP_OFFSET < y) 
	  {
	    //compute the value and send to FTS
	    itsInteger = (int)((( getItsY() + getItsHeight()) - y - BOTTOM_OFFSET) * itsStep);
	    sendValue( new Integer( itsInteger));

	    itsThrottle.Move( itsThrottle.itsX, y - 2);
	    itsMovingThrottle = true;

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getItsY() + getItsHeight() - BOTTOM_OFFSET < y)
	  {
	    itsInteger = itsRangeMin;
	    sendValue( new Integer( itsRangeMin));
	    itsThrottle.Move( itsThrottle.itsX, getItsY() + getItsHeight() - BOTTOM_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getItsY() + UP_OFFSET >= y)
	  {
	    sendValue( new Integer( itsRangeMax));
	    itsInteger = itsRangeMax;
	    itsThrottle.Move( itsThrottle.itsX, getItsY() + UP_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
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
	if( getItsY() + getItsHeight() - BOTTOM_OFFSET >= y && getItsY() + UP_OFFSET <=y )
	  {
	    //compute the value and send to FTS
	    if ( itsInteger == (int)( ((getItsY() + getItsHeight()) - y - BOTTOM_OFFSET) * itsStep) )
	      return false;
	
	    itsInteger = (int)(((getItsY() + getItsHeight()) - y - BOTTOM_OFFSET) * itsStep);

	    sendValue( new Integer( itsInteger + itsRangeMin));
	
	    itsThrottle.Move( itsThrottle.itsX, y - 2);

	    Graphics g = itsSketchPad.getGraphics();

	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getItsY() + getItsHeight() - BOTTOM_OFFSET < y)
	  {
	    sendValue( new Integer( itsRangeMin));
	    itsInteger = itsRangeMin;
	    itsThrottle.Move( itsThrottle.itsX, getItsY() + getItsHeight() - BOTTOM_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getItsY() + UP_OFFSET > y)
	  {
	    sendValue( new Integer( itsRangeMax));
	    itsInteger = itsRangeMax;
	    itsThrottle.Move( itsThrottle.itsX, getItsY() + UP_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	return true;
      }
    return false;
  }

  public boolean MouseUp( MouseEvent evt,int x, int y)
  {
    if( itsSketchPad.itsRunMode || evt.isControlDown() || itsMovingThrottle)
      {
	itsMovingThrottle = false;
	itsFtsObject.put( "value", itsInteger+itsRangeMin);
	Fts.sync();

	return true;
      }
    else 
      return super.MouseUp( evt, x, y);
  }

  public boolean IsInThrottle( int theX, int theY)
  {
    Rectangle aRect = itsThrottle.Bounds();
    return aRect.contains( theX,theY);
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

    g.fill3DRect( getItsX()+1, getItsY()+1, getItsWidth()-2,  getItsHeight()-2, true);

    g.setColor( Color.black);
    g.drawRect( getItsX(), getItsY(), getItsWidth()-1, getItsHeight()-1);
    
    if( !itsSketchPad.itsRunMode) 
      g.fillRect( getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);

    itsThrottle.paintNoErase( g);
  }

  public void MoveBy( int theDeltaH, int theDeltaV) 
  {
    super.MoveBy( theDeltaH, theDeltaV);
    itsThrottle.MoveByAbsolute( theDeltaH, theDeltaV);
  }

  static Dimension currentMinimumSize = new Dimension( 20, BOTTOM_OFFSET + PREFERRED_RANGE_MAX + UP_OFFSET);

  public Dimension getMinimumSize() 
  {
    currentMinimumSize.width =15;
    currentMinimumSize.height = 30;
    return currentMinimumSize;
  }
  
  static Dimension preferredSize = new Dimension( 20, BOTTOM_OFFSET + PREFERRED_RANGE_MAX + UP_OFFSET);

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
