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

class ErmesObjSlider extends ErmesObject implements FtsIntValueListener
{
  //
  // The graphic throttle contained into a 'slider' object.
  //

  class Throttle
  {
    protected int itsX, itsY;
    protected int itsWidth, itsHeight;

    private Rectangle bounds = new Rectangle();

    private final int LATERAL_OFFSET = 2;
    private final int THROTTLE_HEIGHT = 5;
  
    private int itsPreviousX;
    private int itsPreviousY;

    private boolean erased = true;

    private final int XOR_MODE = 0;
    private final int PAINT_MODE = 1;
  
    public Throttle() 
    {
      itsX = getX() + LATERAL_OFFSET;
      itsY = getY() + getHeight() - BOTTOM_OFFSET - 2;
      itsPreviousX = itsX;
      itsPreviousY = itsY;

      itsWidth = getWidth() - 2*LATERAL_OFFSET;
      itsHeight = THROTTLE_HEIGHT;
    }

    protected Rectangle getBounds()
    {
      bounds.setBounds( itsX, itsY, itsWidth, itsHeight);
      return bounds;
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

    //coordinate conversion, clipped

    int SliderToDrag( int theSliderCoord) 
    {
      if ( theSliderCoord <= BOTTOM_OFFSET) 
	return 0;
      else if ( theSliderCoord >= itsPixelRange) 
	return itsPixelRange;
      else
	return ( theSliderCoord - BOTTOM_OFFSET);
    }

    //check if is in the "dragging" area

    boolean IsInDragArea( int absoluteY)
    { 
      return ( AbsoluteToSlider( absoluteY) > UP_OFFSET 
	       && AbsoluteToSlider( absoluteY) < getHeight()- BOTTOM_OFFSET);
    }

    //coordinate conversion, inverted and clipped

    int AbsoluteToDrag( int theAbsoluteY) 
    {
      int temp = AbsoluteToSlider( theAbsoluteY);
      return itsPixelRange - SliderToDrag( temp); // invert
    }

    //coordinate conversion, inverted and clipped  

    int DragToAbsolute( int theDragCoord) 
    {
      int normalizedDrag = theDragCoord;
      if ( theDragCoord <= 0) 
	normalizedDrag = 0;
      else if ( theDragCoord >= itsPixelRange) 
	normalizedDrag = itsPixelRange;

      return getY() + UP_OFFSET + (itsPixelRange - normalizedDrag);
    }
  
    void eraseAndPaint( Graphics g) 
    {
      if ( !erased)
	Paint( g, XOR_MODE);      

      Paint( g, PAINT_MODE);
    }

    void paintNoErase( Graphics g) 
    {
      Paint( g, PAINT_MODE);
    }

    private  void Paint( Graphics g, int mode) 
    {
      int deltaX = getX() + LATERAL_OFFSET;
      int deltaY;

      if ( mode == PAINT_MODE) 
	deltaY = getY() + AbsoluteToSlider( itsY);
      else
	deltaY = itsPreviousY;

      g.setColor( Settings.sharedInstance().getSelectedColor());

      if ( !isSelected()) 
	{
	  if ( mode == XOR_MODE) 
	    g.setXORMode( Settings.sharedInstance().getUIColor());

	  g.fillRect( deltaX + 1, deltaY + 1, itsWidth - 2, itsHeight - 2);
	}

      g.setColor( Color.black);
      if ( mode == XOR_MODE)
	g.setXORMode( isSelected() ? Settings.sharedInstance().getSelectedColor() : Settings.sharedInstance().getUIColor());
      
      g.drawRect( deltaX, deltaY, itsWidth - 1, itsHeight - 1);
      if ( mode == XOR_MODE) 
	{
	  g.setPaintMode();//reset mode to normal
	  erased = true;
	}
      else
	erased = false;
    }
  
    public void Resize( int theWidth, int theHeight)
    {
      itsWidth = theWidth;
      itsHeight = theHeight;
    }

    void MoveAbsolute( int theX, int theY) 
    {
      itsX = theX;
      itsY = theY; 
    }

    void Move( int theX, int theY)
    {
      storeOld();
      MoveAbsolute( theX, theY);
    }
  
    public void moveByAbsolute( int theDeltaH, int theDeltaV) 
    {
      itsX += theDeltaH;
      itsY+=theDeltaV;    
    }
  
    void moveBy( int theDeltaH, int theDeltaV) 
    {
      if ( theDeltaH != 0 || theDeltaV != 0) 
	{
	  storeOld();
	  moveByAbsolute( theDeltaH, theDeltaV);
	}
    }
	
    private void storeOld() 
    {
      itsPreviousX = itsX;
      itsPreviousY = itsY;
    }
  }

  private Throttle itsThrottle;
  private int itsInteger = 0;

  boolean itsMovingThrottle = false;

  private int itsRangeMax;
  private int itsRangeMin;

