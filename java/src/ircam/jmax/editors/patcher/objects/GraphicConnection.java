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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;

import ircam.jmax.fts.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The graphic connection. It handles the paint, 
// the loading/saving of connections. It has user interface
// methods (mouseclick, move, doubleclick...).
//

public class GraphicConnection implements DisplayObject, FtsConnectionListener
{
  private GraphicObject from;
  private int         outlet;

  private GraphicObject to;
  private int         inlet;

  private int         type;

  private ErmesSketchPad sketch;
  private FtsConnection ftsConnection;
  private boolean selected;

  private Point start = new Point();
  private Point end = new Point();

  private boolean down;
  private boolean right;

  // For info in the near function, see http://www.exaflop.org/docs/cgafaq/

  private float length;

  public GraphicConnection(ErmesSketchPad theSketchPad,
			   GraphicObject fromObj, int theOutlet,
			   GraphicObject toObj, int theInlet, 
			   int theType, 
			   FtsConnection theFtsConnection) 
  {
    ftsConnection = theFtsConnection;
    from    = fromObj;
    to      = toObj;
    sketch     = theSketchPad;
    inlet      = theInlet;
    outlet     = theOutlet;
    type = theType;
    selected         = false;

    ftsConnection.setConnectionListener(this);
  }

  public void updateDimensions()
  {
    start.x = from.getOutletAnchorX(outlet);
    start.y = from.getOutletAnchorY(outlet);
    end.x   = to.getInletAnchorX(inlet);
    end.y   = to.getInletAnchorY(inlet);

    down  = (start.x <= end.x);
    right = (start.y <= end.y);

    length = (float) Math.sqrt((start.x - end.x)*(start.x - end.x) + (start.y - end.y)*(start.y - end.y));
  }

  public void updateType()
  {
    type = ftsConnection.getType();
  }

  public GraphicObject getSourceObject() 
  {
    return from;
  }

  public GraphicObject getDestObject() 
  {
    return to;
  }


  public int getInlet()
  {
    return inlet;
  }


  public int getOutlet()
  {
    return outlet;
  }

  public FtsConnection getFtsConnection()
  {
    return ftsConnection;
  }

  public ErmesSketchPad getSketchPad()
  {
    return sketch;
  }

  // Destructor

  public void delete()
  {
    ftsConnection.delete();
    sketch.getDisplayList().remove(this);
  }

  public void typeChanged(int theType)
  {
    type = theType;
    redraw();
  }

  //--------------------------------------------------------
  // Select
  // select a connection
  //--------------------------------------------------------

  final public void setSelected(boolean v ) 
  {
    selected = v;
  }

  final public boolean isSelected()
  {
    return selected;
  }

  private final boolean isNear( int x, int y)
  {
    // First, answer false for all the points outside a bounding rectangle for
    // the connection

    if (down)
      {
	if ((x < (start.x - 4)) || (x > (end.x + 4)))
	  return false;
      }
    else
      {
	if ((x < (end.x - 4)) || (x > (start.x + 4)))
	  return false;
      }

    if (right)
      {
	if ((y < start.y - 4) || (y > end.y + 4))
	  return false;
      }
    else
      {
	if ((y < end.y - 4) || (y > start.y + 4))
	  return false;
      }

    float z = (float) ((start.y - y) * (end.x - start.x) - (start.x - x) * (end.y - start.y));

    if (z > 0.0)
      return ((z/length) < 4.0);
    else
      return ((z/length) > -4.0);
  }

  public SensibilityArea getSensibilityAreaAt( int mouseX, int mouseY)
  {
    if (isNear(mouseX, mouseY))
      return SensibilityArea.get(this, Squeack.CONNECTION);
    else
      return null;
  }

    
    //final static float dash1[] = {10.0f, 3.0f, 2.0f, 3.0f};
  final static float unity = 2.0f;
  final static float dash1[] = {unity, 3*unity};
  final static float dash2[] = {unity, unity};
  final static BasicStroke dashed1 = new BasicStroke(1.0f, 
						     BasicStroke.CAP_BUTT, 
						     BasicStroke.JOIN_MITER, 
						     10.0f, dash1, unity);
  final static BasicStroke dashed2 = new BasicStroke(1.0f, 
						     BasicStroke.CAP_BUTT, 
						     BasicStroke.JOIN_MITER, 
						     10.0f, dash2, 0);
  final static BasicStroke dashed3 = new BasicStroke(1.0f, 
						     BasicStroke.CAP_BUTT, 
						     BasicStroke.JOIN_MITER, 
						     10.0f, dash1, 3*unity);
  final static BasicStroke normal = new BasicStroke(1.0f, 
						    BasicStroke.CAP_BUTT, 
						    BasicStroke.JOIN_MITER, 
						    10.0f);

