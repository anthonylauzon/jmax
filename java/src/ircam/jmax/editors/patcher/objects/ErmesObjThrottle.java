package ircam.jmax.editors.patcher.objects;

import java.awt.*;

import ircam.jmax.editors.patcher.*;

//
// The graphic throttle contained into a 'slider' object.
//
class ErmesObjThrottle {
  protected int itsX, itsY;
  protected int itsWidth, itsHeight;

  private ErmesObjSlider itsSlider;

  private Rectangle bounds = new Rectangle();

  private final int LATERAL_OFFSET = 2;
  private final int THROTTLE_HEIGHT = 5;
  
  private int itsPreviousX;
  private int itsPreviousY;

  private boolean erased = true;

  private static final int XOR_MODE = 0;
  private static final int PAINT_MODE = 1;
  
  public ErmesObjThrottle( ErmesObjSlider theSlider) 
  {
    itsSlider = theSlider;
    itsX = itsSlider.getX() + LATERAL_OFFSET;
    itsY = itsSlider.getY() + theSlider.getHeight() - theSlider.BOTTOM_OFFSET - 2;
    itsPreviousX = itsX;
    itsPreviousY = itsY;

    itsWidth = theSlider.getWidth() - 2*LATERAL_OFFSET;
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
    if ( theAbsoluteY >= itsSlider.getY())
      if ( theAbsoluteY <= itsSlider.getY() + itsSlider.getHeight()) 
	return ( theAbsoluteY - itsSlider.getY());
      else 
	return itsSlider.getHeight();
    else
      return 0;
  }

  //coordinate conversion, clipped
  int SliderToDrag( int theSliderCoord) 
  {
    if ( theSliderCoord <= itsSlider.BOTTOM_OFFSET) 
      return 0;
    else if ( theSliderCoord >= itsSlider.itsPixelRange) 
      return itsSlider.itsPixelRange;
    else
      return ( theSliderCoord - itsSlider.BOTTOM_OFFSET);
  }

  //check if is in the "dragging" area
  boolean IsInDragArea( int absoluteY)
  { 
    return ( AbsoluteToSlider( absoluteY) > itsSlider.UP_OFFSET 
	     && AbsoluteToSlider( absoluteY) < itsSlider.getHeight()- itsSlider.BOTTOM_OFFSET);
  }

  //coordinate conversion, inverted and clipped
  int AbsoluteToDrag( int theAbsoluteY) 
  {
    int temp = AbsoluteToSlider( theAbsoluteY);
    return itsSlider.itsPixelRange - SliderToDrag( temp); // invert
  }

  //coordinate conversion, inverted and clipped  
  int DragToAbsolute( int theDragCoord) 
  {
    int normalizedDrag = theDragCoord;
    if ( theDragCoord <= 0) 
      normalizedDrag = 0;
    else if ( theDragCoord >= itsSlider.itsPixelRange) 
      normalizedDrag = itsSlider.itsPixelRange;

    return itsSlider.getY() + itsSlider.UP_OFFSET + (itsSlider.itsPixelRange - normalizedDrag);
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
    int deltaX = itsSlider.getX() + LATERAL_OFFSET;
    int deltaY;

    if ( mode == PAINT_MODE) 
      deltaY = itsSlider.getY() + AbsoluteToSlider( itsY);
    else
      deltaY = itsPreviousY;

    g.setColor( Settings.sharedInstance().getSelectedColor());

    if ( !itsSlider.isSelected()) 
      {
	if ( mode == XOR_MODE) 
	  g.setXORMode( Settings.sharedInstance().getUIColor());

	g.fillRect( deltaX + 1, deltaY + 1, itsWidth - 2, itsHeight - 2);
      }

    g.setColor( Color.black);
    if ( mode == XOR_MODE)
      g.setXORMode( itsSlider.isSelected() ? Settings.sharedInstance().getSelectedColor() : Settings.sharedInstance().getUIColor());

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
    itsX = theX; itsY = theY; 
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
