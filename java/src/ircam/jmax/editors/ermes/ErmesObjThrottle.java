package ircam.jmax.editors.ermes;

import java.awt.*;

//
// The graphic throttle contained into a 'slider' object.
//
class ErmesObjThrottle {
  int itsX, itsY;
  ErmesObjSlider itsSlider;
  Dimension preferredSize = null;
  Rectangle bounds = new Rectangle();

  final int LATERAL_OFFSET = 2;
  final int THROTTLE_HEIGHT = 5;
  
  int itsPreviousX;
  int itsPreviousY;

  boolean erased=true;

  static final int XOR_MODE = 0;
  static final int PAINT_MODE = 1;
  
  public ErmesObjThrottle( ErmesObjSlider theSlider, int x, int y) 
  {
    itsSlider = theSlider;
    itsX = x + LATERAL_OFFSET;
    itsY = y + theSlider.getItsHeight() - theSlider.BOTTOM_OFFSET - 2;
    itsPreviousX = itsX;
    itsPreviousY = itsY;
    
    preferredSize = new Dimension( theSlider.getItsWidth() - 2*LATERAL_OFFSET, THROTTLE_HEIGHT);
  }

  //coordinate conversion, not inverted and clipped  
  int AbsoluteToSlider( int theAbsoluteY) 
  {
    if ( theAbsoluteY >= itsSlider.getItsY())
      if ( theAbsoluteY <= itsSlider.getItsY() + itsSlider.getItsHeight()) 
	return ( theAbsoluteY - itsSlider.getItsY());
      else 
	return itsSlider.getItsHeight();
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
	     && AbsoluteToSlider( absoluteY) < itsSlider.getItsHeight()- itsSlider.BOTTOM_OFFSET);
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

    return itsSlider.getItsY() + itsSlider.UP_OFFSET + (itsSlider.itsPixelRange - normalizedDrag);
  }
  
  void eraseAndPaint( Graphics g) 
  {
    if ( !erased)
      Paint_specific( g, XOR_MODE);      
    Paint_specific( g, PAINT_MODE);
  }

  void paintNoErase( Graphics g) 
  {
    Paint_specific( g, PAINT_MODE);
  }

  void Paint_specific( Graphics g, int mode) 
  {
    int deltaX = itsSlider.getItsX() + LATERAL_OFFSET;
    int deltaY;

    if ( mode == PAINT_MODE) 
      deltaY = itsSlider.getItsY() + AbsoluteToSlider( itsY);
    else
      deltaY = itsPreviousY;

    g.setColor( itsSlider.itsUISelectedColor);

    if ( !itsSlider.itsSelected) 
      {
	if ( mode == XOR_MODE) 
	  g.setXORMode( itsSlider.itsUINormalColor);

	g.fillRect( deltaX+1, deltaY+1, getPreferredSize().width-2, getPreferredSize().height-2);
      }

    g.setColor( Color.black);
    if ( mode == XOR_MODE)
      g.setXORMode( itsSlider.itsSelected?itsSlider.itsUISelectedColor:itsSlider.itsUINormalColor);
    g.drawRect( deltaX, deltaY, getPreferredSize().width-1, getPreferredSize().height-1);
    if ( mode == XOR_MODE) 
      {
	g.setPaintMode();//reset mode to normal
	erased = true;
      }
    else
      erased = false;
  }
  
  public Rectangle Bounds() 
  {
    bounds.setBounds( itsX, itsY, getPreferredSize().width, getPreferredSize().height);
    return bounds;
  }

  public void Resize( int theWidth, int theHeight)
  {
    preferredSize.width = theWidth;
    preferredSize.height = theHeight;
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
  
  public void MoveByAbsolute( int theDeltaH, int theDeltaV) 
  {
    itsX += theDeltaH;
    itsY+=theDeltaV;    
  }
  
  void MoveBy( int theDeltaH, int theDeltaV) 
  {
    if ( theDeltaH != 0 || theDeltaV != 0) 
      {
	storeOld();
	MoveByAbsolute( theDeltaH, theDeltaV);
      }
  }
	
  private void storeOld() 
  {
    itsPreviousX = itsX;
    itsPreviousY = itsY;
  }

  public Dimension getMinimumSize() 
  {
    return getPreferredSize();
  }
  
  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
