package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.Math;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;


//
// The graphic connection. It handles the paint, 
// the loading/saving of connections. It has user interface
// methods (mouseclick, move, doubleclick...).
//

public class ErmesConnection implements ErmesDrawable, DisplayObject
{
  private ErmesObject from;
  private int         outlet;

  private ErmesObject to;
  private int         inlet;

  private ErmesSketchPad sketch;
  private FtsConnection ftsConnection;
  private boolean selected;

  private int startX;
  private int startY;
  private int endX;
  private int endY;

  private boolean down;
  private boolean right;

  // For info in the near function, see http://www.exaflop.org/docs/cgafaq/

  private float length;

  public ErmesConnection(ErmesSketchPad theSketchPad,
			 ErmesObject fromObj, int theOutlet,
			 ErmesObject toObj, int theInlet,
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
    startX = from.getConnectionStartX( outlet);
    startY = from.getConnectionStartY( outlet);
    endX   = to.getConnectionEndX( inlet);
    endY   = to.getConnectionEndY( inlet);

    down  = (startX <= endX);
    right = (startY <= endY);

    length = (float) Math.sqrt((startX - endX)*(startX - endX) + (startY - endY)*(startY - endY));
  }


  ErmesObject getSourceObject() 
  {
    return from;
  }

  ErmesObject getDestObject() 
  {
    return to;
  }


  FtsConnection getFtsConnection()
  {
    return ftsConnection;
  }

  ErmesSketchPad getSketchPad()
  {
    return sketch;
  }

  // Destructor

  void delete()
  {
    ftsConnection.delete();

    if (selected)
      ErmesSelection.patcherSelection.deselect(this);

    redraw();
    sketch.getDisplayList().remove(this);
  }

  //--------------------------------------------------------
  // Select
  // select a connection
  //--------------------------------------------------------

  final void setSelected(boolean v ) 
  {
    selected = v;
  }

  final public boolean isSelected()
  {
    return selected;
  }

  boolean isNear( int x, int y)
  {
    float z = (float) ((startY - y) * (endX - x) - (startX - x) * (endY - y));

    if (z > 0.0)
      {
	if ((z/length) < 4.0)
	  System.err.println("Connection " + this + "close to point (" + x + "," + y + ")" + " distance " + z/length);
      }
    else
      {
	if ((z/length) > -4.0)
	  System.err.println("Connection " + this + "close to point (" + x + "," + y + ")" + " distance " + z/length);
      }


    if (z > 0.0)
      return ((z/length) < 4.0);
    else
      return ((z/length) > -4.0);
  }

  public void paint( Graphics g) 
  {
    g.setColor( Color.black);

    if ( selected) 
      {
	if ( java.lang.Math.abs( startX-endX)>50) 
	  {
	    g.drawLine( startX, startY, endX, endY);
	    g.drawLine( startX, startY+1, endX, endY+1);
	  } 
	else 
	  {
	    g.drawLine( startX, startY, endX, endY);
	    g.drawLine( startX-1, startY, endX-1, endY);
	  }
      } 
    else
      g.drawLine( startX, startY, endX, endY);
  }

  // Connections should store their bounds !!!

  public void redraw()
  {
    if (down)
      {
	if (right)
      	  sketch.repaint(startX, startY, endX - startX, endY - startY);
	else
	  sketch.repaint(startX, endY, endX - startX, startY - endY);
      }
    else
      if (right)
	sketch.repaint(endX, startY, startX - endX, endY - startY);
    else
	sketch.repaint(endX, endY, startX - endX, startY - endY);
  }


  public final boolean intersects(Rectangle r)
  {
    if (down)
      {
	if (right)
	  return !((r.x + r.width <= startX) || (r.y + r.height <= startY) ||
		   (r.x >= endX) || (r.y >= endY));
	else
	  return !((r.x + r.width <= startX) || (r.y + r.height <= endY) ||
		   (r.x >= endX) || (r.y >= startY));
      }
    else
      {
	if (right)
	  return !((r.x + r.width <= endX) || (r.y + r.height <= startY) ||
		   (r.x >= startX) || (r.y >= endY));
	else
	  return !((r.x + r.width <= endX) || (r.y + r.height <= endY) ||
		   (r.x >= startX) || (r.y >= startY));
      }
  }


  // Print function

  public String toString()
  {
    return ("ErmesConnection<" + from + "." + outlet + "-" +
	    to + "." + inlet +
	    " (" + startX + "." + startY + ")-" + length + "-(" + endX + "." + endY + ")>");
  }
  
}


