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
import java.awt.event.*;
import javax.swing.*;

/**
 * A JPanel that is able to show the content of a TableDataModel.*/
public class TableDisplay extends JPanel
{

  /**
   * Constructor. */
  public TableDisplay()
  {
    super();
  }

  /**
   * Constructor with a table render */
  public TableDisplay(TableRenderer tr)
  {
    super();
    this.tr = tr;
  }

  /**
   * Specify which render to use for this TableDisplay */
  public void setRenderer(TableRenderer tr)
  {
    this.tr = tr;
  }

  /**
   * The paint routine just delegates to the RenderManager
   * the representation of its data.*/
  public void paint(Graphics g) 
  {
    Rectangle r = g.getClipBounds();
    tr.render(g, r); //et c'est tout	
  }
  
  /**
   * paints all the points in a given (closed) range */
  public void rangePaint(int index1, int index2)
  {
    Graphics g = getGraphics();

    for (int i = index1; i<= index2; i++)
      tr.renderPoint(g, i);
    
    g.dispose();
  }
  
    /**
     * paints a single point */
  public void singlePaint(int index)
  {
    Graphics g = getGraphics();

    tr.renderPoint(g, index);
    g.dispose();
  }
  
  //--- Fields
  TableRenderer tr;
}

