package ircam.jmax.editors.ermes;

import java.awt.*;

/**
 * The toolbar's button. It handles the selected/unselected state,
 * and inform the toolbar when it is pressed.
 */
class ErmesToggleButton extends Canvas {
  boolean pressed;
  private int itsNum;
  Image itsImage1, itsImage2;
  ErmesToolBar itsToolBar;
  
	
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesToggleButton(Image theImage1, Image theImage2, ErmesToolBar theToolBar, int theNum) {
    itsNum = theNum;
    pressed = false;
    itsToolBar = theToolBar;
    itsImage1 = theImage1;
    itsImage2 = theImage2;
  }
  
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    Dimension d = size();
    g.setColor(Color.lightGray);
    
    g.draw3DRect(0, 0, d.width - 1, d.height - 1, true);
    g.draw3DRect(3, 3, d.width - 7, d.height - 7, false);
    
    if(!pressed) g.drawImage(itsImage1, 4, 4,this);
    else g.drawImage(itsImage2, 4, 4,this);
  }
	
  //--------------------------------------------------------
  //	mouseDown
  //--------------------------------------------------------
  public boolean mouseDown(Event evt,int x, int y) {
    
    if(evt.clickCount>1 && pressed) {//double click = lock
      pressed = false;	  
      itsToolBar.ButtonPressed(itsNum);
      repaint();
      itsToolBar.Lock();
      return true;
    }
    Toggle();
    if (pressed) {
      itsToolBar.ButtonPressed(itsNum);
    }
    else itsToolBar.ButtonDepressed(itsNum);
    return true;
  }
  
  //--------------------------------------------------------
  //	Toggle
  //--------------------------------------------------------
  public void Toggle() {
    pressed = !pressed;
    repaint();
  }
  
  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------
  public Dimension minimumSize() {
    return new Dimension(40,32);
  }

  //--------------------------------------------------------
  //	preferredSize
  //--------------------------------------------------------
  public Dimension preferredSize() {
    return minimumSize();
  }
}





