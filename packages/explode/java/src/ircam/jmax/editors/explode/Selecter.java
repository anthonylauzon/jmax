package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;

/**
 * a rubber-banding interaction module.
 * It takes care of drawing a "selection rectangle" in a 
 * component, and communicate it to a SelectionListener
 */
public class Selecter extends InteractionModule {
  SelectionListener itsListener;

  boolean updated = true;

  Point startSelection = new Point();
  int oldX = 0;
  int oldY = 0;
  
  int startX;
  int startY;

  public Selecter(SelectionListener theListener, Component theGraphicObject) {
    super(theGraphicObject, theGraphicObject);
    itsListener = theListener;
  }
  
  public void mousePressed(MouseEvent e) {
    oldX = e.getX();  //vars used to XOR-paint the selection rectangle
    oldY = e.getY();
    startSelection.setLocation(oldX, oldY);

  } 

  public void mouseDragged(MouseEvent e) {
    drawXORSelection(e.getX(), e.getY());
  }

  public void mouseReleased(MouseEvent e) {
    int x = e.getX();
    int y = e.getY();
    updated = true;
    drawXORSelection(x, y);
    updated = false;
    itsListener.selectionChoosen(startX, startY, x, y);
  }


  /**
   * draws the rubber rectangle during a drag-selection.
   * This method is invoked during a selection
   */
  public void drawXORSelection(int x1, int y1) {
    
    Graphics g = itsGraphicDestination.getGraphics();
    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.
    
    if (!updated) { /*erase the shadow*/
      drawRectGiven2Points(g, startSelection.x, startSelection.y, oldX, oldY);
      updated = true;
    
    }
    
    drawRectGiven2Points(g, startSelection.x, startSelection.y, x1, y1);
    oldX = x1; oldY = y1;
    
    g.setPaintMode();
    g.setColor(Color.black);
    updated = false;
  
  } 

  /**
   * utility function. Gets rid of the "negative lenght" problems
   * in the selection rectangles.
   */
  private void drawRectGiven2Points(Graphics g, int x1, int y1, int x2, int y2) {
    int ox;
    int oy;
    int width;
    int height;
    
    /* sets the origin */
    if (x1 < x2) ox = x1;
    else ox = x2;
    
    if (y1 < y2) oy = y1;
    else oy = y2;

    /* sets width and height */
    width = x1-x2; if (width<0) width = -width;
    height = y1-y2; if (height<0) height = -height;

    g.drawRect(ox, oy, width, height);

  }
}




