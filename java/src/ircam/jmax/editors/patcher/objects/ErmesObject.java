package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;


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

abstract public class ErmesObject implements ErmesDrawable, DisplayObject {

  static class ErmesObjInOutlet
  {
    protected static final int VISIBLE_WIDTH = 5;
    protected static final int VISIBLE_HEIGHT = 3;
    protected static final int PAD = 2;
  }

  protected ErmesSketchPad itsSketchPad;

  protected FtsObject itsFtsObject = null;

  private boolean selected = false;

  protected Font itsFont = null;
  protected FontMetrics itsFontMetrics = null;

  private Rectangle bounds;

  // Sensibility areas
  private static InletSensibilityArea inletArea = new InletSensibilityArea();
  private static OutletSensibilityArea outletArea = new OutletSensibilityArea();
  private static HResizeSensibilityArea hResizeArea = new HResizeSensibilityArea();


  // A Static method that work as a virtual constructor;
  // given an FTS object, build the proper FTS Object

  static public ErmesObject makeErmesObject( ErmesSketchPad sketch, FtsObject object) 
  {
    ErmesObject eobj;
    String theName = object.getClassName();

    if (theName.equals( "messbox"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjMessage( sketch, object);
    else if (theName.equals( "button"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjBang( sketch, object);
    else if (theName.equals( "toggle"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjToggle( sketch, object);
    else if (theName.equals( "intbox"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjInt( sketch, object);
    else if (theName.equals( "floatbox"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjFloat( sketch, object);
    else if (theName.equals( "comment"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjComment( sketch, object);
    else if ( theName.equals( "slider"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjSlider( sketch, object);
    else if (theName.equals( "inlet"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjIn( sketch, object);
    else if (theName.equals( "outlet"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjOut( sketch, object);
    else if (theName.equals( "jpatcher"))
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjPatcher( sketch, object);
    else
      eobj = new ircam.jmax.editors.patcher.objects.ErmesObjExternal( sketch, object);

    object.setObjectListener(eobj);

    return eobj;
  }

  protected ErmesObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    String fontName;
    int fontSize;

    itsSketchPad = theSketchPad;
    itsFtsObject = theFtsObject;

    selected = false;

    bounds = new Rectangle( theFtsObject.getX(), theFtsObject.getY(),
				  theFtsObject.getWidth(), theFtsObject.getHeight());

    fontName = itsFtsObject.getFont();
    fontSize = itsFtsObject.getFontSize();

    if (fontName == null)
      fontName = itsSketchPad.getDefaultFontName();

    if (fontSize < 0)      
      fontSize = itsSketchPad.getDefaultFontSize();

    itsFont = FontCache.lookupFont(fontName, fontSize);
    itsFontMetrics = FontCache.lookupFontMetrics(fontName, fontSize);
  }

  // Destructor 

  public void delete()
  {
    if (selected)
      ErmesSelection.patcherSelection.deselect(this);
    
    redraw();
    itsSketchPad.getDisplayList().remove(this);

    dispose();
    itsFtsObject.delete();
  }
  
  public final int getX() 
  {
    return bounds.x;
  }

  protected void setX( int theX) 
  {
    bounds.x = theX;
    itsFtsObject.setX( bounds.x);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getY() 
  {
    return bounds.y;
  }

  protected void setY( int theY) 
  {
    bounds.y = theY;
    itsFtsObject.setY( bounds.y);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getWidth() 
  {
    return bounds.width;
  }

  public void setWidth( int w) 
  {
    if (w > 0)
      {
	bounds.width = w;
	itsFtsObject.setWidth( w);
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  public final int getHeight() 
  {
    return bounds.height;
  }

  public void setHeight( int h) 
  {
    if (h > 0)
      {
	bounds.height = h;
	itsFtsObject.setHeight( h);
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  // Special version that do not update the connections.

  protected void setHeightNoConnections( int h) 
  {
    if (h > 0)
      {
	bounds.height = h;
	itsFtsObject.setHeight( h);
      }
  }

  public Font getFont() 
  {
    return itsFont;
  }
  
  public String getFontName()
  {
    return itsFont.getName();
  }

  public int getFontSize()
  {
    return itsFont.getSize();
  }

  public FontMetrics getFontMetrics() 
  {
    return itsFontMetrics;
  }

  public void setFontName(String fontName)
  {
    setFont(FontCache.lookupFont(fontName, itsFont.getSize()));
  }

  public void setFontSize(int size)
  {
    setFont(FontCache.lookupFont(itsFont.getName(), size));
  }

  protected void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    itsFtsObject.setFont(itsFont.getName());
    itsFtsObject.setFontSize(itsFont.getSize());;
  }

  public int getConnectionStartX( int outletNum) 
    {
      int n = itsFtsObject.getNumberOfOutlets();
      int distance = 0;

      if (n > 1)
	distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1);
      
      return getX() + ErmesObjInOutlet.PAD + outletNum*distance + ErmesObjInOutlet.VISIBLE_WIDTH/2;
    }

  public int getConnectionStartY( int outletNum) 
    {
      return getY() + getHeight();
    }

  public int getConnectionEndX( int inletNum) 
    {
      int n = itsFtsObject.getNumberOfInlets();
      int distance = 0;

      if (n > 1)
	distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1);
      
      return getX() + ErmesObjInOutlet.PAD + inletNum*distance + ErmesObjInOutlet.VISIBLE_WIDTH/2;
    }

  public int getConnectionEndY( int inletNum)
    {
      return getY() - 1;
    }

  void paintInOutlets( Graphics g, int n, int y)
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

  public void paint( Graphics g)
  {
    g.setColor( Color.black);

    paintInOutlets( g, itsFtsObject.getNumberOfInlets(), getY() - 1);
    paintInOutlets( g, itsFtsObject.getNumberOfOutlets(), getY() + getHeight() - 2);

    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
  }

  public void inspect() 
  {
  }

  // Add one to cope with the inlet/outlets

  public void redraw()
  {
    itsSketchPad.repaint( bounds.x, bounds.y - 1, bounds.width, bounds.height + 2 );
    itsSketchPad.getDisplayList().redrawConnectionsFor(this); // experimental
  }

  public void redrawConnections()
  {
    itsSketchPad.getDisplayList().redrawConnectionsFor(this);
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves

  public void redefine( String text) 
  {
  }

  final public void setSelected(boolean v) 
  {
    selected = v;
  }

  final public boolean isSelected() 
  {
    return selected;
  }

  public final FtsObject getFtsObject() 
  {
    return itsFtsObject;
  }

  public ErmesSketchPad getSketchPad() 
  {
    return itsSketchPad;
  }

  // This is a try to handle the "bug 20" problem.
  // To re-establish the old situation, just comment this method and
  // erase "_old" from the next method.

  private int findNearestInOutlet( int mouseX, int n)
  {
    // (em) consider the sensibility areas as centered in the
    // inoutlet's graphic appereance,  slice = distance/3, hole = slice, sensibility_width = 2*slice 
    if ( n ==0) return -1;
    if ( n == 1) return 0;

    int io = -1;

    int firstLetPosition =  ErmesObjInOutlet.PAD;
    int distance = (getWidth() - 2*ErmesObjInOutlet.PAD - ErmesObjInOutlet.VISIBLE_WIDTH) / (n-1); 
    int lastLetPosition = firstLetPosition + (n-1)*distance;
    

    int slice = distance / 3;

    int x = mouseX-getX();

    if ( x < firstLetPosition + slice)
      io = 0;
    else if ( x > lastLetPosition - slice)
      io = n-1;
    else 
      {
	int estimatedLet = (distance > 0)? (x-slice)/distance + 1 : 0;
	int sensibilityStartX = firstLetPosition + estimatedLet*distance - slice;
	
	if (x > sensibilityStartX &&
	    x < sensibilityStartX + 2*slice + ErmesObjInOutlet.VISIBLE_WIDTH)
	  io = estimatedLet;

      }

    return io;
  }

  public SensibilityArea findSensibilityArea( int mouseX, int mouseY)
    {
      int x = getX();
      int y = getY();
      int w = getWidth();
      int h = getHeight();

      if ( mouseX >= x + w - HResizeSensibilityArea.width 
	   && mouseY > y + InletOutletSensibilityArea.height
	   && mouseY < y + h - InletOutletSensibilityArea.height)
	{
	  hResizeArea.setObject(this);
	  return hResizeArea;
	}
      else if ( mouseY < y + InletOutletSensibilityArea.height)
	{
	  int inlet = findNearestInOutlet( mouseX, itsFtsObject.getNumberOfInlets());

	  if (inlet >= 0)
	    {
	      inletArea.setNumber( inlet);
	      inletArea.setObject(this);
	      return inletArea;
	    }

	  return null;
	}
      else if (mouseY >= y + h - InletOutletSensibilityArea.height)
	{
	  int outlet = findNearestInOutlet( mouseX, itsFtsObject.getNumberOfOutlets());

	  if ( outlet >= 0)
	    {
	      outletArea.setNumber( outlet);
	      outletArea.setObject(this);
	      return outletArea;
	    }

	  return null;
	}

      return null;
    }

  // This method is called to edit the object by means of a popup
  // structure

  public void popUpEdit(Point p)
  {
  }

  // This method is called when we want to edit the object 
  // within this editor (its text content or equivalent)
  // The point argument give the mouse click location, when
  // it is usefull

  public void edit()
  {
    edit(null);
  }

  public void edit(Point p)
  {
  }

  // This method is called whenever we want to edit the content of
  // an object within a separate editor; do nothing by default

  public void editContent()
  {
  }

  // Squeack handling; note that squeacks delivered to objects
  // have the locations fields masked to zero, to simplify code writing.

  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
  }

  public void moveBy( int theDeltaH, int theDeltaV) 
  {
    if ( theDeltaH == 0 && theDeltaV == 0)
      return;

    setX( bounds.x + theDeltaH);
    setY( bounds.y + theDeltaV);
  }

  public Rectangle getBounds() 
  {
    // (fd, mdc) Bounds don't copy the bound rectangle any more
    // and nobody is allowed to modify it.
    return bounds;
  }

  public boolean intersects(Rectangle rect)
  {
    return bounds.intersects(rect);
  }

  public boolean contains(Point point)
  {
    return bounds.contains(point);
  }

  public boolean contains(int x, int y)
  {
    return bounds.contains(x, y);
  }

  public void rectangleUnion(Rectangle rect)
  {
    SwingUtilities.computeUnion(bounds.x, bounds.y, bounds.width, bounds.height, rect);
  }


  /* SUpport for graphic ordering; temporarly, it is not
     persistent, i.e. is not stored in the FTS object */

  private int layer;

  final public void setLayer(int v)
  {
    layer = v;
  }

  final public int getLayer()
  {
    return layer;
  }

  public Dimension Size() 
  {
    return ( new Dimension( bounds.width, bounds.height));
  }

  // Called at ErmesObject disposal

  public void dispose()
  {
    itsFtsObject.setObjectListener(null);
  }


  public void showErrorDescription()
  {
    if ( itsFtsObject != null) 
      {
	int ax, ay, ah, aw;
	String annotation;
	String value;
	Graphics g;

	itsFtsObject.updateErrorDescription();
	Fts.sync();
	annotation = itsFtsObject.getErrorDescription();

	if (annotation != null)
	  {
	    ax = bounds.x + bounds.width / 2;
	    ay = bounds.y + bounds.height / 2;
	    aw = itsFontMetrics.stringWidth( annotation);
	    ah = itsFontMetrics.getHeight();

	    g = itsSketchPad.getGraphics();

	    g.setColor( Color.white);
	    g.fillRect( ax - 1 , ay - ah - 1, aw + 2, ah + 2);

	    g.setColor( Color.black);
	    g.drawRect( ax - 1, ay - ah - 1, aw + 2, ah + 2);
	    g.setFont(itsFont);
	    g.drawString( annotation, ax, ay);
	    g.dispose();
	  }
      }
  }
}



