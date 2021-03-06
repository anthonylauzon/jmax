//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
			{
			    int nextX = adapter.getX(next)+((UtilBpfPoint)point).getDeltaX(adapter);
			    g.drawLine(x, y, nextX, adapter.getY(next)+((UtilBpfPoint)point).getDeltaY(adapter));
			}
	    }
	else //normal paint
	    {
		next = model.getNextPoint(point); 
		      
		if(selected)
		    g.setColor(Color.black);
		
		if(next != null)
		    {			
			int nextX = adapter.getX(next);
			int nextY = adapter.getY(next);

			g.drawLine(x, y, nextX, nextY); 			

			if( selected && bgc.getSelection().isInSelection(next))  
			    {
				if( java.lang.Math.abs(x - nextX) > java.lang.Math.abs(y - nextY))
				    g.drawLine(x, y+1, nextX, nextY+1);
				else 
				    g.drawLine(x+1, y, nextX+1, nextY);
			    } 				
		    }
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

