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
    // All this parameters can be changed without changing the geometry
    // of the patch, i.e. the connection positions, unless stated otherwise

    protected static final int WIDTH = 5;
    protected static final int HEIGHT = 3;
    protected static final int HIGHLIGHTED_HEIGHT = 6;
    protected static final int HIGHLIGHTED_WIDTH = 6;

    // PAD is the distance between the object border and the 
    // center of the inlet/outlet; CHANGE the Connection geometry

    protected static final int PAD = 4;

    // INLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    protected static final int INLET_OVERLAP = 2;

    // INLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    protected static final int INLET_OFFSET = 1;

    // OUTLET_OVERLAP is the part of the inlet rectangle that
    // go inside the object

    protected static final int OUTLET_OVERLAP = 2;

    // OUTLET_OFFSET is the vertical distance between the anchor point
    // and the object; CHANGE the Connection geometry

    protected static final int OUTLET_OFFSET = 0;
  }

  protected ErmesSketchPad itsSketchPad;

  protected FtsObject ftsObject = null;

  private boolean selected = false;

  int inletDistance; // the distance between two inlets anchor point
  int outletDistance; // the distance between two outlets anchor point

  protected Font itsFont = null;
  protected FontMetrics itsFontMetrics = null;

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
    ftsObject = theFtsObject;

    selected = false;

    fontName = ftsObject.getFont();
    fontSize = ftsObject.getFontSize();

    if (fontName == null)
      fontName = itsSketchPad.getDefaultFontName();

    if (fontSize < 0)      
      fontSize = itsSketchPad.getDefaultFontSize();

    itsFont = FontCache.lookupFont(fontName, fontSize);
    itsFontMetrics = FontCache.lookupFontMetrics(fontName, fontSize);

    updateInOutlets();
  }

  // Destructor 

  public void delete()
  {
    if (selected)
      ErmesSelection.patcherSelection.deselect(this);
    
    redraw();
    itsSketchPad.getDisplayList().remove(this);

    dispose();
    ftsObject.delete();
  }
  
  public final int getX() 
  {
    return ftsObject.getX();
  }

  protected void setX( int x) 
  {
    ftsObject.setX(x);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getY() 
  {
    return ftsObject.getY();
  }

  protected void setY( int y) 
  {
    ftsObject.setY(y);
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public final int getWidth() 
  {
    return ftsObject.getWidth();
  }

  public void setWidth( int w) 
  {
    if (w > 0)
      {
	ftsObject.setWidth(w);
	updateInOutlets();
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  public final int getHeight() 
  {
    return ftsObject.getHeight();
  }

  public void setHeight( int h) 
  {
    if (h > 0)
      {
	ftsObject.setHeight(h);
	itsSketchPad.getDisplayList().updateConnectionsFor(this);
      }
  }

  // Special version that do not update the connections.

  protected void setHeightNoConnections( int h) 
  {
    if (h > 0)
      {
	ftsObject.setHeight(h);
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

    ftsObject.setFont(itsFont.getName());
    ftsObject.setFontSize(itsFont.getSize());;
  }

  public final int getOutletAnchorX(int outlet)
  {
    return getX() + ErmesObjInOutlet.PAD + outlet * outletDistance;
  }

  public final int getOutletAnchorY(int outlet)
  {
    return getY() + getHeight();
  }

  public int getInletAnchorX(int inlet)
  {
    return getX() + ErmesObjInOutlet.PAD + inlet * inletDistance;
  }

  public int getInletAnchorY(int inlet)
  {
    return getY() - 1;
  }

  private void paintInlets(Graphics g)
  {
    int n = ftsObject.getNumberOfInlets();

    for ( int i = 0; i < n; i++)
      {
	int x, y, w, h;

	x = getInletAnchorX(i);
	y = getInletAnchorY(i);

	if (itsSketchPad.isHighlightedInlet(this, i))
	  {
	    w = ErmesObjInOutlet.HIGHLIGHTED_WIDTH;
	    h = ErmesObjInOutlet.HIGHLIGHTED_HEIGHT;
	  }
	else
	  {
	    w = ErmesObjInOutlet.WIDTH;
	    h = ErmesObjInOutlet.HEIGHT;
	  }
	
	g.fillRect( x - w / 2, y - h + ErmesObjInOutlet.INLET_OVERLAP + ErmesObjInOutlet.INLET_OFFSET, w, h);
      }
  }

  private void paintOutlets(Graphics g)
  {
    int n = ftsObject.getNumberOfOutlets();

    for ( int i = 0; i < n; i++)
      {
	int x, y, w, h;

	x = getOutletAnchorX(i);
	y = getOutletAnchorY(i);

	if (itsSketchPad.isHighlightedOutlet(this, i))
	  {
	    w = ErmesObjInOutlet.HIGHLIGHTED_WIDTH;
	    h = ErmesObjInOutlet.HIGHLIGHTED_HEIGHT;
	  }
	else
	  {
	    w = ErmesObjInOutlet.WIDTH;
	    h = ErmesObjInOutlet.HEIGHT;
	  }
	
	g.fillRect( x - w / 2, y - ErmesObjInOutlet.OUTLET_OVERLAP - ErmesObjInOutlet.OUTLET_OFFSET, w, h);
      }
  }

  public void paint( Graphics g)
  {
    g.setColor( Color.black);

    paintInlets(g);
    paintOutlets(g);

    g.drawRect( getX(), getY(), getWidth()-1, getHeight()-1);
  }

  public void inspect() 
  {
  }

  // Add one to cope with the inlet/outlets

  public void redraw()
  {
    itsSketchPad.repaint(ftsObject.getX(),
			 ftsObject.getY() - ErmesObjInOutlet.HIGHLIGHTED_HEIGHT +
			 ErmesObjInOutlet.INLET_OFFSET + ErmesObjInOutlet.INLET_OVERLAP,
			 ftsObject.getWidth(),
			 ftsObject.getHeight() + 2 * ErmesObjInOutlet.HIGHLIGHTED_HEIGHT  -
			 ErmesObjInOutlet.INLET_OFFSET - ErmesObjInOutlet.INLET_OVERLAP -
			 ErmesObjInOutlet.OUTLET_OFFSET - ErmesObjInOutlet.OUTLET_OVERLAP);

    itsSketchPad.getDisplayList().redrawConnectionsFor(this); // experimental
  }

  public void redrawConnections()
  {
    itsSketchPad.getDisplayList().redrawConnectionsFor(this);
  }

  private void updateInOutlets()
  {
    if (ftsObject.getNumberOfInlets() > 1)
      inletDistance = (getWidth() - 2 * ErmesObjInOutlet.PAD) / (ftsObject.getNumberOfInlets() - 1);
    else
      inletDistance = 0; // not used in this case

    if (ftsObject.getNumberOfOutlets() > 1)
      outletDistance = (getWidth() - 2 * ErmesObjInOutlet.PAD) / (ftsObject.getNumberOfOutlets() - 1);
    else
      outletDistance = 0; // not used in this case
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves

  public void redefine( String text) 
  {
    updateInOutlets();
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
    return ftsObject;
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

    int firstLetPosition =  ErmesObjInOutlet.PAD  - ErmesObjInOutlet.WIDTH/2;;
    int distance = (getWidth() - 2*ErmesObjInOutlet.PAD) / (n-1); 
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
	    x < sensibilityStartX + 2*slice + ErmesObjInOutlet.WIDTH)
	  io = estimatedLet;

      }

    return io;
  }

  public DisplayObject getDisplayObjectAt( int mouseX, int mouseY)
  {
    int x = ftsObject.getX();
    int y = ftsObject.getY();
    int w = ftsObject.getWidth();
    int h = ftsObject.getHeight();

    if (! ((mouseX >= x) && (mouseX <= x + w) &&
	   (mouseY >= y) && (mouseY <= y + h)))
      return null;

    if ( mouseX >= x + w - HResizeSensibilityArea.width 
	 && mouseY > y + InletOutletSensibilityArea.height
	 && mouseY < y + h - InletOutletSensibilityArea.height)
      {
	hResizeArea.setObject(this);
	return hResizeArea;
      }
    else if ( mouseY < y + InletOutletSensibilityArea.height)
      {
	int inlet = findNearestInOutlet( mouseX, ftsObject.getNumberOfInlets());

	if (inlet >= 0)
	  {
	    inletArea.setNumber( inlet);
	    inletArea.setObject(this);
	    return inletArea;
	  }
      }
    else if (mouseY >= y + h - InletOutletSensibilityArea.height)
      {
	int outlet = findNearestInOutlet( mouseX, ftsObject.getNumberOfOutlets());

	if ( outlet >= 0)
	  {
	    outletArea.setNumber( outlet);
	    outletArea.setObject(this);
	    return outletArea;
	  }
      }

    // Try subclass specialized methods

    DisplayObject dobject = findSensibilityArea(mouseX, mouseY);

    if (dobject == null)
      return this;
    else
      return dobject;
  }

  protected DisplayObject findSensibilityArea( int mouseX, int mouseY)
  {
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

  public void moveBy( int dx, int dy) 
  {
    if (dx != 0)
      setX( ftsObject.getX() + dx);

    if (dy != 0)
      setY( ftsObject.getY() + dy);
  }

  // Get the bounds in a rectangle

  public void getBounds(Rectangle bounds) 
  {
    bounds.x = ftsObject.getX();
    bounds.y = (ftsObject.getY() - ErmesObjInOutlet.HIGHLIGHTED_HEIGHT +
		ErmesObjInOutlet.INLET_OFFSET + ErmesObjInOutlet.INLET_OVERLAP);
    bounds.width  = ftsObject.getWidth();
    bounds.height = (ftsObject.getHeight() + 2 * ErmesObjInOutlet.HIGHLIGHTED_HEIGHT  -
		     ErmesObjInOutlet.INLET_OFFSET - ErmesObjInOutlet.INLET_OVERLAP -
		     ErmesObjInOutlet.OUTLET_OFFSET - ErmesObjInOutlet.OUTLET_OVERLAP);
  }

  public final boolean intersects(Rectangle r)
  {
    return !((r.x + r.width <= ftsObject.getX()) ||
	     (r.y + r.height <= (ftsObject.getY() - ErmesObjInOutlet.HIGHLIGHTED_HEIGHT +
				 ErmesObjInOutlet.INLET_OFFSET + ErmesObjInOutlet.INLET_OVERLAP)) ||
	     (r.x >= ftsObject.getX() + ftsObject.getWidth()) ||
	     (r.y >= (ftsObject.getY() + ftsObject.getHeight() + ErmesObjInOutlet.HIGHLIGHTED_HEIGHT -
		      ErmesObjInOutlet.OUTLET_OFFSET - ErmesObjInOutlet.OUTLET_OVERLAP)));
  }


  public void rectangleUnion(Rectangle r)
  {
    SwingUtilities.computeUnion(ftsObject.getX(), ftsObject.getY(),
				ftsObject.getWidth(), ftsObject.getHeight(), r);
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

  // Called at ErmesObject disposal

  public void dispose()
  {
    ftsObject.setObjectListener(null);
  }


  public void showErrorDescription()
  {
    if ( ftsObject != null) 
      {
	int ax, ay, ah, aw;
	String annotation;
	String value;
	Graphics g;

	ftsObject.updateErrorDescription();
	Fts.sync();
	annotation = ftsObject.getErrorDescription();

	if (annotation != null)
	  {
	    ax = ftsObject.getX() + ftsObject.getWidth() / 2;
	    ay = ftsObject.getY() + ftsObject.getHeight() / 2;
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



