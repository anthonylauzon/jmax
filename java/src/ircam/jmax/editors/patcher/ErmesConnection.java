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
  Rectangle  itsRectangle = new Rectangle();

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

  
  public ErmesConnection( ErmesSketchPad theSketchPad, ErmesObject fromObj, int theOutlet, ErmesObject toObj, int theInlet, FtsConnection theFtsConnection) 
    {
      itsFtsConnection = theFtsConnection;
      itsFromObject = fromObj;
      itsToObject = toObj;
      itsSketchPad = theSketchPad;
      itsInletNum = theInlet;
      itsOutletNum = theOutlet;
      selected = false;
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

  boolean IsNearToPoint( int x, int y) 
    {
      int startX = itsFromObject.getConnectionStartX( itsOutletNum);
      int startY = itsFromObject.getConnectionStartY( itsOutletNum);
      int endX = itsToObject.getConnectionEndX( itsInletNum);
      int endY = itsToObject.getConnectionEndY( itsInletNum);

      int dx = java.lang.Math.abs( endX - startX);
      int dy = java.lang.Math.abs( endY - startY);

      if ( dx > dy)
	return ( SegmentIntersect( x, y-3, x, y+3, startX, startY, endX, endY));
      else
	return ( SegmentIntersect( x-3, y, x+3, y, startX, startY, endX, endY));
    }


  private boolean SegmentIntersect( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) 
    {
      int dx21 = x2 - x1;
      int dx31 = x3 - x1;
      int dx41 = x4 - x1;
      int dy21 = y2 - y1;
      int dy31 = y3 - y1;
      int dy41 = y4 - y1;
      int det1 = dx31*dy21 - dy31*dx21;
      int det2 = dx41*dy21 - dy41*dx21;

      if ( ( det1 == 0)&&( det2 == 0)) 
	{
	  if ( ( y1 == y2)&&( y1 == y3)&&( y1 == y4))
	    return ( Scambio( x1, x2, x3, x4));
	  else
	    return ( Scambio( y1, y2, y3, y4));
	} 
      else 
	{
	  int dx32 = x3 - x2;
	  int dx34 = x3 - x4;
	  int dy32 = y3 - y2;
	  int dy34 = y3 - y4;
	  int det3 = dx34*dy31 - dy34*dx31;
	  int det4 = dx34*dy32 - dy34*dx32;
	  return ( ( ( ( det1<=0)&&( det2>=0))||( ( det1>=0)&&( det2<=0)))&&
		   ( ( ( det3<=0)&&( det4>=0))||( ( det3>=0)&&( det4<=0))));
	}
    }

  boolean Scambio( int x1, int x2, int x3, int x4) 
    {
      int temp;
      if ( x2 < x1) 
	{
	  temp = x2;
	  x2 = x1;
	  x1 = temp;
	}
      if ( x4 < x3) 
	{
	  temp = x4;
	  x4 = x3;
	  x3 = temp;
	}

      return ( ( x2 > x3)&&( x4 > x1));
    }

  public void Paint( Graphics g) 
    {
      int startX = itsFromObject.getConnectionStartX( itsOutletNum);
      int startY = itsFromObject.getConnectionStartY( itsOutletNum);
      int endX = itsToObject.getConnectionEndX( itsInletNum);
      int endY = itsToObject.getConnectionEndY( itsInletNum);

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
    int startX = itsFromObject.getConnectionStartX( itsOutletNum);
    int startY = itsFromObject.getConnectionStartY( itsOutletNum);
    int endX = itsToObject.getConnectionEndX( itsInletNum);
    int endY = itsToObject.getConnectionEndY( itsInletNum);
      
    itsSketchPad.repaint(Math.min(startX, endX), Math.min(startY, endY),
			 Math.abs(startX - endX), Math.abs(startY - endY));
  }

  public Rectangle getBounds()
  {
    int startX = itsFromObject.getConnectionStartX( itsOutletNum);
    int startY = itsFromObject.getConnectionStartY( itsOutletNum);
    int endX = itsToObject.getConnectionEndX( itsInletNum);
    int endY = itsToObject.getConnectionEndY( itsInletNum);
      
    itsRectangle.x      = Math.min(startX, endX);
    itsRectangle.y      = Math.min(startY, endY);
    itsRectangle.width  = Math.abs(startX - endX);
    itsRectangle.height = Math.abs(startY - endY);

    return itsRectangle;
  }
}


