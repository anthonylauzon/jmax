
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
    Graphics g = gc.getGraphicDestination().getGraphics();

    g.setColor(Color.gray);
    g.setXORMode(Color.white); 

    tempRect.setBounds(movingPoint.x, 0, dx, gc.getGraphicDestination().getSize().height);

    normalizeRectangle(tempRect);

    g.fillRect(tempRect.x, tempRect.y, tempRect.width, tempRect.height);

    g.setPaintMode();
    g.setColor(Color.black);
    g.dispose();
    movingPoint.setLocation(movingPoint.x+dx, movingPoint.y+dy); 
  }

  //--- Fields
  Rectangle tempRect = new Rectangle(); 

}

