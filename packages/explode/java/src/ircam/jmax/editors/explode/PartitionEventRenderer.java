package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * The "musical editor" event renderer in a Score: the oval-based note, no lenght,
 * black color, no labels.
 */
public class PartitionEventRenderer implements EventRenderer {
  final static int NOTE_DEFAULT_WIDTH = 5;
  final static int NOTE_DEFAULT_HEIGHT = 3;
  AdapterProvider itsAdapterProvider;

  int oldX, oldY;

  public PartitionEventRenderer(AdapterProvider theAdapterProvider) {
    itsAdapterProvider = theAdapterProvider;
  }

  public void render(ScrEvent e, Graphics g, boolean selected) {

    int x = itsAdapterProvider.getAdapter().getX(e);
    int y = itsAdapterProvider.getAdapter().getY(e);
    int lenght = itsAdapterProvider.getAdapter().getLenght(e);

    if (selected) g.setColor(Color.red);

    g.fillRect(x, y, lenght, NOTE_DEFAULT_HEIGHT);
    g.drawString(""+e.getPitch(), x, y-5);

    if (selected) g.setColor(Color.black);
  }
  
}










