
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import java.awt.*;

/** 
 * The "decoration layer" of the table editor (the zero line and the gray) */
public class TopLayer extends AbstractLayer {

  /**
   * Constructor */
  public TopLayer(TableGraphicContext theTgc)
  {
    tgc = theTgc;
  }

  /**
   * Layer interface. */
  public void render(Graphics g, int order)
  {
    TableAdapter ta = tgc.getAdapter();
    int middleY = ta.getY(0);
    int end = ta.getX(tgc.getDataModel().getSize());

    //box around the table
    g.drawRect(ta.getX(0), 0, end, tgc.getGraphicDestination().getSize().height );

    //horizontal zero line
    g.setColor(Color.red);
    g.drawLine(0, middleY, end, middleY);


    //unused vertical area
    g.setColor(Color.lightGray);
    g.fillRect(end+1, 0, tgc.getGraphicDestination().getSize().width-end, tgc.getGraphicDestination().getSize().height);
    //no unused horizontal area... for now?

  }


  //--- Fields
  TableGraphicContext tgc;
}
