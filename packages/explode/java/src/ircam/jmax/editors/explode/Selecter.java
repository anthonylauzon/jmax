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
  

  public Selecter(SelectionListener theListener, Component theGraphicObject) {
    super(theGraphicObject, theGraphicObject);
    itsListener = theListener;
  }
  
  public void mousePressed(MouseEvent e) {
    interactionBeginAt(e.getX(), e.getY());
  } 

  public void interactionBeginAt(int x, int y) {
    oldX = x;  //vars used to XOR-paint the selection rectangle
    oldY = y;
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

    tempRect.setBounds(startSelection.x, startSelection.y, x-startSelection.x, y-startSelection.y);
    normalizeRectangle(tempRect);
    
    itsListener.selectionChoosen(tempRect.x, tempRect.y, tempRect.width, tempRect.height);
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

  Rectangle tempRect = new Rectangle();
  /**
   * utility function. Gets rid of the "negative lenght" problems
   * in the selection rectangles.
   */
  private void drawRectGiven2Points(Graphics g, int x1, int y1, int x2, int y2) {
    tempRect.setBounds(x1, y1, x2-x1, y2-y1);
    normalizeRectangle(tempRect);

    g.drawRect(tempRect.x, tempRect.y, tempRect.width, tempRect.height);

  }

  /**
   * utility function. Gets rid of the "negative width/lenght" problems
   * in rectangles.
   */
  public void normalizeRectangle(Rectangle r) {
    
    /* sets the origin */
    if (r.width < 0) r.x = r.x+r.width;
    
    if (r.height < 0) r.y = r.y+r.height;

    /* sets width and height */
    if (r.width<0) r.width = -r.width;
    if (r.height<0) r.height = -r.height;

  }
}




