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

package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The graphic connection. It handles the paint, 
// the loading/saving of connections. It has user interface
// methods (mouseclick, move, doubleclick...).
//

public class GraphicConnection implements DisplayObject
{
  private GraphicObject from;
  private int         outlet;

  private GraphicObject to;
  private int         inlet;

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
			 FtsConnection theFtsConnection) 
  {
    ftsConnection = theFtsConnection;
    from    = fromObj;
    to      = toObj;
    sketch     = theSketchPad;
    inlet      = theInlet;
    outlet     = theOutlet;
    selected         = false;
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

  public void paint( Graphics g) 
  {
    g.setColor( Color.black);

    if ( selected) 
      {
	if ( java.lang.Math.abs(start.x - end.x) > java.lang.Math.abs(start.y - end.y))
	  {
	    g.drawLine(start.x, start.y, end.x, end.y);
	    g.drawLine(start.x, start.y+1, end.x, end.y+1);
	  } 
	else 
	  {
	    g.drawLine(start.x, start.y, end.x, end.y);
	    g.drawLine(start.x+1, start.y, end.x+1, end.y);
	  }
      } 
    else
      g.drawLine(start.x, start.y, end.x, end.y);
  }

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


