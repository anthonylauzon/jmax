package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * The piano-roll event renderer in a Score: the line-based note, 
 * lenght proportional to duration, fixed width, black color, no labels.
 */
public class PartitionEventRenderer implements EventRenderer {

  /**
   * constructor.
   */
  public PartitionEventRenderer(GraphicContext theGc) 
  {
    gc = theGc;
  }


  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(ScrEvent e, Graphics g, boolean selected) 
  {
    int x = gc.getAdapter().getX(e);
    int y = gc.getAdapter().getY(e);
    int lenght = gc.getAdapter().getLenght(e);

    if (selected) g.setColor(Color.red);
    else g.setColor(Color.black);

    g.fillRect(x, y, lenght, NOTE_DEFAULT_HEIGHT);
    g.drawString(""+e.getPitch(), x, y-5);
  }
  
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(ScrEvent e, int x, int y) 
  {
    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);
    int evtlenght = gc.getAdapter().getLenght(e);

    return  (evtx<=x && (evtx+evtlenght >= x) && evty<=y && (evty+NOTE_DEFAULT_HEIGHT) >= y);
  }


  Rectangle eventRect = new Rectangle();
  Rectangle tempRect = new Rectangle();
  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(ScrEvent e, int x, int y, int w, int h) 
  {
    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);
    int evtlenght = gc.getAdapter().getLenght(e);

    tempRect.setBounds(x, y, w, h);
    eventRect.setBounds(evtx, evty, evtlenght, NOTE_DEFAULT_HEIGHT);
    return  tempRect.intersects(eventRect);
  }


  //------------Fields
  final static int NOTE_DEFAULT_WIDTH = 5;
  final static int NOTE_DEFAULT_HEIGHT = 3;
  GraphicContext gc;

  int oldX, oldY;

}










