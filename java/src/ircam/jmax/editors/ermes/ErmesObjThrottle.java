package ircam.jmax.editors.ermes;

import java.awt.*;

/**
 * The graphic throttle contained into a 'slider' object.
 */
class ErmesObjThrottle{
  int itsX, itsY;
  ErmesObjSlider itsSlider;
  Dimension preferredSize = null;
  final int LATERAL_OFFSET = 2;
  final int THROTTLE_HEIGHT = /*6*/5;
  
  public ErmesObjThrottle(ErmesObjSlider theSlider, int x, int y) {
    itsSlider = theSlider;
    itsX = x+LATERAL_OFFSET;
    itsY = y+theSlider.currentRect.height-theSlider.BOTTOM_OFFSET-2;
    preferredSize = new Dimension(theSlider.currentRect.width-2*LATERAL_OFFSET,THROTTLE_HEIGHT);
  }

  
  int AbsoluteToSlider(int theAbsoluteY) { //coordinate conversion, not inverted and clipped
    if (theAbsoluteY >= itsSlider.itsY)
      if (theAbsoluteY <= itsSlider.itsY+itsSlider.currentRect.height) 
	return (theAbsoluteY - itsSlider.itsY);
      else return itsSlider.currentRect.height;
    else return 0;
  }

  int SliderToDrag(int theSliderCoord) {	//coordinate conversion, clipped
    if (theSliderCoord <= itsSlider.BOTTOM_OFFSET) return 0;
    else if (theSliderCoord >= itsSlider.itsPixelRange) return itsSlider.itsPixelRange;
    else return (theSliderCoord - itsSlider.BOTTOM_OFFSET);
  }
	
  boolean IsInDragArea(int absoluteY) { //check if is in the "dragging" area
    return (AbsoluteToSlider(absoluteY) > itsSlider.UP_OFFSET &&
	    AbsoluteToSlider(absoluteY) < itsSlider.currentRect.height- itsSlider.BOTTOM_OFFSET);
  }
	
  int AbsoluteToDrag(int theAbsoluteY) {	//coordinate conversion, inverted and clipped
    int temp = AbsoluteToSlider(theAbsoluteY);
    return itsSlider.itsPixelRange-SliderToDrag(temp); // invert
  }
  
  int DragToAbsolute(int theDragCoord) {	//coordinate conversion, inverted and clipped
    int normalizedDrag = theDragCoord;
    if (theDragCoord <= 0) normalizedDrag = 0;
    else if (theDragCoord >= itsSlider.itsPixelRange) normalizedDrag = itsSlider.itsPixelRange;
    return (itsSlider.itsY+itsSlider.UP_OFFSET+(itsSlider.itsPixelRange-normalizedDrag));
  }
  
  void Paint(Graphics g) {
    int deltaX = itsSlider.itsX + LATERAL_OFFSET;
    int deltaY = itsSlider.itsY + AbsoluteToSlider(itsY);
    g.setColor(itsSlider.itsUISelectedColor);
    g.fillRect(deltaX+1, deltaY+1, getPreferredSize().width-2, getPreferredSize().height);
    g.fill3DRect(deltaX+2, deltaY+2, getPreferredSize().width-4, getPreferredSize().height-2, true);
    g.setColor(Color.black);
    g.drawRect(deltaX+0, deltaY+0, getPreferredSize().width-1, getPreferredSize().height-1);
  }
  
	
  public Rectangle Bounds(){
    return new Rectangle(itsX, itsY, getPreferredSize().width, getPreferredSize().height);
    
  }
  
  public void Resize(int theWidth, int theHeight){
    preferredSize.width = theWidth;
    preferredSize.height = theHeight;
  }
		
  
  public void Move(int theX, int theY){
    itsX = theX; itsY = theY; 
  }
  
  //no more used
  //  public boolean MouseDrag(Event evt,int x, int y) {
  //  if(itsSlider.GetSketchPad().itsRunMode){
      //itsSlider.ThrottleMovedTo(itsY+y);
      
      //itsSlider.ThrottleMovedTo(AbsoluteToDrag(y));
  //    return true;
  //  }
  //  else return false;
  //}
  
  public void MoveBy(int theDeltaH, int theDeltaV) {
    itsX += theDeltaH;
    itsY+=theDeltaV;
  }
	
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return getPreferredSize();
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}