  public void paint( Graphics g) 
  {
      //Color aubergine = new Color(100, 100, 190);

    if(type == FtsConnection.fts_connection_signal)	
	{
	    g.setColor(Color.black);

	    double angle = Math.abs(Math.atan2((float)(start.y - end.y), (float)(end.x - start.x)));
	    if((angle>Math.PI/4)&&(angle<Math.PI*3/4))
		{		 
		    if(selected)
			{
			    ((Graphics2D)g).setStroke(dashed1);
			    g.drawLine(start.x+1, start.y, end.x+1, end.y);
			    g.drawLine(start.x+2, start.y, end.x+2, end.y);
			    ((Graphics2D)g).setStroke(dashed2);
			    g.drawLine(start.x, start.y, end.x, end.y);
			    g.drawLine(start.x+1, start.y, end.x+1, end.y);
			    ((Graphics2D)g).setStroke(dashed3);
			    g.drawLine(start.x-1, start.y, end.x-1, end.y);
			    g.drawLine(start.x, start.y, end.x, end.y); 
			}
		    else
			{
			    ((Graphics2D)g).setStroke(dashed1);
			    g.drawLine(start.x+1, start.y, end.x+1, end.y);
			    ((Graphics2D)g).setStroke(dashed2);
			    g.drawLine(start.x, start.y, end.x, end.y);
			    ((Graphics2D)g).setStroke(dashed3);
			    g.drawLine(start.x-1, start.y, end.x-1, end.y);
			}
		}
	    else
		{
		    if(selected)
			{
			    ((Graphics2D)g).setStroke(dashed1);
			    g.drawLine(start.x, start.y-1, end.x, end.y-1);
			    g.drawLine(start.x, start.y, end.x, end.y);
			    ((Graphics2D)g).setStroke(dashed2);
			    g.drawLine(start.x, start.y, end.x, end.y);
			    g.drawLine(start.x, start.y+1, end.x, end.y+1);
			    ((Graphics2D)g).setStroke(dashed3);
			    g.drawLine(start.x, start.y+1, end.x, end.y+1); 
			    g.drawLine(start.x, start.y+2, end.x, end.y+2); 
			}
		    else
			{
			    ((Graphics2D)g).setStroke(dashed1);
			    g.drawLine(start.x, start.y-1, end.x, end.y-1);
			    ((Graphics2D)g).setStroke(dashed2);
			    g.drawLine(start.x, start.y, end.x, end.y);
			    ((Graphics2D)g).setStroke(dashed3);
			    g.drawLine(start.x, start.y+1, end.x, end.y+1); 
			}
		}

	    ((Graphics2D)g).setStroke(normal);
	}
    else
	{
	    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
	    
	    if(type == FtsConnection.fts_connection_invalid)
		g.setColor( Color.gray);
	    else
		g.setColor( Color.black);
		
	    ((Graphics2D)g).drawLine(start.x, start.y, end.x, end.y);
	    if (selected)  
		{
		    if ( java.lang.Math.abs(start.x - end.x) > java.lang.Math.abs(start.y - end.y))
			g.drawLine(start.x, start.y+1, end.x, end.y+1);
		    else 
			g.drawLine(start.x+1, start.y, end.x+1, end.y);
		} 	    
	    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
	}
  }

  public void updatePaint(Graphics g){}

  // Connections should store their bounds !!!

  public void redraw()
  {
    int x, y;
    int height, width;

    if (down)
      {
	x = start.x - 2;
	width = end.x - start.x + 4;
      }
    else
      {
	x = end.x - 2;
	width = start.x - end.x + 4;
      }

    if (right)
      {
	y = start.y - 2;
	height  = end.y - start.y + 4;
      }
    else
      {
	y = end.y - 2;
	height = start.y - end.y + 4;
      }

    sketch.repaint(x, y, width, height);
  }


  public final boolean intersects(Rectangle r)
  {
    boolean ret;

    if (down)
      {
	if (right)
	  ret =  !((r.x + r.width < start.x) || (r.y + r.height < start.y) ||
		   (r.x > end.x) || (r.y > end.y));
	else
	  ret =  !((r.x + r.width < start.x) || (r.y + r.height < end.y) ||
		   (r.x > end.x) || (r.y > start.y));
      }
    else
      {
	if (right)
	  ret = !((r.x + r.width < end.x) || (r.y + r.height < start.y) ||
		   (r.x > start.x) || (r.y > end.y));
	else
	  ret =  !((r.x + r.width < end.x) || (r.y + r.height < end.y) ||
		   (r.x > start.x) || (r.y > start.y));
      }

    return ret;
  }


  // Print function

  public String toString()
  {
    return ("GraphicConnection<" + from + "." + outlet + "-" +
	    to + "." + inlet +
	    " (" + start.x + "." + start.y + ")-" + length + "-(" + end.x + "." + end.y + ")>");
  }
  
}
