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

class ErmesObjSlider extends ErmesObject implements FtsIntValueListener
{
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

    itsRangeMin = ((FtsSliderObject)itsFtsObject).getMinValue();
    itsRangeMax = ((FtsSliderObject)itsFtsObject).getMaxValue();

    if (itsRangeMax == 0)
      {
	itsRangeMax = 128;
	((FtsSliderObject)itsFtsObject).setMaxValue(itsRangeMax);
      }

    itsRange = itsRangeMax - itsRangeMin;
    itsStep = ( float)itsRange/itsPixelRange;

    if (getWidth() < 20)
      setWidth( 20);

    int h = BOTTOM_OFFSET + itsRange + UP_OFFSET;
    if (getHeight() < h)
      setHeight( h);

    itsThrottle = new ErmesObjThrottle( this);
  }

  public void setMinValue( int theValue) 
  {
    itsRangeMin = theValue;
    ((FtsSliderObject)itsFtsObject).setMinValue(itsRangeMin);
  }

  public int getMinValue() 
  {
    return itsRangeMin;
  }

  public void setMaxValue( int theValue) 
  {
    itsRangeMax = theValue;
    ((FtsSliderObject)itsFtsObject).setMaxValue(itsRangeMax);
  }

  public int getMaxValue() 
  {
    return itsRangeMax;
  }

  public void resizeBy( int theDeltaH, int theDeltaV)
  {
    // Added minimum size for the slider, otherwise
    // you can resize and loose it :-< ..
    // Maurizio & Enzo

    if ((-theDeltaH > (getWidth() - 10))  ||
	(-theDeltaV > (getHeight() - 20)))
      return;

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
    ((FtsSliderObject)itsFtsObject).setValue(itsInteger);
  }

  public void valueChanged( int value) 
  {
    if ( itsMovingThrottle) 
      return;

    if ( itsInteger != value) 
      {
	itsInteger = value;
	int clippedValue = ( value < itsRangeMin) ? itsRangeMin: ((value >= itsRangeMax) ? itsRangeMax : value);
	clippedValue -= itsRangeMin;
      
	if ( itsThrottle != null) 
	  {
	    itsThrottle.Move( itsThrottle.itsX, (int) (getY() + getHeight() - BOTTOM_OFFSET - 2 -clippedValue/itsStep));
	  }

	Graphics g = itsSketchPad.getGraphics();

	if (itsSketchPad.itsMode == ErmesSketchPad.LOCKMODE)
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
    if (itsSketchPad.itsMode == ErmesSketchPad.LOCKMODE || evt.isControlDown())
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

	((FtsSliderObject)itsFtsObject).setValue(itsInteger);	

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific( g);
	g.dispose();
      }
    else 
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void MouseDrag_specific( MouseEvent evt,int x, int y)
  {
    if( (itsSketchPad.itsMode == ErmesSketchPad.LOCKMODE || evt.isControlDown()) && itsMovingThrottle )
      {
	if( getY() + getHeight() - BOTTOM_OFFSET >= y && getY() + UP_OFFSET <=y )
	  {
	    //compute the value and send to FTS
	    if ( itsInteger == (int)( ((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep) )
	      return;
	
	    itsInteger = (int)(((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep);

	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger + itsRangeMin);
	
	    itsThrottle.Move( itsThrottle.itsX, y - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getY() + getHeight() - BOTTOM_OFFSET < y)
	  {
	    itsInteger = itsRangeMin;
	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger);

	    itsThrottle.Move( itsThrottle.itsX, getY() + getHeight() - BOTTOM_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
	else if( getY() + UP_OFFSET > y)
	  {
	    itsInteger = itsRangeMax;
	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger);

	    itsThrottle.Move( itsThrottle.itsX, getY() + UP_OFFSET - 2);

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific( g);
	    g.dispose();
	  }
      }
  }

  void MouseUp( MouseEvent evt,int x, int y)
  {
    if (itsSketchPad.itsMode == ErmesSketchPad.LOCKMODE || evt.isControlDown() || itsMovingThrottle)
      {
	itsMovingThrottle = false;
	((FtsSliderObject)itsFtsObject).updateValue();
	Fts.sync();
      }
    else 
      super.MouseUp( evt, x, y);
  }

  public boolean IsInThrottle( int theX, int theY)
  {
    return itsThrottle.getBounds().contains( theX,theY);
  }
  
//   public boolean NeedPropertyHandler()
//   {
//     return true;
//   }

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
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    g.fill3DRect( getX()+1, getY()+1, getWidth()-2,  getHeight()-2, true);

    itsThrottle.paintNoErase( g);

    super.Paint_specific( g);
  }

  public void MoveBy( int theDeltaH, int theDeltaV) 
  {
    super.MoveBy( theDeltaH, theDeltaV);
    itsThrottle.MoveByAbsolute( theDeltaH, theDeltaV);
  }

  private static VResizeSensibilityArea vResizeArea = new VResizeSensibilityArea();

  SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if (mouseY >= y + h - VResizeSensibilityArea.height
	 && mouseX >= x + w / 2)
      return vResizeArea;
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }
}
