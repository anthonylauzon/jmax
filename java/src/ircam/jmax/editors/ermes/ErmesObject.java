package ircam.jmax.editors.frobber;

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

abstract class ErmesObject implements ErmesDrawable {

  ErmesSketchPad itsSketchPad;

  FtsObject itsFtsObject = null;

  MaxVector itsInletList = new MaxVector();
  MaxVector itsOutletList = new MaxVector();

  boolean itsSelected = false;
  boolean updated = false;

  Font itsFont = null;
  FontMetrics itsFontMetrics = null;

  static Color itsUINormalColor = new Color( 153, 204, 255);
  static Color itsUISelectedColor = new Color( 51, 153, 204);
  static Color itsLangNormalColor = new Color( 153, 204, 204);
  static Color itsLangSelectedColor = new Color( 51, 153, 153);

  final static int PADS_DISTANCE = 12;
  static final int DRAG_DIMENSION = 4;

  private Rectangle itsRectangle = new Rectangle();

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

    updateInOutlets();
  }

  protected final int getX() 
  {
    return itsRectangle.x;
  }

  protected void setX( int theX) 
  {
    itsRectangle.x = theX;
    itsFtsObject.put( "x", itsRectangle.x);
  }

  protected final int getY() 
  {
    return itsRectangle.y;
  }

  protected void setY( int theY) 
  {
    itsRectangle.y = theY;
    itsFtsObject.put( "y", itsRectangle.y);
  }

  protected final int getWidth() 
  {
    return itsRectangle.width;
  }

  protected void setWidth( int theWidth) 
  {
    itsRectangle.width = theWidth;
    itsFtsObject.put( "w", theWidth);
  }

  protected final int getHeight() 
  {
    return itsRectangle.height;
  }

  protected void setHeight( int theHeight) 
  {
    itsRectangle.height = theHeight;
    itsFtsObject.put( "h", theHeight);
  }

  Font getFont() 
  {
    return itsFont;
  }

  void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    //ResizeToNewFont( itsFont);

    itsFtsObject.put( "font", itsFont.getName());
    itsFtsObject.put( "fs", itsFont.getSize());
  }

  abstract Dimension getMinimumSize();

  final ErmesSketchWindow GetSketchWindow() 
  {
    return (ErmesSketchWindow)(itsSketchPad.itsSketchWindow);
  }

  abstract protected void Paint_specific( Graphics g);

  void MouseDown_specific( MouseEvent e, int x, int y)
  {
  }

  void inspect() 
  {
  }

  void ResizeToNewFont( Font itsFont) 
  {
  }

  void UpdateOnly( Graphics g) 
  {
    g.setColor( itsSketchPad.getBackground());
    g.fillRect( itsRectangle.x, itsRectangle.y, itsRectangle.width, itsRectangle.height);
  }

  public void Paint( Graphics g)  // (fd) public, because public in ErmesDrawable...
  {
    Paint_specific( g);
  }

  public void Repaint()
  {
    itsSketchPad.repaint( itsRectangle.x, itsRectangle.y, itsRectangle.width, itsRectangle.height);
  }

  protected void DoublePaint() 
  {
    //this double paint is usefull when an object change its state in run mode
    Graphics aGraphics = itsSketchPad.getGraphics();

    if ( aGraphics != null) 
      {
	Paint( aGraphics);
      }
  }

  void updateInOutlets()
  {
    int i;
    ErmesObjInlet aErmesObjInlet;
    ErmesObjOutlet aErmesObjOutlet;
    int aHDist;

    // retrieve the inlet, outlet informations
    if ( itsFtsObject == null) 
      return;

    int n_inlts = itsFtsObject.getNumberOfInlets();
    int n_outlts = itsFtsObject.getNumberOfOutlets();
    int in_local_distance = PADS_DISTANCE;
    int out_local_distance = PADS_DISTANCE;

    int old_ninlts = itsInletList.size(); //used in case of redefines
    int old_noutlts = itsOutletList.size(); //used in case of redefines

    int maxPads = (n_outlts > n_inlts) ? n_outlts : n_inlts;

//     if ( maxPads * PADS_DISTANCE > itsRectangle.width) 
//       { //the pads are longer then the element
// 	resize( maxPads*PADS_DISTANCE, itsRectangle.height);
//       }

    if (n_inlts > 1)
      in_local_distance = ( itsRectangle.width-10)/(n_inlts-1) ;

    if (n_outlts > 1)
      out_local_distance = (itsRectangle.width-10)/(n_outlts-1) ;

    if (n_inlts > old_ninlts) 
      { //we added inlets...
	for ( i=0; i< n_inlts; i++) 
	  {
	    if ( i<old_ninlts) 
	      {
		aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( i);
		aErmesObjInlet.MoveTo( itsRectangle.x + 2 + i*in_local_distance, aErmesObjInlet.itsY);
	      } 
	    else 
	      {
		aErmesObjInlet = new ErmesObjInlet( i, this, itsRectangle.x + 2 + i*in_local_distance, itsRectangle.y);

		itsInletList.addElement( aErmesObjInlet);
		itsSketchPad.AddInlet( aErmesObjInlet);
	      }
	  }
      } 
    else if ( n_inlts <= old_ninlts) 
      { //we reduced the number of inlets...
	if ( n_inlts>1)
	  aHDist = ( itsRectangle.width-10)/(n_inlts-1);
	else
	  aHDist=0;

	for ( i=0; i< old_ninlts; i++) 
	  {
	    if (i<n_inlts) 
	      {
		aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( i);
		aErmesObjInlet.MoveTo( itsRectangle.x + 2 + i*aHDist, aErmesObjInlet.itsY);
	      } else 
		{
		  //erase the inlet, and the associated connections
		  aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( itsInletList.size() - 1);
		  itsInletList.removeElementAt( itsInletList.size() - 1);
		  itsSketchPad.RemoveInlet( aErmesObjInlet);
                }
	  }
      }

    if (n_outlts>old_noutlts) 
      { //we added outlets...
	if (n_outlts>1)
	  aHDist = (itsRectangle.width-10)/(n_outlts-1);
	else
	  aHDist=0;

	for ( i = old_noutlts; i< n_outlts; i++) 
	  {
	    aErmesObjOutlet = new ErmesObjOutlet( i, this, itsRectangle.x, itsRectangle.y);
	    itsOutletList.addElement( aErmesObjOutlet);
	    itsSketchPad.AddOutlet( aErmesObjOutlet);
	  }

	for ( i = 0; i < itsOutletList.size(); i++) 
	  {
	    aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt( i);
	    aErmesObjOutlet.MoveTo( itsRectangle.x + 2 + i*aHDist, aErmesObjOutlet.itsY);
	  }
      }
    else if (n_outlts <= old_noutlts) 
      { //we reduced the number of outlets
	int size = itsOutletList.size()-1;
	for ( i=n_outlts;i<old_noutlts;i++) 
	  {
	    aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt( size);
	    itsOutletList.removeElementAt( size);
	    itsSketchPad.RemoveOutlet( aErmesObjOutlet);
	    size--;
	  }
	if (n_outlts>1)
	  aHDist = (itsRectangle.width-10)/(n_outlts-1);
	else
	  aHDist = 0;

	for ( i = 0; i < n_outlts;i++) 
	  {
	    aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt( i);
	    aErmesObjOutlet.MoveTo( itsRectangle.x + 2 + i*aHDist, aErmesObjOutlet.itsY);
	  }
      }
  }

  boolean isUIController() 
  {
    return false;
  }

  // redefine provide a default empty implementation
  // for the object that do not redefine themselves
  void redefine( String text) 
  {
  }

  void Select()
  {
    if ( !itsSelected) 
      itsSelected = true;
  }

  void Deselect() 
  {
    if (itsSelected) 
      itsSelected = false;
  }

  FtsObject GetFtsObject() 
  {
    return itsFtsObject;
  }

  MaxVector GetOutletList() 
  {
    return itsOutletList;
  }

  MaxVector GetInletList() 
  {
    return itsInletList;
  }

  ErmesSketchPad GetSketchPad() 
  {
    return itsSketchPad;
  }

  void ConnectionRequested( ErmesObjInOutlet theRequester) 
  {
    // HERE the checking: is the type of connection requested allowed?
    if ( theRequester instanceof ErmesObjOutlet) //if is an outlet...
      itsSketchPad.OutletConnect( this, theRequester);
    else
      itsSketchPad.InletConnect( this, theRequester); // then, is it's an inlet
  }

  boolean ConnectionAbort( ErmesObjInOutlet theRequester) 
  {
    // HERE the checking: is the type of connection abort allowed?
    // ( for now always allowed)
    theRequester.setSelected( false);
    itsSketchPad.ResetConnect();
    return true; //for now, everything is allowed
  }

  boolean MouseMove( MouseEvent e, int x, int y) 
  {
    if ( itsSketchPad.itsRunMode)
      return false;
    else if ( e.isControlDown()) 
      {
	if ( isUIController()) 
	  {
	    if ( itsSketchPad.itsSketchWindow.getCursor() != Cursor.getPredefinedCursor( Cursor.HAND_CURSOR))
	      itsSketchPad.itsSketchWindow.setCursor( Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
	  }
	return true;
      } 
    else if ( IsInDragBox( x,y)) 
      {
	GetSketchWindow().setCursor( Cursor.getPredefinedCursor( Cursor.SE_RESIZE_CURSOR));
	return true;
      } 
    else
      return false;
  }

  boolean MouseDrag( MouseEvent e,int x,int y) 
  {
    return MouseDrag_specific( e, x, y);
  }

  boolean IsInDragBox( int x,int y) 
  {
    return x > (itsRectangle.x + itsRectangle.width - DRAG_DIMENSION)
      && x < (itsRectangle.x + itsRectangle.width) 
      && y > (itsRectangle.y + itsRectangle.height - DRAG_DIMENSION) 
      && y < (itsRectangle.y + itsRectangle.height);
  }

  void MouseDown( MouseEvent e,int x, int y) 
  {
    if ( !itsSketchPad.itsRunMode && !e.isControlDown())
      {
	if ( (e.getClickCount() > 1) && e.isShiftDown() ) 
	  {
	    RestoreDimensions();
	    Repaint();
	  } 
	else if ( itsSelected)
	  itsSketchPad.clickHappenedOnAnAlreadySelected = true;
	else
	  itsSketchPad.clickHappenedOnAnAlreadySelected = false;

	if ( IsInDragBox( x,y)) 
	  SetInitDrag();
	else
	  MouseDown_specific( e, x, y);
      } 
    else
      MouseDown_specific( e, x, y);
  }

  void MouseUp_specific( MouseEvent e, int x, int y) 
  {
  }

  boolean MouseDrag_specific( MouseEvent e, int x, int y) 
  {
    return false;
  }

  void MouseUp( MouseEvent e,int x,int y) 
  {
    if ( itsSketchPad.itsRunMode || e.isControlDown())
      MouseUp_specific( e, x, y);
  }

  void RestoreDimensions( ) 
  {
    //possible optimization: don't repaint if nothing changes

    resizeBy( getMinimumSize().width - itsRectangle.width, getMinimumSize().height - itsRectangle.height);
  }

  void ResizeToText( int theDeltaX, int theDeltaY) 
  {
    int aWidth = itsRectangle.width + theDeltaX;
    int aHeight = itsRectangle.height + theDeltaY;
    if ( aWidth < getMinimumSize().width)
      aWidth = getMinimumSize().width;
    if ( aHeight < getMinimumSize().height)
      aHeight = getMinimumSize().height;
    resizeBy( aWidth - itsRectangle.width, aHeight - itsRectangle.height);
  };

  boolean canResizeBy( int theDeltaX, int theDeltaY) 
  {
    return (getWidth() + theDeltaX >= getMinimumSize().width)
      && (getHeight() + theDeltaY >= getMinimumSize().height);
  }

  void SetInitDrag() 
  {
    itsSketchPad.SetResizeState( this);
  }

  void MoveBy( int theDeltaH, int theDeltaV) 
  {
    if ( theDeltaH == 0 && theDeltaV == 0)
      return;

    setX( itsRectangle.x + theDeltaH);
    setY( itsRectangle.y + theDeltaV);
    itsRectangle.x = itsRectangle.x;
    itsRectangle.y = itsRectangle.y;

    for ( Enumeration e1 = itsInletList.elements(); e1.hasMoreElements(); ) 
      {
	ErmesObjInlet aInlet = ( ErmesObjInlet) e1.nextElement();
	aInlet.MoveBy( theDeltaH, theDeltaV);
      }

    for ( Enumeration e2 = itsOutletList.elements(); e2.hasMoreElements(); ) 
      {
	ErmesObjOutlet aOutlet = ( ErmesObjOutlet) e2.nextElement();
	aOutlet.MoveBy( theDeltaH, theDeltaV);
      }
  }

  protected void recomputeInOutletsPositions()
  {
    if ( itsInletList.size() > 1) 
      {
	int aInletDistance = (itsRectangle.width - 10) / (itsInletList.size() - 1);

	for ( int i = 1; i < itsInletList.size(); i++)
	  {
	    ErmesObjInlet aInlet = (ErmesObjInlet) itsInletList.elementAt( i);
	    aInlet.MoveTo( getX() + 2 + i*aInletDistance, aInlet.itsY);
	  }
      }

    int aHDistance;

    if ( itsOutletList.size() > 1)
      aHDistance = (itsRectangle.width - 10) / (itsOutletList.size() - 1);
    else
      aHDistance = 0;

    for ( int j = 0; j < itsOutletList.size(); j++) 
      {
	ErmesObjOutlet aOutlet = (ErmesObjOutlet) itsOutletList.elementAt( j);
	// big shit...
	aOutlet.MoveTo( getX() + 2 + j*aHDistance, getY() + getHeight() - 2);
      }
  }

  void resizeBy( int theDeltaW, int theDeltaH) 
  {
    if ( theDeltaW == 0 && theDeltaH == 0)
      return;

    if ( -theDeltaW > itsRectangle.width || -theDeltaH > itsRectangle.height)
      return;

    setWidth( itsRectangle.width + theDeltaW);
    setHeight( itsRectangle.height + theDeltaH);

    recomputeInOutletsPositions();
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
    itsInletList = null;
    itsOutletList = null;
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
