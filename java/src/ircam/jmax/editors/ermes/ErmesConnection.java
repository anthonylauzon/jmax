package ircam.jmax.editors.frobber;

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

  public ErmesObjInlet itsInlet;
  public ErmesObjOutlet itsOutlet;

  int itsInletNum;
  int itsOutletNum;
  ErmesSketchPad itsSketchPad;
  FtsConnection itsFtsConnection;
  ErmesObject itsFromObject;
  ErmesObject itsToObject;
  boolean itsSelected;

  public Point getStartPoint() 
  {
    return itsOutlet.GetAnchorPoint();
  }

  public Point getEndPoint() 
  {
    return itsInlet.GetAnchorPoint();
  }

  public ErmesSketchPad GetSketchPad() 
  {
    return itsSketchPad;
  }

  public ErmesObjInlet GetInlet() 
  {
    return itsInlet;
  }

  public ErmesObjOutlet GetOutlet() 
  {
    return itsOutlet;
  }

  public boolean GetSelected() 
  {
    return itsSelected;
  }

  public ErmesObject GetSourceObject() 
  {
    return itsOutlet.GetOwner();
  }

  public ErmesObject GetDestObject() 
  {
    return itsInlet.GetOwner();
  }

  //--------------------------------------------------------
  // CONSTRUCTOR
  // NOTE this constructor builds also an instance of FtsConnection. It is intended
  // to be used from user-driven connections, unlike the second constructor, which receives
  // the FtsConnection. This is going to change in a middle-term (4/03/97)
  //--------------------------------------------------------
  public ErmesConnection( ErmesSketchPad theSketchPad, ErmesObjInlet theInlet, ErmesObjOutlet theOutlet) throws FtsException 
  {
    itsSketchPad = theSketchPad;
    itsInlet = theInlet;
    itsOutlet = theOutlet;
    itsFromObject = itsOutlet.GetOwner();
    itsToObject = itsInlet.GetOwner();

    try 
      {
	itsFtsConnection = Fts.makeFtsConnection( itsFromObject.itsFtsObject, theOutlet.getNum(), itsToObject.itsFtsObject, theInlet.getNum());
      } 
    catch ( FtsException e) 
      {
	// ????????
	System.out.println( "Connection Error");
	throw e;
      }

    itsSelected = false;
  }


  public ErmesConnection( ErmesObject fromObj, ErmesObject toObj, ErmesSketchPad theSketchPad, int theOutlet, int theInlet, FtsConnection theFtsConnection) 
  {
    itsFtsConnection = theFtsConnection;
    itsFromObject = fromObj;
    itsToObject = toObj;
    itsSketchPad = theSketchPad;
    itsInletNum = theInlet;
    itsOutletNum = theOutlet;
    itsSelected = false;
  }

  public void update( FtsConnection theFtsConnection) 
  {
    itsFtsConnection = theFtsConnection;
    itsInlet = (ErmesObjInlet) itsToObject.itsInletList.elementAt( itsInletNum);
    itsOutlet = (ErmesObjOutlet)itsFromObject.itsOutletList.elementAt( itsOutletNum);
    itsOutlet.AddConnection( this);
    itsOutlet.setSelected( false);
    itsInlet.AddConnection( this);
    itsInlet.setSelected( false);
    itsSketchPad.itsConnections.addElement( this);
  }


  //--------------------------------------------------------
  // Select
  // select a connection
  //--------------------------------------------------------
  public void Select()
  {
    if ( !itsSelected) 
      itsSelected = true;
  }

  //--------------------------------------------------------
  // Deselect
  // deselect a connection
  //--------------------------------------------------------
  public void Deselect() 
  {
    if ( itsSelected) 
      itsSelected = false;
  }

  public void MouseDown( MouseEvent evt, int x, int y) 
  {
    if ( !itsSketchPad.itsRunMode) 
      {
	itsSketchPad.ClickOnConnection( this, evt, x, y);
      }
  }

  boolean IsNearToPoint( int x, int y) 
  {

    Point start = getStartPoint();
    Point end = getEndPoint();

    int dx = java.lang.Math.abs( end.x - start.x);
    int dy = java.lang.Math.abs( end.y - start.y);

    if ( dx > dy)
      return ( SegmentIntersect( x, y-3, x, y+3, start.x, start.y, end.x, end.y));
    else
      return ( SegmentIntersect( x-3, y, x+3, y, start.x, start.y, end.x, end.y));
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
    Point start = getStartPoint();
    Point end = getEndPoint();

    g.setColor( Color.black);
    if ( itsSelected) 
      {
	if ( java.lang.Math.abs( start.x-end.x)>50) 
	  {
	    g.drawLine( start.x, start.y, end.x, end.y);
	    g.drawLine( start.x, start.y+1, end.x, end.y+1);
	  } 
	else 
	  {
	    g.drawLine( start.x, start.y, end.x, end.y);
	    g.drawLine( start.x-1, start.y, end.x-1, end.y);
	  }
      } 
    else
      g.drawLine( start.x, start.y, end.x, end.y);
  }
}


