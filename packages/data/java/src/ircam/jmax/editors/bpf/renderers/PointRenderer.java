//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.bpf.renderers;

import ircam.jmax.editors.bpf.*;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.text.*;

/**
 * An IntegerValue event renderer. It is represented as a vertical black bar
 */
public class PointRenderer implements ObjectRenderer {

  public PointRenderer()
    {
    } 

  /**
   * constructor.
   */
  public PointRenderer(BpfGraphicContext theGc) 
  {
    gc = theGc;
  }

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected)
    {
	render(obj, g, selected, gc);
    } 

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
    public void render(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
    {
	BpfPoint point = (BpfPoint) obj;
	BpfGraphicContext bgc = (BpfGraphicContext) theGc;
	BpfAdapter adapter = bgc.getAdapter();
	BpfDataModel model = bgc.getDataModel();

	int x = adapter.getX(point);
	int y = adapter.getY(point);    
      
	if(selected)
	    g.setColor(Color.red);
	else
	    g.setColor(Color.black);
	
	g.fillOval(x-2, y-2, 5, 5);
	if(selected)
	    g.drawOval(x-4, y-4, 9, 9);
		

	BpfPoint prev, next;
	if(point instanceof UtilBpfPoint)//during the XOR draw
	    {
		BpfPoint original = ((UtilBpfPoint)point).getOriginal();
		next = model.getNextPoint(original);				
		prev = model.getPreviousPoint(original);
		
		if((prev != null)&&(!bgc.getSelection().isInSelection(prev)))
		    g.drawLine(adapter.getX(prev), adapter.getY(prev), x, y);

		if(next != null)
		    if (!bgc.getSelection().isInSelection(next))
			g.drawLine(x, y, adapter.getX(next), adapter.getY(next));
		    else
			{//qui anche il next e' selezionato
			    //qui deve vedere se il next e' < della sua posizione originale e clipparlo
			    //deve fare la stessa cosa con se stesso rispetto al prev
			    
			    int nextX = adapter.getX(next)+((UtilBpfPoint)point).getDeltaX(adapter);
			    //if(nextX < adapter.getX(original)) nextX = adapter.getX(original)+1;
			    
			    //si fa dare il next del next 
			    /*BpfPoint nextnext = model.getNextPoint(next);
			      if(nextnext!=null)
			      if(nextX > adapter.getX(nextnext)) nextX = adapter.getX(nextnext)-1;*/		 
			    
			    g.drawLine(x, y, nextX, adapter.getY(next)+((UtilBpfPoint)point).getDeltaY(adapter));
			}

		//draw the cross and the current values
		if(bgc.getSelection().size()==1)
		    {
			g.drawLine(x, y-10, x, y+10);
			g.drawLine(x-10, y, x+10, y);
			bgc.display(numberFormat.format(point.getTime())+" - "+
				    numberFormat.format(point.getValue()));
		    }
	    }
	else //normal paint
	    {
		next = model.getNextPoint(point); 
		      
		if(selected)
		    g.setColor(Color.black);
		
		if(next != null)
		    g.drawLine(x, y, adapter.getX(next), adapter.getY(next)); 
	    }
  }
  
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y) 
    {
	return contains(obj, x, y, gc);
    }

  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y, GraphicContext theGc) 
  {
    BpfPoint e = (BpfPoint) obj;
    BpfAdapter adapter = ((BpfGraphicContext) theGc).getAdapter();

    int evtx = adapter.getX(e);
    int evty = adapter.getY(e);    

    return ((evtx-POINT_RADIUS <= x) && (evtx+POINT_RADIUS >= x) &&
	    (evty-POINT_RADIUS <= y ) && (evty+POINT_RADIUS >= y));
  }

  Rectangle eventRect = new Rectangle();
  Rectangle tempRect = new Rectangle();

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h)
    {
	return touches(obj, x, y, w, h, gc);
    } 

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h, GraphicContext theGc) 
  {
    BpfPoint e = (BpfPoint) obj;
    BpfAdapter adapter = ((BpfGraphicContext) theGc).getAdapter();

    int evtx = adapter.getX(e);
    int evty = adapter.getY(e);    
    
    eventRect.setBounds(evtx-POINT_RADIUS, evty-POINT_RADIUS, 2*POINT_RADIUS+1, 2*POINT_RADIUS+1);

    tempRect.setBounds(x, y, w, h);

    return  eventRect.intersects(tempRect);

  }

    public static PointRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new PointRenderer();

	return staticInstance;
    }

  //------------Fields
    final static int Integer_HEIGHT = 12;

    BpfGraphicContext gc;
    public static PointRenderer staticInstance;
    public final static int INTEGER_WIDTH = 2;
    public final static int POINT_RADIUS = 5;

    static public NumberFormat numberFormat;
    static 
    {
	numberFormat = NumberFormat.getInstance();
	numberFormat.setMaximumFractionDigits(3);
	numberFormat.setGroupingUsed(false);
    }
}

