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


abstract class ErmesObject implements ErmesDrawable {
  public boolean itsSelected = false;
  public ErmesSketchPad itsSketchPad;
  public FtsObject itsFtsObject = null;
  public MaxVector itsInletList = new MaxVector();
  public MaxVector itsOutletList = new MaxVector();

  int itsInitX, itsInitY;
  boolean itsDragging = false;
  Font itsFont = null;
  FontMetrics itsFontMetrics = null;

  static Color itsUINormalColor = new Color( 153, 204, 255);
  static Color itsUISelectedColor = new Color( 51, 153, 204);
  static Color itsLangNormalColor = new Color( 153, 204, 204);
  static Color itsLangSelectedColor = new Color( 51, 153, 153);

  final static int PADS_DISTANCE = 12;
  static final int DRAG_DIMENSION = 4;

  private int itsX, itsY;
  private Rectangle currentRect = new Rectangle();

  // A Static method that work as a virtual constructor;
  // given an FTS object, build the proper FTS Object

  static ErmesObject makeErmesObject( ErmesSketchPad sketch, FtsObject object) 
  {
    ErmesObject aObject;
    String theName = object.getClassName();

    if ( theName.equals( "messbox"))
      aObject = new ErmesObjMessage( sketch, object);
    else if ( theName.equals( "button"))
      aObject = new ErmesObjBang( sketch, object);
    else if (theName.equals( "toggle"))
      aObject = new ErmesObjToggle( sketch, object);
    else if (theName.equals( "intbox"))
      aObject = new ErmesObjInt( sketch, object);
    else if (theName.equals( "floatbox"))
      aObject = new ErmesObjFloat( sketch, object);
    else if (theName.equals( "comment"))
      aObject = new ErmesObjComment( sketch, object);
    else if ( theName.equals( "slider"))
      aObject = new ErmesObjSlider( sketch, object);
    else if (theName.equals( "inlet"))
      aObject = new ErmesObjIn( sketch, object);
    else if (theName.equals( "outlet"))
      aObject = new ErmesObjOut( sketch, object);
    else if (theName.equals( "jpatcher"))
      aObject = new ErmesObjPatcher( sketch, object);
    else
      aObject = new ErmesObjExternal( sketch, object);

    aObject.Init();

    return aObject;
  }


  public ErmesObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super();