  int itsRange;
  int itsPixelRange;
  float itsStep;

  private static ErmesObjSliderDialog itsSliderDialog = null;

  protected final static int BOTTOM_OFFSET = 5;
  protected final static int UP_OFFSET = 5;

  ErmesObjSlider( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);

    itsRangeMin = ((FtsSliderObject)itsFtsObject).getMinValue();
    itsRangeMax = ((FtsSliderObject)itsFtsObject).getMaxValue();

    if (itsRangeMax == 0)
      {
	itsRangeMax = 127;
	((FtsSliderObject)itsFtsObject).setMaxValue(itsRangeMax);
      }

    itsRange = itsRangeMax - itsRangeMin;
    itsPixelRange = itsRangeMax - itsRangeMin;
    itsStep = ( float)itsRange/itsPixelRange;

    if (getWidth() < 20)
      setWidth( 20);

    int h = BOTTOM_OFFSET + itsRange + UP_OFFSET;
    if (getHeight() < h)
      setHeight( h);

    itsThrottle = new Throttle();
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

	if ( itsSketchPad.isLocked())
	  Paint_movedThrottle( g);
	else
	  Paint( g);

	g.dispose();
      }
  }

  public void inspect()
  {
    Point aPoint = itsSketchPad.getSketchWindow().getLocation();

    if ( itsSliderDialog == null) 
      itsSliderDialog = new ErmesObjSliderDialog();

    itsSliderDialog.setLocation( aPoint.x + getX(), aPoint.y + getY() - 25);
    itsSliderDialog.ReInit( String.valueOf( itsRangeMax), String.valueOf( itsRangeMin), String.valueOf( itsInteger), this, itsSketchPad.getSketchWindow());
  }

  public void sliderDown(Point mouse)
  {  
    System.err.println("ErmesObjSlider: gotMouseDown " + mouse);

    int x, y;

    x = mouse.x;
    y = mouse.y;

    if( IsInThrottle( x, y))
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

    redraw();
  }

  public void sliderDrag(Point mouse)
  {
    System.err.println("ErmesObjSlider: gotMouseDrag " + mouse);

    int x, y;

    x = mouse.x;
    y = mouse.y;

    if (itsMovingThrottle )
      {
	if( getY() + getHeight() - BOTTOM_OFFSET >= y && getY() + UP_OFFSET <=y )
	  {
	    //compute the value and send to FTS
	    if ( itsInteger == (int)( ((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep) )
	      return;
	
	    itsInteger = (int)(((getY() + getHeight()) - y - BOTTOM_OFFSET) * itsStep);

	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger + itsRangeMin);
	
	    itsThrottle.Move( itsThrottle.itsX, y - 2);
	  }
	else if( getY() + getHeight() - BOTTOM_OFFSET < y)
	  {
	    itsInteger = itsRangeMin;
	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger);

	    itsThrottle.Move( itsThrottle.itsX, getY() + getHeight() - BOTTOM_OFFSET - 2);
	  }
	else if( getY() + UP_OFFSET > y)
	  {
	    itsInteger = itsRangeMax;
	    ((FtsSliderObject)itsFtsObject).setValue(itsInteger);

	    itsThrottle.Move( itsThrottle.itsX, getY() + UP_OFFSET - 2);
	  }

	redraw();
      }
  }

  public void sliderUp(Point mouse)
  {
    if (itsMovingThrottle)
      {
	itsMovingThrottle = false;
	((FtsSliderObject)itsFtsObject).updateValue();
	Fts.sync();
      }
  }


  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case Squeack.DOWN:
	sliderDown(mouse);
	break;
      case Squeack.DRAG:
	sliderDrag(mouse);
	break;
      case Squeack.UP:
	sliderUp(mouse);
	break;
      }
  }
	

  public boolean IsInThrottle( int theX, int theY)
  {
    return itsThrottle.getBounds().contains( theX,theY);
  }
  
  public void Paint_movedThrottle( Graphics g) 
  {
    itsThrottle.eraseAndPaint( g);
  }

  public void Paint( Graphics g) 
  {
    if( !isSelected()) 
      g.setColor( Settings.sharedInstance().getUIColor());
    else
      g.setColor( Settings.sharedInstance().getSelectedColor());

    g.fill3DRect( getX()+1, getY()+1, getWidth()-2,  getHeight()-2, true);

    itsThrottle.paintNoErase( g);

    super.Paint( g);
  }

  public void moveBy( int theDeltaH, int theDeltaV) 
  {
    super.moveBy( theDeltaH, theDeltaV);
    itsThrottle.moveByAbsolute( theDeltaH, theDeltaV);
  }

  private static VResizeSensibilityArea vResizeArea = new VResizeSensibilityArea();

  public SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    if (mouseY >= y + h - VResizeSensibilityArea.height
	 && mouseX >= x + w / 2)
      {
	vResizeArea.setObject(this);
	return vResizeArea;
      }
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }
}
