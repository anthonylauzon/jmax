//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import java.awt.*;

/** 
 * The "decoration layer" of the table editor (only the gray area for now) */
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

    //vertical black line at the end of the table area
    g.setColor(Color.black);
    g.drawLine(end, 0, end, tgc.getGraphicDestination().getSize().height );


    //unused vertical area
    g.setColor(Color.lightGray);
    g.fillRect(end+1, 0, tgc.getGraphicDestination().getSize().width-end, tgc.getGraphicDestination().getSize().height);
    //no unused horizontal area... for now?

  }


  //--- Fields
  TableGraphicContext tgc;
}
