//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;

/**
 * The piano-roll event renderer in a Score: the line-based event, 
 * with a lenght , a fixed width, black color, a label.
 */
public class PartitionEventRenderer implements ObjectRenderer {

  /**
   * constructor.
   */
  public PartitionEventRenderer(ExplodeGraphicContext theGc) 
  {
    gc = theGc;
  }


  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected) 
  {
    ScrEvent e = (ScrEvent) obj;

    int x = gc.getAdapter().getX(e);
    int y = gc.getAdapter().getY(e);
    int lenght = gc.getAdapter().getLenght(e);
    int label = gc.getAdapter().getLabel(e);

    if (selected) 
      {
	g.setColor(Color.red);
	if (ExplodeSelection.getCurrent().getModel() != gc.getDataModel()) g.drawRect(x, y, lenght, NOTE_DEFAULT_HEIGHT);
	else g.fillRect(x, y, lenght, NOTE_DEFAULT_HEIGHT);
      }
    else 
      {
	g.setColor(Color.black);
	g.fillRect(x, y, lenght, NOTE_DEFAULT_HEIGHT);
      }

    g.drawString(""+label, x, y-5);
  
  }
  
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y) 
  {
    ScrEvent e = (ScrEvent) obj;

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
  public boolean touches(Object obj, int x, int y, int w, int h) 
  {
    ScrEvent e = (ScrEvent) obj;

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
  ExplodeGraphicContext gc;

  int oldX, oldY;

}










