package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import java.lang.Math;

//
// The graphic connection. It handles the paint, the autorouting,
// the loading/saving of connections. It has user interface
// methods (mouseclick, move, doubleclick...).
//

class ErmesConnection implements ErmesDrawable {

  ErmesObject itsFromObject;
  int itsOutletNum;

  ErmesObject itsToObject;
  int itsInletNum;

  ErmesSketchPad itsSketchPad;
  FtsConnection itsFtsConnection;
  boolean itsSelected;

  ErmesSketchPad GetSketchPad() 
    {
      return itsSketchPad;
    }

  boolean GetSelected() 
    {
      return itsSelected;
    }

  ErmesObject getSourceObject() 
    {
      return itsFromObject;
    }

  ErmesObject getDestObject() 
    {
      return itsToObject;
    }

  ErmesConnection( ErmesSketchPad theSketchPad, ErmesObject fromObj, int fromOutlet, ErmesObject toObj, int toInlet) throws FtsException
    {
      itsSketchPad = theSketchPad;

      itsFromObject = fromObj;
      itsOutletNum = fromOutlet;

      itsToObject = toObj;
      itsInletNum = toInlet;

      try 
	{
	  itsFtsConnection = Fts.makeFtsConnection( itsFromObject.itsFtsObject, itsOutletNum, itsToObject.itsFtsObject, itsInletNum);
	} 
      catch ( FtsException e) 
	{
	  // ????????
	  System.out.println( "Connection Error");
	  throw e;
	}

      itsSelected = false;

      update( itsFtsConnection); // (fd) ???
    }


  ErmesConnection( ErmesSketchPad theSketchPad, ErmesObject fromObj, ErmesObject toObj, int theOutlet, int theInlet, FtsConnection theFtsConnection) 
    {
      itsFtsConnection = theFtsConnection;
      itsFromObject = fromObj;
      itsToObject = toObj;
      itsSketchPad = theSketchPad;
      itsInletNum = theInlet;
      itsOutletNum = theOutlet;
      itsSelected = false;
    }

  void update( FtsConnection theFtsConnection) 
    {
      itsFtsConnection = theFtsConnection;
      itsSketchPad.itsConnections.addElement( this);
    }


  //--------------------------------------------------------
  // Select
  // select a connection
  //--------------------------------------------------------
  void Select( boolean paintNow) 
    {
      if ( !itsSelected) 
	{
	  itsSelected = true;
	  if ( paintNow)
	    DoublePaint();
	  else
	    itsSketchPad.addToDirtyConnections( this);
	}
    }

  //--------------------------------------------------------
  // Deselect
  // deselect a connection
  //--------------------------------------------------------
  void Deselect( boolean paintNow) 
    {
      if ( itsSelected) 
	{
	  itsSelected = false;
	  itsSketchPad.markSketchAsDirty();
	  if ( paintNow)
	    itsSketchPad.paintDirtyList();
	}
    }

  void MouseDown( MouseEvent evt, int x, int y) 
    {
      if ( !itsSketchPad.itsRunMode) 
	{
	  itsSketchPad.ClickOnConnection( this, evt, x, y);
	}
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

  void DoublePaint( ) 
    {
      //this double paint is usefull when an object schange its state in run mode
      Graphics aGraphics = itsSketchPad.getGraphics();

      if ( aGraphics != null) 
	{
	  Paint( aGraphics);
	  aGraphics.dispose();
	}

      if ( itsSketchPad.offScreenPresent)
	Paint( itsSketchPad.GetOffGraphics());
    }

  public void Paint( Graphics g) 
    {
      int startX = itsFromObject.getConnectionStartX( itsOutletNum);
      int startY = itsFromObject.getConnectionStartY( itsOutletNum);
      int endX = itsToObject.getConnectionEndX( itsInletNum);
      int endY = itsToObject.getConnectionEndY( itsInletNum);

      g.setColor( Color.black);
      if ( itsSelected) 
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
}
