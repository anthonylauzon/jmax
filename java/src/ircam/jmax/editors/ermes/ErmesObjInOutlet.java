package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

import ircam.jmax.utils.*;

//
// The abstract base class for (object's) graphic inlet and outlet
//
abstract class ErmesObjInOutlet implements ErmesDrawable {

  protected static final int VISIBLE_WIDTH = 5;
//   protected static final int VISIBLE_HEIGHT = 2;
  protected static final int VISIBLE_HEIGHT = 3;
  protected static final int SENSIBLE_WIDTH = 10;
  protected static final int SENSIBLE_HEIGHT = 10;

  private final MaxVector itsConnections = new MaxVector();
  
  private ErmesObject itsOwner;

  private final Rectangle sensibleBounds = new Rectangle();

  protected int itsX;
  protected int itsY;

  private final Point itsAnchorPoint = new Point();

  private boolean selected;

  private int itsNum;

  // (fd) Used by ErmesSketchPad... and only there
  boolean itsAlreadyMoveIn;

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjInOutlet( int theNum, ErmesObject theOwner, int x, int y)
  {
    itsNum = theNum;
    itsOwner = theOwner;
    
    itsX = x;
    itsY = y;

    itsAlreadyMoveIn = false;
    selected = false;
    
    updateSensibleBounds();
  }

  void MoveBy( int theDeltaX, int theDeltaY) 
  {
    MoveTo( itsX + theDeltaX, itsY + theDeltaY);
  }
  
  void MoveTo( int theX, int theY) 
  {
    itsX = theX;
    itsY = theY;

    updateSensibleBounds();
  }

  void AddConnection( ErmesConnection theConnection)
  {
    itsConnections.addElement( theConnection);
  }

  // ----------------------------------------
  // Paint methods
  // ----------------------------------------
  public void Paint(Graphics g) 
  {
    if (g == null) 
      return;

    if (selected)
      g.setColor( Color.red); // (fd) should use global selection color
    else
      g.setColor( Color.black);

    g.fillRect( getVisibleX(), getVisibleY(), VISIBLE_WIDTH, VISIBLE_HEIGHT);
  }

  protected void DoublePaint()
  {
    Paint( itsOwner.itsSketchPad.getGraphics());

    if (itsOwner.itsSketchPad.offScreenPresent) 
      Paint( itsOwner.itsSketchPad.GetOffGraphics());
  }

  // ----------------------------------------
  // SensibleBounds property
  // ----------------------------------------
  Rectangle getSensibleBounds() 
  {
    return sensibleBounds;
  }

  private void updateSensibleBounds() 
  {
    sensibleBounds.setBounds( itsX, itsY, SENSIBLE_WIDTH, SENSIBLE_HEIGHT);
  }

  // ----------------------------------------
  // Change "selected" state
  // ----------------------------------------
  void setSelected( boolean selected)
  {
    this.selected = selected;

    DoublePaint();
  }
  
  boolean GetSelected()
  {
    return selected;
  }
  
  // ----------------------------------------
  // Accessor methods
  // ----------------------------------------
  ErmesObject GetOwner()
  {
    return itsOwner;
  }
  
  MaxVector GetConnections()
  {
    return itsConnections;
  }

  int getNum() 
  {
    return itsNum;
  }

  Point GetAnchorPoint() 
  {
    itsAnchorPoint.setLocation( getAnchorX(), getAnchorY());

    return itsAnchorPoint;
  }  

  // ----------------------------------------
  // Coordinates properties
  // ----------------------------------------
  protected final int getAnchorX()
  {
    return itsX + (SENSIBLE_WIDTH - VISIBLE_WIDTH) - 1;
  }

  protected abstract int getAnchorY();

  protected final int getVisibleX()
  {
    return itsX + (SENSIBLE_WIDTH - VISIBLE_WIDTH)/2;
  }

  protected abstract int getVisibleY();
}
