package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * The "musical editor" event renderer in a Score: the oval-based note, no lenght,
 * black color, no labels.
 */
public class PartitionEventRenderer implements EventRenderer {
  final static int OVAL_WIDTH = 5;
  final static int OVAL_HEIGHT = 5;
  AdapterProvider itsAdapterProvider;

  int oldX, oldY;

  public PartitionEventRenderer(AdapterProvider theAdapterProvider) {
    itsAdapterProvider = theAdapterProvider;
  }

  public void render(ScrEvent e, Graphics g) {
    int x = itsAdapterProvider.getAdapter().getX(e);
    int y = itsAdapterProvider.getAdapter().getY(e);
    System.err.println("---evt x:"+ x+"---evt y:"+y);
    g.fillOval(x, y, OVAL_WIDTH, OVAL_HEIGHT);
      
  }
  
}










