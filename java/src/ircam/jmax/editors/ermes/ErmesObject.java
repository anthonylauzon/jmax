package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;


// The base class of all the graphic objects on the sketch.
// This class has a knowledge of its corrisponding fos object. It contains
// methods for:
// - painting, selecting, moving, dragging (services to the sketch)
// - create fos objects, change the value when the FTS value
// has changed (services to FTS), redefine itself.
// - sending values when the user interact with the object
// - handle the object with data and open the associated editor
// (example: subpatchers, table, etc.)

// A survivor...
class ErmesObjInOutlet {
  protected static final int VISIBLE_WIDTH = 5;
  protected static final int VISIBLE_HEIGHT = 3;
  protected static final int PAD = 2;
}

abstract class ErmesObject implements ErmesDrawable {

  ErmesSketchPad itsSketchPad;

  FtsObject itsFtsObject = null;

  boolean itsSelected = false;

  Font itsFont = null;
  FontMetrics itsFontMetrics = null;

  static final int PADS_DISTANCE = 12;
  static final int DRAG_DIMENSION = 4;

  private Rectangle itsRectangle = new Rectangle();

  // Sensibility areas
  private static InletSensibilityArea inletArea= new InletSensibilityArea();
  private static OutletSensibilityArea outletArea= new OutletSensibilityArea();
  private static InletOutletSensibilityArea inletOutletArea= new InletOutletSensibilityArea();
  private static ResizeSensibilityArea resizeArea = new ResizeSensibilityArea();
  private static NothingSensibilityArea nothingArea = new NothingSensibilityArea();


  // A Static method that work as a virtual constructor;
  // given an FTS object, build the proper FTS Object

  static ErmesObject makeErmesObject( ErmesSketchPad sketch, FtsObject object) 
  {
    String theName = object.getClassName();

    if (theName.equals( "messbox"))
      return new ErmesObjMessage( sketch, object);
    else if (theName.equals( "button"))
      return new ErmesObjBang( sketch, object);
    else if (theName.equals( "toggle"))
      return new ErmesObjToggle( sketch, object);
    else if (theName.equals( "intbox"))
      return new ErmesObjInt( sketch, object);
    else if (theName.equals( "floatbox"))
      return new ErmesObjFloat( sketch, object);
    else if (theName.equals( "comment"))
      return new ErmesObjComment( sketch, object);
    else if ( theName.equals( "slider"))
      return new ErmesObjSlider( sketch, object);
    else if (theName.equals( "inlet"))
      return new ErmesObjIn( sketch, object);
    else if (theName.equals( "outlet"))
      return new ErmesObjOut( sketch, object);
    else if (theName.equals( "jpatcher"))
      return new ErmesObjPatcher( sketch, object);
    else
      return new ErmesObjExternal( sketch, object);
  }

  ErmesObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    itsSketchPad = theSketchPad;

    itsFtsObject = theFtsObject;

    itsSelected = false;

    Object value = itsFtsObject.get( "font");

    Object aFont = itsFtsObject.get( "font");
    Object aSize = itsFtsObject.get( "fs");

    if (aFont instanceof FtsVoid)
      {
	if (aSize instanceof Integer)
	  {
	    int fontSize = ((Integer) aSize).intValue();

	    itsFont = FontCache.lookupFont( fontSize );
	    itsFontMetrics = FontCache.lookupFontMetrics( fontSize);
	  }
	else
	  setFont( itsSketchPad.sketchFont);
      } 
    else 
      {
	int aIntSize;

	if (aSize instanceof FtsVoid)
	  aIntSize = itsSketchPad.sketchFont.getSize();
	else
	  aIntSize = ((Integer)aSize).intValue();

	setFont( new Font( (String) aFont, itsSketchPad.sketchFont.getStyle(), aIntSize));
      }

