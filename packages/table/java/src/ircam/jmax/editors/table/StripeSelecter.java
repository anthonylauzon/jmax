
package ircam.jmax.editors.table;

import java.awt.*;

import ircam.jmax.toolkit.*;


/**
 * A specific shape of selection for the Table: the vertical stripe.
 * This class uses the functionalities of the Selecter IM just
 * redefining the XORDraw method (that is, the way the selection is shown.
 * @see Selecter
 */ 
public class StripeSelecter extends Selecter {

  /**
   * Constructor. */
  public StripeSelecter(GraphicSelectionListener l)
  {
   super(l); 
  }

  /**
   * overrides Selecter.XORDraw. The actual drawing routine
   */
  public void XORDraw(int dx, int dy) 
  {

    tempRect.setBounds(movingPoint.x, 0, dx, gc.getGraphicDestination().getSize().height);
    normalizeRectangle(tempRect);

    Graphics g = gc.getGraphicDestination().getGraphics();
    drawGrayRect(g, tempRect);
    g.dispose();

    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy); 
  }

  /**
   * draws the selection */
  public static void drawGrayRect(Graphics g, Rectangle r)
  {

    g.setColor(Color.gray);
    g.setXORMode(Color.white); 
    g.fillRect(r.x, r.y, r.width, r.height);

    g.setPaintMode();
    g.setColor(Color.black);

  }

  //--- Fields
  Rectangle tempRect = new Rectangle(); 

}

