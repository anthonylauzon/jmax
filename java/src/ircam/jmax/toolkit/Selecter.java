package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

/**
 * A rubber-banding interaction module.
 * It takes care of drawing a "selection rectangle" in a 
 * component, and communicate the result to a GraphicSelectionListener
 */
public class Selecter extends InteractionModule implements XORPainter {

  /**
   * Constructor. This class uses a XORHandler to draw the shape
   * of the rubber banding rectangle
   */
  public Selecter(GraphicSelectionListener theListener) 
  {
    super();
    
    itsXORHandler = new XORHandler(this);
    itsListener = theListener;
  }
  

  /**
   * overrides InteractionModule.mousePressed()
   */
  public void mousePressed(MouseEvent e) 
  {
    int x = e.getX();
    int y = e.getY();

    interactionBeginAt(x, y);
    itsListener.selectionPointChoosen(x, y, e.getModifiers());
  } 


  /**
   * used to set the starting point of the interaction.
   */
  public void interactionBeginAt(int x, int y) 
  {  
    startSelection.setLocation(x, y);
    movingPoint.setLocation(x, y);
    itsXORHandler.beginAt(x, y);
    active = true;
  }

  /**
   * overrides InteractionModule.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    if (!active) return;//!!
    itsXORHandler.moveTo(e.getX(), e.getY());
    //    gc.getStatusBar().post(ScrToolbar.getTool(), ""+
    //			   (gc.getAdapter().getInvX(e.getX()))+
    //			   ", "+
    //			   (gc.getAdapter().getInvY(e.getY())));
  }

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    if (!active) return; //abnormal condition
    int x = e.getX();
    int y = e.getY();

    itsXORHandler.end();

    tempRect.setBounds(startSelection.x, startSelection.y, x-startSelection.x, y-startSelection.y);
    normalizeRectangle(tempRect);
    
    if (tempRect.width == 0) tempRect.width =1;
    if (tempRect.height == 0) tempRect.height =1;
    
    itsListener.selectionChoosen(tempRect.x, tempRect.y, tempRect.width, tempRect.height);
    active = false;
  }


  protected void unBindFromProducer() 
  {
    super.unBindFromProducer();
    active = false;
  }


  /**
   * from the XORPainter interface
   */
  public void XORErase() 
  {
    XORDraw(0, 0);
  }

  /**
   * from the XOR painter interface. The actual drawing routine
   */
  public void XORDraw(int dx, int dy) 
  {
    Graphics g = gc.getGraphicDestination().getGraphics();

    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy);

    tempRect.setBounds(startSelection.x, startSelection.y, movingPoint.x-startSelection.x, movingPoint.y-startSelection.y);
    normalizeRectangle(tempRect);

    g.drawRect(tempRect.x, tempRect.y, tempRect.width, tempRect.height);

    g.setPaintMode();
    g.setColor(Color.black);
    g.dispose();
  }


  /**
   * utility function. Gets rid of the "negative width/lenght" problems
   * in rectangles.
   */
  public void normalizeRectangle(Rectangle r) 
  {  
    /* sets the origin */
    if (r.width < 0) r.x = r.x+r.width;
    
    if (r.height < 0) r.y = r.y+r.height;

    /* sets width and height */
    if (r.width<0) r.width = -r.width;
    if (r.height<0) r.height = -r.height;
  }

  //--- Fields
  GraphicSelectionListener itsListener;

  Point startSelection = new Point();
  Point movingPoint = new Point();
  XORHandler itsXORHandler;

  boolean active = false;
  Rectangle tempRect = new Rectangle();

}