    itsRectangle = new Rectangle( theFtsObject.getX(), theFtsObject.getY(), theFtsObject.getWidth(), theFtsObject.getHeight());
  }

  protected final int getX() 
  {
    return itsRectangle.x;
  }

  protected void setX( int theX) 
  {
    itsRectangle.x = theX;
    itsFtsObject.setX( itsRectangle.x);
  }

  protected final int getY() 
  {
    return itsRectangle.y;
  }

  protected void setY( int theY) 
  {
    itsRectangle.y = theY;
    itsFtsObject.setY( itsRectangle.y);
  }

  protected final int getWidth() 
  {
    return itsRectangle.width;
  }

  protected void setWidth( int theWidth) 
  {
    itsRectangle.width = theWidth;
    itsFtsObject.setWidth( theWidth);
  }

  protected final int getHeight() 
  {
    return itsRectangle.height;
  }

  protected void setHeight( int theHeight) 
  {
    itsRectangle.height = theHeight;
    itsFtsObject.setHeight( theHeight);
  }

  Font getFont() 
  {
    return itsFont;
  }

  void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    itsFtsObject.put( "font", itsFont.getName());
    itsFtsObject.put( "fs", itsFont.getSize());

    // (fd) Maurizio told me that it may be enough to tell
    // that the patcher is changed if you changed only the fonts.
    // So I believe him...
    itsFtsObject.localPut( "font", itsFont.getName());
    itsFtsObject.localPut( "fs", itsFont.getSize());
  }

  final ErmesSketchWindow GetSketchWindow() 
  {
    return (ErmesSketchWindow)(itsSketchPad.itsSketchWindow);
  }

  int getConnectionStartX( int outletNum) 
    {
      int n = itsFtsObject.getNumberOfOutlets();
      int distance = 0;

      if (n > 1)
	distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1);
      
      return getX() + ErmesObjInOutlet.PAD + outletNum*distance + ErmesObjInOutlet.VISIBLE_WIDTH/2;
    }

  int getConnectionStartY( int outletNum) 
    {
      return getY() + getHeight();
    }

  int getConnectionEndX( int inletNum) 
    {
      int n = itsFtsObject.getNumberOfInlets();
      int distance = 0;

      if (n > 1)
	distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1);
      
      return getX() + ErmesObjInOutlet.PAD + inletNum*distance + ErmesObjInOutlet.VISIBLE_WIDTH/2;
    }

  int getConnectionEndY( int inletNum)
    {
      return getY() - 1;
    }

  protected void paintInOutlets( Graphics g, int n, int y)
  {
    int x = getX() + ErmesObjInOutlet.PAD;
    int distance = 0;

    // Optimisation: distance could be recomputed when inlet/outlet number changes and stored in instance variable
    if (n > 1)
      distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1); 

    for ( int i = 0; i < n; i++)
      {
	g.fillRect( x, y, ErmesObjInOutlet.VISIBLE_WIDTH, ErmesObjInOutlet.VISIBLE_HEIGHT);
	x += distance;
      }
  }

  protected void Paint_specific( Graphics g)
  {
    g.setColor( Color.black);

    paintInOutlets( g, itsFtsObject.getNumberOfInlets(), getY() - 1);
    paintInOutlets( g, itsFtsObject.getNumberOfOutlets(), getY() + getHeight() - 2);

    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
  }

  void MouseDown_specific( MouseEvent e, int x, int y)
  {
  }

  void inspect() 
  {
  }

  void UpdateOnly( Graphics g) 
  {
    g.setColor( itsSketchPad.getBackground());
    g.fillRect( itsRectangle.x, itsRectangle.y, itsRectangle.width, itsRectangle.height);
  }

  public void Paint( Graphics g)  // (fd) public, because public in ErmesDrawable...
  {
    if ( itsSketchPad.isInGroup) 
      {
	//emergency situation: ignore the Graphics and paint offScreen
	Paint_specific( itsSketchPad.GetOffGraphics());
	itsSketchPad.drawPending = true;
      } 
    else
      Paint_specific( g);
  }

  protected void DoublePaint() 
  {
    //this double paint is usefull when an object change its state in run mode
    Graphics aGraphics = itsSketchPad.getGraphics();

    if ( aGraphics != null) 
      {
	Paint( aGraphics);
      }

    if ( itsSketchPad.offScreenPresent && !itsSketchPad.itsRunMode)
      Paint( itsSketchPad.GetOffGraphics());
  }

  // Should be move to an empty interface used as marker
  boolean isUIController() 
  {
    return false;
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves
  void redefine( String text) 
  {
  }

  void Select( boolean paintNow) 
  {
    if ( !itsSelected) 
      {
	itsSelected = true;

	if (paintNow)
	  DoublePaint();
	else
	  itsSketchPad.addToDirtyObjects( this);
      }
  }

  void Deselect( boolean PaintNow) 
  {
    if (itsSelected) 
      {
	itsSelected = false;
	if (PaintNow)
	  DoublePaint();
	else
	  itsSketchPad.addToDirtyObjects( this);
      }
  }

  FtsObject GetFtsObject() 
  {
    return itsFtsObject;
  }

  ErmesSketchPad GetSketchPad() 
  {
    return itsSketchPad;
  }


  private int findNearestInOutlet( int mouseX, int n)
  {
    if ( n == 0)
      return -1;

    int w = getWidth();

    int x = mouseX - getX();
    int distance = w / n;
    int hole = distance/3;
    int nearestLeft = x / (w / n);

    int io = -1;

    if (nearestLeft == 0)
      {
	if ( x < distance - hole)
	  io = 0;
      }
    else if (nearestLeft == n - 1)
      {
	if ( x > (n - 1)*distance + hole)
	  io = n - 1;
      }
    else
      {
	int nearestLeftX = nearestLeft * distance;
	
	if ( x > nearestLeftX + hole && x < nearestLeftX + distance - hole)
	  io = nearestLeft;
      }

    return io;
  }

  SensibilityArea findSensibilityArea( int mouseX, int mouseY)
    {
      int x = getX();
      int y = getY();
      int w = getWidth();
      int h = getHeight();

      if ( mouseX >= x + w - ResizeSensibilityArea.width 
	   && mouseY > y + InletOutletSensibilityArea.height
	   && mouseY < y + h - InletOutletSensibilityArea.height)
	return resizeArea;
      else if ( mouseY < y + InletOutletSensibilityArea.height)
	{
	  int inlet = findNearestInOutlet( mouseX, itsFtsObject.getNumberOfInlets());

	  if (inlet >= 0)
	    {
	      inletArea.setNumber( inlet);

	      return inletArea;
	    }

	  return nothingArea;
	}
      else if (mouseY >= y + h - InletOutletSensibilityArea.height)
	{
	  int outlet = findNearestInOutlet( mouseX, itsFtsObject.getNumberOfOutlets());

	  if ( outlet >= 0)
	    {
	      outletArea.setNumber( outlet);

	      return outletArea;
	    }

	  return nothingArea;
	}

      return nothingArea;
    }

  SensibilityArea findConnectionSensibilityArea( int mouseX, int mouseY)
    {
      int inlet = findNearestInOutlet( mouseX, itsFtsObject.getNumberOfInlets());

      if ( inlet >= 0)
	{
	  inletOutletArea.setNumber( inlet);
	  return inletOutletArea;
	}

      return nothingArea;
    }

  void MouseDrag( MouseEvent e,int x,int y) 
  {
    MouseDrag_specific( e, x, y);
  }

  void MouseDown( MouseEvent e,int x, int y) 
  {
    if ( !itsSketchPad.itsRunMode && !e.isControlDown())
      {
	if ( itsSelected)
	  itsSketchPad.clickHappenedOnAnAlreadySelected = true;
	else
	  itsSketchPad.clickHappenedOnAnAlreadySelected = false;

	MouseDown_specific( e, x, y);
      } 
    else
      MouseDown_specific( e, x, y);
  }

  void MouseUp_specific( MouseEvent e, int x, int y) 
  {
  }

  void MouseDrag_specific( MouseEvent e, int x, int y) 
  {
  }

  void MouseUp( MouseEvent e, int x, int y)
  {
    if ( itsSketchPad.itsRunMode || e.isControlDown())
      MouseUp_specific( e, x, y);
  }

  void MoveBy( int theDeltaH, int theDeltaV) 
  {
    if ( theDeltaH == 0 && theDeltaV == 0)
      return;

    setX( itsRectangle.x + theDeltaH);
    setY( itsRectangle.y + theDeltaV);
    itsRectangle.x = itsRectangle.x;
    itsRectangle.y = itsRectangle.y;
  }

  void resizeBy( int theDeltaW, int theDeltaH) 
  {
    if ( theDeltaW == 0 && theDeltaH == 0)
      return;

    if ( -theDeltaW > itsRectangle.width || -theDeltaH > itsRectangle.height)
      return;

    setWidth( itsRectangle.width + theDeltaW);
    setHeight( itsRectangle.height + theDeltaH);
  }

  void resize( int w, int h) 
  {
    setWidth( w);
    setHeight( h);
  }

  final Rectangle getBounds() 
  {
    // (fd, mdc) Bounds don't copy the bound rectangle any more
    // and nobody is allowed to modify it.
    return itsRectangle;
  }

  Dimension Size() 
  {
    return ( new Dimension( itsRectangle.width, itsRectangle.height));
  }

  // Called at ErmesObject disposal
  void cleanAll()
  {
    itsSketchPad = null;
    itsFtsObject = null;
    itsFont = null;
    itsFontMetrics = null;
  }

  // Experimental MDC
  void showAnnotation( String property) 
  {
    if ( itsFtsObject != null) 
      {
	int ax, ay, ah, aw;
	String annotation;
	Object value;
	Graphics g;

	itsSketchPad.setAnnotating();

	itsFtsObject.ask( property);
	Fts.sync();
	value = itsFtsObject.get( property);

	if (! (value instanceof FtsVoid))
	  {
	    annotation = value.toString();
	    ax = itsRectangle.x + itsRectangle.width / 2;
	    ay = itsRectangle.y + itsRectangle.height / 2;
	    aw = itsFontMetrics.stringWidth( annotation);
	    ah = itsFontMetrics.getHeight();

	    g = itsSketchPad.getGraphics();

	    g.setColor( Color.white);
	    g.fillRect( ax - 1 , ay - ah - 1, aw + 2, ah + 2);

	    g.setColor( Color.black);
	    g.drawRect( ax - 1, ay - ah - 1, aw + 2, ah + 2);
	    g.drawString( annotation, ax, ay);
	  }
      }
  }
}
