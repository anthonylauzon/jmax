package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

import ircam.jmax.utils.*;

//
// The abstract base class for (object's) graphic inlet and outlets
//
abstract public class ErmesObjInOutlet implements ErmesDrawable {
  public MaxVector itsConnections;
  
  ErmesObject itsOwner;
  boolean itsAlreadyMoveIn = false;
  Rectangle currentBounds = new Rectangle();
  Rectangle currentSensibleBounds = new Rectangle();
  Point itsAnchorPoint = new Point();

  protected final int UPDATE_AND_DRAW = 0;
  protected final int DRAW = 1;

  protected boolean selected = false;
  protected boolean connected = false; //ignore the connected flag
  protected int itsX, itsY;
  protected int itsCurrentDrawingMethod = DRAW;

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjInOutlet( ErmesObject theOwner, int x_coord, int y_coord)
  {
    itsConnections = new MaxVector();
    itsX = x_coord;
    itsY = y_coord;

    itsOwner = theOwner;
    selected = false;
    connected = false;
    preferredSize = new Dimension(7,9);

    if (IsInlet())
      itsY -= 9;
    else
      itsY += itsOwner.getItsHeight();

    recomputeBounds();
    updateAnchorPoint();
  }

  public void Paint(Graphics g) 
  {
    if (g== null) 
      return;
    if ( itsCurrentDrawingMethod == DRAW) 
      Paint_specific(g);
    else 
      Update(g);
  }

  abstract protected void Paint_specific(Graphics g);

  protected void DoublePaint() {
    Graphics aGraphics = itsOwner.itsSketchPad.getGraphics();
    if (aGraphics != null) 
      {
	Paint(aGraphics);	
      }
    
    if (itsOwner.itsSketchPad.offScreenPresent) 
      Paint(itsOwner.itsSketchPad.GetOffGraphics());
  }

  void Repaint(boolean paintNow) 
  {
    ErmesSketchPad mySketch = itsOwner.itsSketchPad;

    if (paintNow) 
      {
	Graphics g = mySketch.getGraphics();
	Update(g);
	g.dispose();

	Update(mySketch.offGraphics);

	for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) 
	  {
	    ((ErmesConnection)e.nextElement()).DoublePaint();
	  }
	mySketch.removeDirtyInOutlet(this);
      }
    else 
      {
	mySketch.addToDirtyInOutlets(this);
	for (Enumeration e = itsConnections.elements(); e.hasMoreElements();) 
	  {
	    mySketch.addToDirtyConnections((ErmesConnection)e.nextElement());
	  }
      }
  }

  public void Update(Graphics g) 
  {
    int aHeight;

    if( !itsOwner.itsSketchPad.itsGraphicsOn || g == null) 
      return;
    g.setColor( itsOwner.itsSketchPad.getBackground());
    if ( IsInlet())
      aHeight = preferredSize.height;
    else
      aHeight = preferredSize.height+1;

    g.fillRect( itsX, itsY, preferredSize.width, aHeight);

    Paint_specific(g);
  }
    
  final Rectangle Bounds()
  {
    return currentBounds;
  }
  
  final Rectangle getSensibleBounds() 
  {
    return currentSensibleBounds;
  }

  private final void recomputeBounds() 
  {
    if (IsInlet())
      currentSensibleBounds.setBounds( itsX-2, itsY+4, preferredSize.width+4, preferredSize.height);
    else 
      currentSensibleBounds.setBounds( itsX-2, itsY-4,preferredSize.width+4, preferredSize.height);

    currentBounds.setBounds( itsX, itsY, preferredSize.width, preferredSize.height);

  }

  public void SetConnected( boolean theConnected, boolean paintNow)
  {
    ChangeState( selected, theConnected, paintNow);
  }
  
  public int GetItsX()
  {
    return itsX; 
  }
	
  public int GetItsY()
  {
    return itsY; 
  }
  
  public boolean GetConnected()
  {
    return connected; 
  }
	
  public boolean GetSelected()
  {
    return selected;
  }
  
  public ErmesObject GetOwner(){
    return itsOwner;
  }
  
  public MaxVector GetConnections()
  {
    return itsConnections;
  }
  
  abstract boolean IsInlet();
  
  Point GetAnchorPoint() 
  {
    return itsAnchorPoint;
  }  
  

  public void ChangeState( boolean theSelState, boolean theConState, boolean paintNow)
  {
    ErmesSketchPad mySketch = itsOwner.itsSketchPad;
    boolean toPaint = false;

    if (selected != theSelState) 
      {
	toPaint = true;
	itsCurrentDrawingMethod = UPDATE_AND_DRAW;
	selected = theSelState;
      }

    connected = theConState;

    if (toPaint) 
      {
	Graphics g = mySketch.getGraphics();
	Paint(g);
	g.dispose();

	Paint(mySketch.offGraphics);
	for ( Enumeration e = itsConnections.elements(); e.hasMoreElements(); ) 
	  {
	    ((ErmesConnection)e.nextElement()).DoublePaint();	  
	  }
      }
  }
  
  public void MoveBy(int theDeltaX, int theDeltaY) 
  {
    MoveTo( itsX + theDeltaX, itsY += theDeltaY);
  }
  
  public void MoveTo(int theX, int theY) 
  {
    itsX = theX; 
    itsY = theY;
    recomputeBounds();
    updateAnchorPoint();
    itsCurrentDrawingMethod = DRAW;
  }
  
  public void AddConnection(ErmesConnection theConnection)
  {
    itsConnections.addElement( theConnection);
  }
	
  abstract void updateAnchorPoint();
	
  public Dimension getMinimumSize() 
  {
    return getPreferredSize();
  }
  
  static Dimension preferredSize;

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
