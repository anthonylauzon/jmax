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
  ErmesObject itsFromObject;
  int itsOutletNum;

  ErmesObject itsToObject;
  int itsInletNum;

  ErmesSketchPad itsSketchPad;
  FtsConnection itsFtsConnection;
  private boolean selected;

  private int startX;
  private int startY;
  private int endX;
  private int endY;

  static final int UP_RIGHT = 0;
  static final int UP_LEFT  = 0;
  static final int DOWN_RIGHT = 0;
  static final int DOWN_LEFT  = 0;
  private boolean down;
  private boolean right;

  // For info in the near function, see http://www.exaflop.org/docs/cgafaq/

  private int squaredK;

  public ErmesConnection(ErmesSketchPad theSketchPad,
			 ErmesObject fromObj, int theOutlet,
			 ErmesObject toObj, int theInlet,
			 FtsConnection theFtsConnection) 
  {
    itsFtsConnection = theFtsConnection;
    itsFromObject    = fromObj;
    itsToObject      = toObj;
    itsSketchPad     = theSketchPad;
    itsInletNum      = theInlet;
    itsOutletNum     = theOutlet;
    selected         = false;
  }


  void update()
  {
    startX = itsFromObject.getConnectionStartX( itsOutletNum);
    startY = itsFromObject.getConnectionStartY( itsOutletNum);
    endX   = itsToObject.getConnectionEndX( itsInletNum);
    endY   = itsToObject.getConnectionEndY( itsInletNum);

    down  = (startX <= endX);
    right = (startY <= endY);

    squaredK = 9 * ((startX - endX)*(startX - endX) + (startY - endY)*(startY - endY));
  }

  ErmesSketchPad getSketchPad() 
  {
    return itsSketchPad;
  }

  ErmesObject getSourceObject() 
  {
    return itsFromObject;
  }

  ErmesObject getDestObject() 
  {
    return itsToObject;
  }

  
  // Destructor

  void delete()
  {
    itsFtsConnection.delete();

    if (selected)
      ErmesSelection.patcherSelection.deselect(this);
    redraw();
    itsSketchPad.getDisplayList().remove(this);
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
    int z = (startY - y) * (endX - x) - (startX - x) * (endY - y);

    return (z*z < squaredK);
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
      	  itsSketchPad.repaint(startX, startY, endX - startX, endY - startY);
	else
	  itsSketchPad.repaint(startX, endY, endX - startX, startY - endY);
      }
    else
      if (right)
	itsSketchPad.repaint(endX, startY, startX - endX, endY - startY);
    else
	itsSketchPad.repaint(endX, endY, startX - endX, startY - endY);
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
}


