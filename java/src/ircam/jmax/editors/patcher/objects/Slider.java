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

  class Throttle
  {
    protected int itsX, itsY;
    protected int itsWidth, itsHeight;

    public Throttle() 
    {
      itsX = getX() + THROTTLE_LATERAL_OFFSET;
      itsY = getY() + getHeight() - BOTTOM_OFFSET - 2;

      itsWidth = getWidth() - 2*THROTTLE_LATERAL_OFFSET;
      itsHeight = THROTTLE_HEIGHT;
    }

    protected boolean contains(int x, int y)
    {
      return ((x >= itsX) && (x <= itsX + itsWidth) &&
	      (y >= itsY) && (y <= itsY + itsHeight));
    }

    //coordinate conversion, not inverted and clipped  

    int AbsoluteToSlider( int theAbsoluteY) 
    {
      if ( theAbsoluteY >= getY())
	if ( theAbsoluteY <= getY() + getHeight()) 
	  return ( theAbsoluteY - getY());
	else 
	  return getHeight();
      else
	return 0;
    }


    private  void paint(Graphics g) 
    {
      int deltaX = getX() + THROTTLE_LATERAL_OFFSET;
      int deltaY;

      deltaY = getY() + AbsoluteToSlider( itsY);
      g.setColor( Settings.sharedInstance().getSelectedColor());

      if ( !isSelected()) 
	g.fillRect( deltaX + 1, deltaY + 1, itsWidth - 2, itsHeight - 2);

      g.setColor( Color.black);
      g.drawRect( deltaX, deltaY, itsWidth - 1, itsHeight - 1);
    }
  
    void setWidth(int w)
    {
      itsWidth = w;
    }

    void move( int theX, int theY) 
    {
      itsX = theX;
      itsY = theY; 
    }
  }

  private Throttle itsThrottle;
  private int value = 0;

  private int itsRangeMax;
  private int itsRangeMin;

  int itsRange;
  int itsPixelRange;
  float itsStep;

  private static SliderDialog itsSliderDialog = null;

  protected final static int BOTTOM_OFFSET = 5;
  protected final static int UP_OFFSET = 5;

  Slider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);

    itsThrottle = new Throttle();

    itsRangeMin = ((FtsSliderObject)ftsObject).getMinValue();
    itsRangeMax = ((FtsSliderObject)ftsObject).getMaxValue();

    if (itsRangeMax == 0)
      {
	itsRangeMax = 127;
	((FtsSliderObject)ftsObject).setMaxValue(itsRangeMax);
      }

    itsRange = itsRangeMax - itsRangeMin;
    itsPixelRange = itsRangeMax - itsRangeMin;
    itsStep = ( float)itsRange/itsPixelRange;

    if (getWidth() < 20)
      setWidth( 20);

    int h = BOTTOM_OFFSET + itsRange + UP_OFFSET;

    if (getHeight() < h)
      setHeight( h);
  }

  public void setMinValue( int theValue) 
  {
    itsRangeMin = theValue;
    ((FtsSliderObject)ftsObject).setMinValue(itsRangeMin);
  }

  public int getMinValue() 
  {
    return itsRangeMin;
  }

  public void setMaxValue( int theValue) 
  {
    itsRangeMax = theValue;
    ((FtsSliderObject)ftsObject).setMaxValue(itsRangeMax);
  }

  public int getMaxValue() 
  {
    return itsRangeMax;
  }

  private void updateThrottle()
  {
    itsPixelRange = getHeight() - (UP_OFFSET + BOTTOM_OFFSET);
    
    itsStep = (float)itsRange / itsPixelRange;

    itsThrottle.setWidth( getWidth() - 2 * THROTTLE_LATERAL_OFFSET);

    itsThrottle.move( itsThrottle.itsX,
			      (int)(getY() + getHeight() - BOTTOM_OFFSET - 2 - value/itsStep));
  }

  public void setWidth(int w)
  {
    if (w < 10)
      return;

    super.setWidth(w);
    updateThrottle();
  }

  public void setHeight(int h)
  {
    if (h < 10)
      return;

    super.setHeight(h);
    updateThrottle();
  }

  public void fromDialogValueChanged( int theCurrentInt, int theMaxInt, int theMinInt)
  {
    setMaxValue( theMaxInt);
    setMinValue( theMinInt);

    itsRange = itsRangeMax - itsRangeMin;
    itsStep = (float)itsRange / itsPixelRange;

    value = (theCurrentInt < itsRangeMin) ? itsRangeMin:( (theCurrentInt >= itsRangeMax) ? itsRangeMax:theCurrentInt);
    ((FtsSliderObject)ftsObject).setValue(value);
  }

  public void valueChanged(int v) 
  {
    value = v;
    int clippedValue = ( value < itsRangeMin) ? itsRangeMin: ((value >= itsRangeMax) ? itsRangeMax : value);
    clippedValue -= itsRangeMin;
      
    itsThrottle.move( itsThrottle.itsX, (int) (getY() + getHeight() - BOTTOM_OFFSET - 2 -clippedValue/itsStep));
    updateRedraw();
  }

  public void inspect()
  {
    Point aPoint = itsSketchPad.getSketchWindow().getLocation();

    if ( itsSliderDialog == null) 
      itsSliderDialog = new SliderDialog();

    itsSliderDialog.setLocation( aPoint.x + getX(), aPoint.y + getY() - 25);
    itsSliderDialog.ReInit( String.valueOf( itsRangeMax), String.valueOf( itsRangeMin), String.valueOf( value), this, itsSketchPad.getSketchWindow());
  }

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    int newValue;

    if ( getY() + getHeight() - BOTTOM_OFFSET >= mouse.y && getY() + UP_OFFSET < mouse.y) 
      newValue = (int)((( getY() + getHeight()) - mouse.y - BOTTOM_OFFSET) * itsStep);
    else if( getY() + getHeight() - BOTTOM_OFFSET < mouse.y)
      newValue = itsRangeMin;
    else 
      newValue = itsRangeMax;

    ((FtsSliderObject)ftsObject).setValue(newValue);
  }

  public void paint( Graphics g) 
  {
    if( !isSelected()) 
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    g.fill3DRect( getX()+1, getY()+1, getWidth()-2,  getHeight()-2, true);

    itsThrottle.paint(g);

    super.paint(g);
  }

  public void moveBy( int theDeltaH, int theDeltaV) 
  {
    super.moveBy( theDeltaH, theDeltaV);
    updateThrottle();
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