    itsSketchPad = theSketchPad;
    itsFtsObject = theFtsObject;
  }

  public int getItsX() 
  {
    return itsX;
  }

  public void setItsX( int theX) 
  {
    itsX = theX;
    itsFtsObject.put( "x", itsX);
  }

  public int getItsY() 
  {
    return itsY;
  }

  public void setItsY( int theY) 
  {
    itsY = theY;
    itsFtsObject.put( "y", itsY);
  }

  public int getItsWidth() 
  {
    if (currentRect == null)
      return 0;
    else
      return currentRect.width;
  }

  public void setItsWidth( int theWidth) 
  {
    currentRect.width = theWidth;
    itsFtsObject.put( "w", theWidth);
  }

  public int getItsHeight() 
  {
    if (currentRect != null)
      return currentRect.height;
    else
      return 0;
  }

  public void setItsHeight( int theHeight) 
  {
    currentRect.height = theHeight;
    itsFtsObject.put( "h", theHeight);
  }

  public Font getFont() 
  {
    return itsFont;
  }

  public void setFont( Font theFont) 
  {
    itsFont = theFont;
    itsFontMetrics = itsSketchPad.getFontMetrics( theFont);

    ResizeToNewFont( itsFont);

    itsFtsObject.put( "font", itsFont.getName());
    itsFtsObject.put( "fs", itsFont.getSize());
  }

  abstract Dimension getPreferredSize();
  abstract Dimension getMinimumSize();

  final ErmesSketchWindow GetSketchWindow() 
  {
    return (ErmesSketchWindow)(itsSketchPad.itsSketchWindow);
  }

  abstract protected void Paint_specific( Graphics g);

  public void MouseDown_specific( MouseEvent e, int x, int y)
  {
  }

  public void inspect() 
  {
  }

  void ResizeToNewFont( Font itsFont) 
  {
  }

  public void UpdateOnly( Graphics g) 
  {
    if ( !itsSketchPad.itsGraphicsOn)
      return;

    g.setColor( itsSketchPad.getBackground());
    g.fillRect( itsX, itsY, currentRect.width, currentRect.height);
  }

  public void Paint( Graphics g) 
  {
    if ( ! itsSketchPad.itsGraphicsOn)
      return;

    if ( itsSketchPad.isInGroup) 
      {
	//emergency situation: ignore the Graphics and paint offScreen
	Paint_specific( itsSketchPad.GetOffGraphics());
	itsSketchPad.drawPending = true;
      } else
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

  // This method is called during the inits
  public void update( FtsObject theFtsObject) 
  {
    int i;
    ErmesObjInlet aErmesObjInlet;
    ErmesObjOutlet aErmesObjOutlet;
    int aHDist;

    // retrieve the inlet, outlet informations
    if ( theFtsObject == null) 
      return;

    itsFtsObject = theFtsObject;
    int n_inlts = theFtsObject.getNumberOfInlets();
    int n_outlts = theFtsObject.getNumberOfOutlets();
    int in_local_distance = PADS_DISTANCE;
    int out_local_distance = PADS_DISTANCE;

    int old_ninlts = itsInletList.size(); //used in case of redefines
    int old_noutlts = itsOutletList.size(); //used in case of redefines

    Rectangle aRect = currentRect;

    int maxPads = (n_outlts > n_inlts) ? n_outlts : n_inlts;

    if ( maxPads * PADS_DISTANCE > aRect.width) 
      { //the pads are longer then the element
	reshape( aRect.x, aRect.y, maxPads*PADS_DISTANCE, aRect.height);
      }

    if (n_inlts > 1)
      in_local_distance = ( aRect.width-10)/(n_inlts-1) ;

    if (n_outlts > 1)
      out_local_distance = (aRect.width-10)/(n_outlts-1) ;

    if (n_inlts > old_ninlts) 
      { //we added inlets...
	for ( i=0; i< n_inlts; i++) 
	  {
	    if ( i<old_ninlts) 
	      {
		aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( i);
		aErmesObjInlet.MoveTo( itsX + 2 + i*in_local_distance,
				       aErmesObjInlet.itsY);
		itsSketchPad.markSketchAsDirty();
	      } 
	    else 
	      {
		if (this instanceof ircam.jmax.editors.ermes.ErmesObjOut)
		  aErmesObjInlet = new ErmesObjOutInlet( i, this, itsX + 2 + i*in_local_distance, itsY);
		else
		  aErmesObjInlet = new ErmesObjInlet( i, this, itsX + 2 + i*in_local_distance, itsY);

		itsInletList.addElement( aErmesObjInlet);
		itsSketchPad.AddInlet( aErmesObjInlet);
		itsSketchPad.addToDirtyInOutlets( aErmesObjInlet);
	      }
	  }
      } 
    else if ( n_inlts <= old_ninlts) 
      { //we reduced the number of inlets...
	if ( n_inlts>1)
	  aHDist = ( currentRect.width-10)/(n_inlts-1);
	else
	  aHDist=0;

	for ( i=0; i< old_ninlts; i++) 
	  {
	    if (i<n_inlts) 
	      {
		aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( i);
		aErmesObjInlet.MoveTo( itsX + 2 + i*aHDist, aErmesObjInlet.itsY);
		itsSketchPad.markSketchAsDirty();
	      } else 
		{
		  //erase the inlet, and the associated connections
		  aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt( itsInletList.size() - 1);
		  itsInletList.removeElementAt( itsInletList.size() - 1);
		  itsSketchPad.RemoveInlet( aErmesObjInlet);
		  itsSketchPad.markSketchAsDirty();
                }
	  }
      }

    if (n_outlts>old_noutlts) 
      { //we added outlets...
	if (n_outlts>1)
	  aHDist = (currentRect.width-10)/(n_outlts-1);
	else
	  aHDist=0;

	for ( i=old_noutlts; i< n_outlts; i++) 
	  {
	    if (this instanceof ircam.jmax.editors.ermes.ErmesObjIn)
	      aErmesObjOutlet = new ErmesObjInletOutlet( i, this, itsX, itsY);
	    else
	      aErmesObjOutlet = new ErmesObjOutlet( i, this, itsX, itsY);
	    itsOutletList.addElement( aErmesObjOutlet);
	    itsSketchPad.AddOutlet( aErmesObjOutlet);
	    itsSketchPad.addToDirtyInOutlets( aErmesObjOutlet);
	  }

	for ( i=0; i<itsOutletList.size(); i++) 
	  {
	    aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt( i);
	    aErmesObjOutlet.MoveTo( itsX + 2 + i*aHDist, aErmesObjOutlet.itsY);
	    if (old_noutlts > 0 && old_noutlts != n_outlts)
	      itsSketchPad.markSketchAsDirty();
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
	    itsSketchPad.markSketchAsDirty();
	  }
	if (n_outlts>1)
	  aHDist = (currentRect.width-10)/(n_outlts-1);
	else
	  aHDist = 0;

	for ( i = 0; i < n_outlts;i++) 
	  {
	    aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt( i);
	    aErmesObjOutlet.MoveTo( itsX + 2 + i*aHDist, aErmesObjOutlet.itsY);
	    itsSketchPad.markSketchAsDirty();
	  }
      }

  }

  public boolean isUIController() 
  {
    return false;
  }

  // redefineFtsObject provide a default empty implementation
  // for the object that do not redefine themselves
  public void redefineFtsObject() 
  {
  }

  public void Init() 
  {
    String aFont = null;
    Integer  aSize = null;
    Object value;

    value = itsFtsObject.get( "font");

    if (value instanceof String)
      aFont = (String)itsFtsObject.get( "font");

    value = itsFtsObject.get( "fs");
    if (value instanceof Integer)
      aSize = (Integer)itsFtsObject.get( "fs");

    itsSelected = false;
    makeCurrentRect( itsFtsObject);

    if ( (aFont == null) && (aSize != null)) 
      {
	int fontSize = aSize.intValue();

	itsFont = FontCache.lookupFont( fontSize );
	itsFontMetrics = FontCache.lookupFontMetrics( fontSize);
      } 
    else if ( (aFont == null) && (aSize == null)) 
      {
	setFont( itsSketchPad.sketchFont);
      } 
    else 
      {
	int aIntSize;

	if (aSize == null)
	  aIntSize = itsSketchPad.sketchFont.getSize();
	else
	  aIntSize = aSize.intValue();

	setFont( new Font( aFont,
			   itsSketchPad.sketchFont.getStyle(), aIntSize));
      }

    update( itsFtsObject);
  }

  protected void makeCurrentRect( FtsObject theFtsObject) 
  {
    int width = 0;
    int height = 0;

    itsX = ((Integer)theFtsObject.get( "x")).intValue();
    itsY = ((Integer)theFtsObject.get( "y")).intValue();

    Integer widthInt = (Integer) theFtsObject.get( "w");
    if (widthInt != null)
      width = widthInt.intValue();

    Integer heightInt = (Integer)theFtsObject.get( "h");
    if (heightInt != null)
      height = heightInt.intValue();

    if ( width < 10) 
      {
	width = getPreferredSize().width;
	theFtsObject.put( "w", width);
      }

    if ( height < 10) 
      {
	height = getPreferredSize().height;
	theFtsObject.put( "h", height);
      }

    currentRect = new Rectangle( itsX, itsY, width, height);
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

  public void Deselect( boolean PaintNow) 
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

  public FtsObject GetFtsObject() 
  {
    return itsFtsObject;
  }

  public MaxVector GetOutletList() 
  {
    return itsOutletList;
  }

  public MaxVector GetInletList() 
  {
    return itsInletList;
  }

  public ErmesSketchPad GetSketchPad() 
  {
    return itsSketchPad;
  }

  public void ConnectionRequested( ErmesObjInOutlet theRequester) 
  {
    // HERE the checking: is the type of connection requested allowed?
    if (!theRequester.IsInlet()) //if is an outlet...
      itsSketchPad.OutletConnect( this, theRequester);
    else
      itsSketchPad.InletConnect( this, theRequester); // then, is it's an inlet
  }

  public boolean ConnectionAbort( ErmesObjInOutlet theRequester, boolean paintNow) 
  {
    // HERE the checking: is the type of connection abort allowed?
    // ( for now always allowed)
    theRequester.ChangeState( false, theRequester.connected, paintNow);
    itsSketchPad.ResetConnect();
    return true; //for now, everything is allowed
  }

  public boolean MouseMove( MouseEvent e,int x,int y) 
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
      } else
	return false;
  }

  public boolean MouseDrag( MouseEvent e,int x,int y) 
  {
    return MouseDrag_specific( e, x, y);
  }

  public boolean IsInDragBox( int x,int y) 
  {
    return x > (currentRect.x + currentRect.width - DRAG_DIMENSION)
      && x < (currentRect.x + currentRect.width) 
      && y > (currentRect.y + currentRect.height - DRAG_DIMENSION) 
      && y < (currentRect.y + currentRect.height);
  }

  public void MouseDown( MouseEvent e,int x, int y) 
  {
    if ( !itsSketchPad.itsRunMode && !e.isControlDown())
      {
	if ( (e.getClickCount() > 1) && e.isShiftDown() ) 
	  {
	    RestoreDimensions( true);
	  } else if ( itsSelected)
	    itsSketchPad.clickHappenedOnAnAlreadySelected =true;
	else
	  itsSketchPad.clickHappenedOnAnAlreadySelected =false;
	if ( IsInDragBox( x,y)) 
	  {
	    SetInitDrag( x,y);
	  } 
	else
	  MouseDown_specific( e, x, y);
      } 
    else
      MouseDown_specific( e, x, y);
  }

  public boolean MouseUp_specific( MouseEvent e, int x, int y) 
  {
    return false;
  }
  public boolean MouseDrag_specific( MouseEvent e, int x, int y) 
  {
    return false;
  }

  public boolean MouseUp( MouseEvent e,int x,int y) 
  {
    if ( itsSketchPad.itsRunMode || e.isControlDown())
      return MouseUp_specific( e, x, y);

    if ( itsDragging) 
      {
	if ( itsSketchPad.itsResizeMode == itsSketchPad.BOTH_RESIZING) 
	  {
	    int aWidth, aHeight;
	    boolean wrongWidth = java.lang.Math.abs( x - currentRect.x) <  getMinimumSize().width 
	      || ( x < currentRect.x);
	    boolean wrongHeight = (java.lang.Math.abs(y-currentRect.y) < getMinimumSize().height) 
	      || (y < currentRect.y);

	    if (wrongWidth && wrongHeight)
	      RestoreDimensions( true);
	    else if (wrongWidth) 
	      {
		resizeBy( getMinimumSize().width - currentRect.width, y - itsInitY);
		itsSketchPad.repaint();
	      } else if ( wrongHeight) 
		{
		  resizeBy( x - itsInitX, getMinimumSize().height - currentRect.height);
		  itsSketchPad.repaint();
                } 
	    else 
	      {
		resizeBy( x - itsInitX + 1, y - itsInitY + 1);
	      }

	  }
	else if ( itsSketchPad.itsResizeMode == itsSketchPad.HORIZONTAL_RESIZING) 
	  {
	    if ( canResizeBy( x - itsInitX, 0))
	      resizeBy( x - itsInitX, 0);
	    else
	      ResizeToText( x - itsInitX, 0);
	  }
	else if ( itsSketchPad.itsResizeMode == itsSketchPad.VERTICAL_RESIZING) 
	  {
	    if ( canResizeBy( 0, y - itsInitY))
	      resizeBy( 0, y - itsInitY);
	    else
	      ResizeToText( 0, y - itsInitY);
	  }
	itsDragging = false;
      }
    return false;
  }

  public void RestoreDimensions( boolean paintNow) 
  {
    //possible optimization: don't repaint if nothing changes
    resizeBy( getMinimumSize().width - currentRect.width, getMinimumSize().height - currentRect.height);
    if ( paintNow)
      itsSketchPad.repaint();
    else
      itsSketchPad.addToDirtyObjects( this);
  }

  public void ResizeToText( int theDeltaX, int theDeltaY) 
  {
    int aWidth = currentRect.width + theDeltaX;
    int aHeight = currentRect.height + theDeltaY;
    if ( aWidth < getMinimumSize().width)
      aWidth = getMinimumSize().width;
    if ( aHeight < getMinimumSize().height)
      aHeight = getMinimumSize().height;
    resizeBy( aWidth - currentRect.width, aHeight - currentRect.height);
  };

  public boolean canResizeBy( int theDeltaX, int theDeltaY) 
  {
    return (getItsWidth() + theDeltaX >= getMinimumSize().width)
      && (getItsHeight() + theDeltaY >= getMinimumSize().height);
  }

  public void SetInitDrag( int theX, int theY) 
  {
    itsInitX = currentRect.x + currentRect.width;
    itsInitY = currentRect.y + currentRect.height;
    itsDragging = true;
    itsSketchPad.SetResizeState( this);
  }

  public void MoveBy( int theDeltaH, int theDeltaV) 
  {
    int j;
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;

    if ( theDeltaH == 0 && theDeltaV == 0)
      return;

    setItsX( itsX + theDeltaH);
    setItsY( itsY + theDeltaV);
    currentRect.x = itsX;
    currentRect.y = itsY;

    for ( Enumeration e1 = itsInletList.elements(); e1.hasMoreElements(); ) 
      {
	aInlet = ( ErmesObjInlet) e1.nextElement();
	aInlet.MoveBy( theDeltaH, theDeltaV);
      }
    for ( Enumeration e2 = itsOutletList.elements(); e2.hasMoreElements(); ) 
      {
	aOutlet = ( ErmesObjOutlet) e2.nextElement();
	aOutlet.MoveBy( theDeltaH, theDeltaV);
      }
  }

  public void resizeBy( int theDeltaH, int theDeltaV) 
  {
    if ( theDeltaH == 0 && theDeltaV == 0)
      return;

    if ( -theDeltaH > currentRect.width || -theDeltaV > currentRect.height)
      return;

    setItsWidth( currentRect.width + theDeltaH);
    setItsHeight( currentRect.height + theDeltaV);

    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    int aInletDistance;

    if ( itsInletList.size() > 1) 
      {
	aInletDistance = (currentRect.width - 10) / (itsInletList.size() - 1);

	for ( int i = 1; i < itsInletList.size(); i++)
	  {
	    aInlet = (ErmesObjInlet) itsInletList.elementAt( i);
	    aInlet.MoveTo( itsX + 2 + i*aInletDistance, aInlet.itsY);
	  }
      }

    int aHDistance;

    if ( itsOutletList.size() > 1)
      aHDistance = (currentRect.width - 10) / (itsOutletList.size() - 1);
    else
      aHDistance = 0;

    for ( int j = 0; j < itsOutletList.size(); j++) 
      {
	aOutlet = (ErmesObjOutlet) itsOutletList.elementAt( j);
	aOutlet.MoveTo( itsX + 2 + j*aHDistance, aOutlet.itsY + theDeltaV);
      }
  }
  public void resize( int w, int h) 
  {
    setItsWidth( w);
    setItsHeight( h);
  }

  public void reshape( int x, int y, int width, int height) 
  {
    setItsX( x);
    setItsY( y);
    currentRect.x = itsX;
    currentRect.y = itsY;
    resize( width, height);
  }

  final Rectangle Bounds() 
  {
    // (fd, mdc) Bounds don't copy the bound rectangle any more
    // and nobody is allowed to modify it.
    return currentRect;
  }

  public Dimension Size() 
  {
    return ( new Dimension( currentRect.width, currentRect.height));
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
    currentRect = null;
  }

  // Experimental MDC
  public void showAnnotation( String property) 
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
	value= itsFtsObject.get( property);

	if ( value != null)
	  {
	    annotation = value.toString();
	    ax = currentRect.x + currentRect.width / 2;
	    ay = currentRect.y + currentRect.height / 2;
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
