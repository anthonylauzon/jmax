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
    // First, answer false for all the points outside a bounding rectangle for
    // the connection

    if (down)
      {
	if ((x < (startX - 4)) || (x > (endX + 4)))
	  return false;
      }
    else
      {
	if ((x < (endX - 4)) || (x > (startX + 4)))
	  return false;
      }

    if (right)
      {
	if ((y < startY - 4) || (y > endY + 4))
	  return false;
      }
    else
      {
	if ((y < endY - 4) || (y > startY + 4))
	  return false;
      }

    float z = (float) ((startY - y) * (endX - startX) - (startX - x) * (endY - startY));

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
	if ( java.lang.Math.abs(startX - endX) > java.lang.Math.abs(startY - endY))
	  {
	    g.drawLine(startX, startY, endX, endY);
	    g.drawLine(startX, startY+1, endX, endY+1);
	  } 
	else 
	  {
	    g.drawLine(startX, startY, endX, endY);
	    g.drawLine(startX+1, startY, endX+1, endY);
	  }
      } 
    else
      g.drawLine(startX, startY, endX, endY);
  }

  // Connections should store their bounds !!!

  public void redraw()
  {
    int x, y;
    int height, width;

    if (down)
      {
	x = startX - 2;
	width = endX - startX + 4;
      }
    else
      {
	x = endX - 2;
	width = startX - endX + 4;
      }

    if (right)
      {
	y = startY - 2;
	height  = endY - startY + 4;
      }
    else
      {
	y = endY - 2;
	height = startY - endY + 4;
      }

    sketch.repaint(x, y, width, height);
  }


  public final boolean intersects(Rectangle r)
  {
    boolean ret;

    if (down)
      {
	if (right)
	  ret =  !((r.x + r.width < startX) || (r.y + r.height < startY) ||
		   (r.x > endX) || (r.y > endY));
	else
	  ret =  !((r.x + r.width < startX) || (r.y + r.height < endY) ||
		   (r.x > endX) || (r.y > startY));
      }
    else
      {
	if (right)
	  ret = !((r.x + r.width < endX) || (r.y + r.height < startY) ||
		   (r.x > startX) || (r.y > endY));
	else
	  ret =  !((r.x + r.width < endX) || (r.y + r.height < endY) ||
		   (r.x > startX) || (r.y > startY));
      }

    return ret;
  }


  // Print function

  public String toString()
  {
    return ("ErmesConnection<" + from + "." + outlet + "-" +
	    to + "." + inlet +
	    " (" + startX + "." + startY + ")-" + length + "-(" + endX + "." + endY + ")>");
  }
  
}


