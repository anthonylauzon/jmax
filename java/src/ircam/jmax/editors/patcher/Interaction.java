package ircam.jmax.editors.patcher;

import java.awt.*; 
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;

import javax.swing.*; 

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;

/** This class implement the interaction machine for the patcher
  editor; it is keylistener and mouse listener for the sketch and
  the window, and implements the whole state machine.
  */

class Interaction  implements MouseMotionListener, MouseListener, KeyListener
{
  ErmesSketchPad sketch;
  DisplayList    displayList;
  ErmesToolBar   toolBar;
  KeyEventClient keyEventClient = null;

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Constructors                                             //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  Interaction(ErmesSketchPad sketch, DisplayList displayList)
  {
    this.sketch      = sketch;
    this.displayList = displayList;

    itsStartMovingPt = new Point( 0,0);    
    itsStartInclusionRect = new Rectangle();  
  }

  final void setToolBar( ErmesToolBar toolBar)
  {
    this.toolBar     = toolBar;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Utilities                                                //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private Rectangle normalizedRect = new Rectangle();  

  private final Rectangle NormalizedRect(Rectangle theRect) 
  {
    normalizedRect.x = (theRect.width < 0)? theRect.x + theRect.width: theRect.x;
    normalizedRect.y = (theRect.height < 0)? theRect.y +theRect.height: theRect.y;
    normalizedRect.width = (theRect.width < 0)? - theRect.width: theRect.width;
    normalizedRect.height = (theRect.height < 0)? - theRect.height: theRect.height;
    
    return normalizedRect;
  }
  
  private void prepareForDynamicConnect( int outletNumber) 
  {
    startConnectPoint.setLocation( itsPotentialConnectionFromObject.getConnectionStartX( outletNumber),
				   itsPotentialConnectionFromObject.getConnectionStartY( outletNumber));

    currentConnectPoint.setLocation( startConnectPoint);
    previousConnectPoint.setLocation( startConnectPoint);
    sketch.setCursor( Cursor.getDefaultCursor());
  }

  void DynamicScrolling( int theX, int theY)
  {
//     Adjustable aHAdjustable = sketch.itsSketchWindow.itsScrollerView.getHAdjustable();
//     Adjustable aVAdjustable = sketch.itsSketchWindow.itsScrollerView.getVAdjustable();
//     boolean hasScrolled = false;

//     if ( theX >= aHAdjustable.getVisibleAmount() + aHAdjustable.getValue())
//       {
// 	hasScrolled = true;
// 	eraseShadows();
// 	aHAdjustable.setValue( aHAdjustable.getValue() + aHAdjustable.getUnitIncrement());
//       }

//     if ( theY >= aVAdjustable.getVisibleAmount() + aVAdjustable.getValue())
//       {
// 	hasScrolled = true;
// 	eraseShadows();
// 	aVAdjustable.setValue( aVAdjustable.getValue() + aVAdjustable.getUnitIncrement());
//       }

//     if ( theX <= aHAdjustable.getValue())
//       {
// 	hasScrolled = true;
// 	eraseShadows();
// 	aHAdjustable.setValue( aHAdjustable.getValue() - aHAdjustable.getUnitIncrement());
//       }

//     if ( theY <= aVAdjustable.getValue())
//       {
// 	hasScrolled = true;
// 	eraseShadows();
// 	aVAdjustable.setValue( aVAdjustable.getValue() - aVAdjustable.getUnitIncrement());
//       }
  }
  
  private void eraseShadows()
  {
    sketch.redraw();
    erased = true;
    erased1 = true;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   State Machine Support                                    //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private final static int DOING_NOTHING = 0;		
  private final static int START_SELECT  = 2;
  private final static int AREA_SELECT 	 = 3;		
  private final static int MOVING 	 = 4;
  private final static int MOVINGSEGMENT    = 5;	
  private final static int H_RESIZING_OBJECT = 6;
  private final static int V_RESIZING_OBJECT = 60;
  private final static int EDITING_OBJECT   = 7;
  private final static int START_CONNECT = 26;

  private Rectangle currentRect = new Rectangle();
  private Rectangle previousRect = new Rectangle();
  private Point	    currentPoint = new Point();
  private Rectangle currentResizeRect = new Rectangle();
  private Rectangle previousResizeRect = new Rectangle(); 

  private Rectangle currentMovingRect;
  private Point itsStartMovingPt;
  private Rectangle itsStartInclusionRect;

  // (***fd) would like to delete itsCurrentObject, but too messy...

  private ErmesObject itsCurrentObject = null;
  private ErmesObject itsResizingObject = null;

  private ErmesObject itsPotentialConnectionFromObject = null;
  private int itsPotentialConnectionFromOutletNum = -1;

  private Point startConnectPoint = new Point();
  private Point previousConnectPoint = new Point();
  private Point currentConnectPoint = new Point();

  // This is an housekeeping structure to implement the re-edit of an
  // editable object. The logic is:
  // 1) click on an object selects the object and go in MOVING status
  // 2) release the mouse (moves if moved), go in START_SELECT
  // 3) second click select the object, go in MOVING
  // 4) second release re-edit the object only if the first click was on the same, already selected object.

  private boolean clickHappenedOnAnAlreadySelected = false;

  private int editStatus = DOING_NOTHING;
  private int oldEditStatus = DOING_NOTHING;

  final void doNothing() 
  {
    editStatus = DOING_NOTHING;
  }

  final void startSelection()
  {
    editStatus = START_SELECT;
  }

  final boolean isEditingObject()
  {
    return (editStatus == EDITING_OBJECT);
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Mouse Handling                                           //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  private int currentMouseX, currentMouseY;	// used during the MOVING status
  private int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  void ClickOnConnection( ErmesConnection connection, MouseEvent evt,int x, int y)
  {
    switch( editStatus) {

    case DOING_NOTHING:
      ErmesSelection.patcherSelection.select( connection); 
      connection.redraw();
      editStatus = START_SELECT;
      break;

    case START_SELECT:
      if ( !evt.isShiftDown()) 
	{
	  sketch.resetFocus();
	  ErmesSelection.patcherSelection.setOwner(sketch); 

	  ErmesSelection.patcherSelection.redraw(); 
	  ErmesSelection.patcherSelection.deselectAll(); 
	  ErmesSelection.patcherSelection.select( connection); 
	  connection.redraw();
	}
      else
	{
	  if (! ErmesSelection.patcherSelection.isSelected( connection))
	    {
	      ErmesSelection.patcherSelection.select( connection);
	      connection.redraw();
	    }
	  else 
	    {
	      ErmesSelection.patcherSelection.deselect( connection);
	      connection.redraw();

	      if ( ErmesSelection.patcherSelection.isEmpty())
		editStatus = DOING_NOTHING;
	    }
	}	

      break;	
    }
  }

  private void ClickOnObject( ErmesObject object, MouseEvent evt, int theX, int theY)
  {
    switch( editStatus)
      {
      case DOING_NOTHING:
	ErmesSelection.patcherSelection.select( object);
	object.redraw();
	MoveSelected( theX,theY);
	break;

      case START_SELECT:
	if ( evt.isShiftDown()) 
	  {
	    if ( !ErmesSelection.patcherSelection.isSelected( object))
	      {
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }
	    else
	      {
		ErmesSelection.patcherSelection.deselect( object);	
		object.redraw();
		if (! ErmesSelection.patcherSelection.hasObjects()) 
		  editStatus = DOING_NOTHING;
	      }
	  }
	else if ( object.isSelected()) 
	  {
	    MoveSelected( theX, theY);
	  }
	else 
	  { 
	    sketch.resetFocus();
	    ErmesSelection.patcherSelection.setOwner(sketch); 
	    
	    if (ErmesSelection.patcherSelection.isEmpty())
	      {
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }
	    else
	      {
		ErmesSelection.patcherSelection.redraw(); 
		ErmesSelection.patcherSelection.deselectAll();
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }

	    MoveSelected( theX,theY);
	  }
	break;
      }

    displayList.ChangeObjectPrecedence( object);
  }

  void MoveSelected( int theX, int theY) 
  {
    // only sets the status and the initial coordinates
    // all the work is done by the mouseDrag() method invocations
    oldEditStatus = editStatus;
    editStatus = MOVING;

    currentMouseX = theX;
    currentMouseY = theY;

    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    itsPreviousMouseX = theX;
    itsPreviousMouseY = theY;

    itsStartInclusionRect = ErmesSelection.patcherSelection.getBounds();
  }

  void SetResizeState( ErmesObject theResizingObject, int newStatus)
  {
    editStatus = newStatus;

    itsResizingObject = theResizingObject;

    currentResizeRect.setBounds( itsResizingObject.getBounds());

    previousResizeRect.setBounds( currentResizeRect);
  }
  
  // ----------------------------------------
  // Mouse methods
  // ----------------------------------------

  private boolean erased = false;
  private boolean erased1 = false;

  private static final int cursorStateNormal = 0;
  private static final int cursorStateHResize = 1;
  private static final int cursorStateVResize = 10;
  private static final int cursorStateInletOutlet = 2;
  private static final int cursorStateControl = 3;
  private static final int cursorStateConnect = 4;
  private int cursorState = cursorStateNormal;

  public void mouseMoved( MouseEvent e)
  {
    sketch.cleanAnnotations(); // MDC

    if (sketch.isLocked())
      return;

    int x = e.getX();
    int y = e.getY();

    itsCurrentObject = displayList.getObjectContaining( x, y);
    
    if (itsCurrentObject != null)
      {
	SensibilityArea area = itsCurrentObject.findSensibilityArea( x, y);

	if (area instanceof HResizeSensibilityArea)
	  {
	    if ( cursorState != cursorStateHResize)
	      {
		sketch.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
		cursorState = cursorStateHResize;
	      }
	  }
	else if (area instanceof VResizeSensibilityArea)
	  {
	    if ( cursorState != cursorStateVResize)
	      {
		sketch.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
		cursorState = cursorStateVResize;
	      }
	  }
	else if (area instanceof OutletSensibilityArea)
	  {
	    if ( cursorState != cursorStateInletOutlet)
	      {
		sketch.setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
		cursorState = cursorStateInletOutlet;
	      }
	  }
	// final case
	else if (area instanceof NothingSensibilityArea)
	  {
	    if (cursorState != cursorStateNormal)
	      {
		sketch.setCursor( Cursor.getDefaultCursor());
		cursorState = cursorStateNormal;
	      }
	  }
      }
    else   // not in an object
      {
	if (cursorState != cursorStateNormal)
	  {
	    sketch.setCursor( Cursor.getDefaultCursor());
	    cursorState = cursorStateNormal;
	  }
      }
  }

  public void mouseDragged( MouseEvent e)
  {
    int x = e.getX();
    int y = e.getY();

    if (cursorState != cursorStateNormal 
	&& cursorState != cursorStateHResize
	&& cursorState != cursorStateVResize)
      {
	sketch.setCursor( Cursor.getDefaultCursor());
	cursorState = cursorStateNormal;
      }

    sketch.cleanAnnotations(); // MDC
    
    if ( sketch.isLocked() || e.isControlDown())
      {
	if ( itsCurrentObject != null)
	  itsCurrentObject.mouseDrag( e, x, y);
	return;
      }

    DynamicScrolling( x, y);
    
    switch (editStatus) {
    case START_CONNECT:
      {
	currentConnectPoint.setLocation( x, y);

	Graphics g = sketch.getGraphics();
	if (!erased) 
	  {
	    g.setColor( Color.black);
	    g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	    g.drawLine( startConnectPoint.x, startConnectPoint.y, previousConnectPoint.x, previousConnectPoint.y);
	  }
	g.drawLine( startConnectPoint.x, startConnectPoint.y, currentConnectPoint.x, currentConnectPoint.y);
	previousConnectPoint.setLocation( currentConnectPoint);
	erased = false;

	g.dispose();

	itsCurrentObject = displayList.getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	  {
	    SensibilityArea area = itsCurrentObject.findConnectionSensibilityArea( x, y);

	    if (area instanceof InletOutletSensibilityArea)
	      {
		if (itsCurrentObject != itsPotentialConnectionFromObject)
		  {
		    // (***fd) will add highlighting of the in/outlet
		    sketch.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
		    cursorState = cursorStateConnect;

		    return;
		  }
	      }
	  }

	if (cursorState != cursorStateNormal)
	  {
	    sketch.setCursor( Cursor.getDefaultCursor());
	    cursorState = cursorStateNormal;
	  }

	return; 
      }

    case AREA_SELECT:
      {
	if ( (java.lang.Math.abs( x - currentPoint.x) < 5)
	     || (java.lang.Math.abs( y - currentPoint.y) < 5)) 
	  return;

	if ( x > currentPoint.x) 
	  if ( y > currentPoint.y)	
	    currentRect.setBounds( currentPoint.x, currentPoint.y, x - currentPoint.x, y - currentPoint.y);
	  else
	    currentRect.setBounds( currentPoint.x, y, x - currentPoint.x, currentPoint.y - y);
	else if ( y > currentPoint.y)  
	  currentRect.setBounds( x, currentPoint.y, currentPoint.x - x, y - currentPoint.y); 
	else
	  currentRect.setBounds( x, y, currentPoint.x - x, currentPoint.y - y);
      
	Graphics g = sketch.getGraphics();
	if (!erased)
	  {
	    g.setColor( Color.black);
	    g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	    g.drawRect( previousRect.x, previousRect.y, previousRect.width, previousRect.height);
	  }
	//paint new:
	g.drawRect( currentRect.x, currentRect.y, currentRect.width, currentRect.height);
	previousRect.setBounds( currentRect);
	erased = false;
	g.dispose();

	return;
      }

    case H_RESIZING_OBJECT:
      {
	int prevX = previousResizeRect.x + previousResizeRect.width;

	// (***fd) must resize selection, and maintain a vector of resize rectangles
	itsResizingObject.resizeBy( x - prevX, 0);
	currentResizeRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = sketch.getGraphics();
	if ( !erased)
	  {
	    g.setColor( Color.black);
	    g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	    g.drawRect( previousResizeRect.x, previousResizeRect.y, previousResizeRect.width, previousResizeRect.height);
	  }
	//paint new:
	g.drawRect( currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
	previousResizeRect.setBounds( currentResizeRect);
	erased = false;
	g.dispose();

	return;
      } 

    case V_RESIZING_OBJECT:
      {
	int prevY = previousResizeRect.y + previousResizeRect.height;

	// (***fd) must resize selection, and maintain a vector of resize rectangles
	itsResizingObject.resizeBy( 0, y - prevY);
	currentResizeRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = sketch.getGraphics();
	if ( !erased)
	  {
	    g.setColor( Color.black);
	    g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	    g.drawRect( previousResizeRect.x, previousResizeRect.y, previousResizeRect.width, previousResizeRect.height);
	  }
	//paint new:
	g.drawRect( currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
	previousResizeRect.setBounds( currentResizeRect);
	erased = false;
	g.dispose();

	return;
      } 

    case MOVING:
      if ( itsStartInclusionRect.x + (x - itsStartMovingPt.x) >= 0)
	currentMouseX = x;

      if ( itsStartInclusionRect.y + (y - itsStartMovingPt.y) >= 0)
	currentMouseY = y;

      Graphics g = sketch.getGraphics();
      Rectangle aRect;
      g.setColor( Color.black);
      g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());

      int oldX, oldY, oldWidth, oldHeight;
	  
      if ( (currentMovingRect != null) && !erased)
	{
	  g.drawRect( currentMovingRect.x, 
		      currentMovingRect.y, 
		      currentMovingRect.width, 
		      currentMovingRect.height);
	}


      currentMovingRect = ErmesSelection.patcherSelection.getBounds();
      currentMovingRect.x += (currentMouseX-itsStartMovingPt.x);
      currentMovingRect.y += (currentMouseY-itsStartMovingPt.y);

      g.drawRect( currentMovingRect.x, 
		  currentMovingRect.y, 
		  currentMovingRect.width, 
		  currentMovingRect.height);
	  
      erased = false;

      g.dispose();

      return;
    }
  }

  public void mousePressed( MouseEvent e)
  {
    Toolkit.getDefaultToolkit().sync();

    sketch.itsSketchWindow.requestFocus();

    if (cursorState != cursorStateNormal 
	&& cursorState != cursorStateHResize
	&& cursorState != cursorStateVResize)
      {
	sketch.setCursor( Cursor.getDefaultCursor());
	cursorState = cursorStateNormal;
      }

    int x = e.getX();
    int y = e.getY();

    sketch.cleanAnnotations(); // MDC    
    
    if ( e.isAltDown())
      {
	itsCurrentObject = displayList.getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	  {
	    if (! FtsHelpPatchTable.openHelpPatch( itsCurrentObject.getFtsObject()))
	      new ErrorDialog( sketch.itsSketchWindow, 
			       "Sorry, no help for object " + itsCurrentObject.getFtsObject().getClassName());
	  }

	return;
      }
    
    if ( sketch.isLocked() || e.isControlDown()) 
      {
	setKeyEventClient( null);

	itsCurrentObject = displayList.getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	  {
	    if ( e.getClickCount() > 1 )
	      itsCurrentObject.editContent();
	    else
	      itsCurrentObject.mouseDown( e, x, y);
	  }

	return;
      }
    
    if (  editStatus == EDITING_OBJECT)
      {
	sketch.getEditField().LostFocus();
	sketch.requestFocus();
	editStatus = DOING_NOTHING;
      }


    if ((editStatus == DOING_NOTHING) &&
	toolBar.isActive())
      {
	toolBar.makeObject(x,y);
	return;
      }
    
    ///if we are in a Object
    itsCurrentObject = displayList.getObjectContaining( x, y);

    if ( itsCurrentObject != null)
      {
	SensibilityArea area = itsCurrentObject.findSensibilityArea( x, y);

	if (area instanceof HResizeSensibilityArea)
	  {
	    itsCurrentObject.redraw();
	    SetResizeState( itsCurrentObject, H_RESIZING_OBJECT);
	  }
	else if (area instanceof VResizeSensibilityArea)
	  {
	    itsCurrentObject.redraw();
	    SetResizeState( itsCurrentObject, V_RESIZING_OBJECT);
	  }
	else if (area instanceof OutletSensibilityArea)
	  {
	    ErmesSelection.patcherSelection.setOwner(sketch); 

	    if (! ErmesSelection.patcherSelection.isEmpty())
	      ErmesSelection.patcherSelection.deselectAll();

	    editStatus = START_CONNECT;

	    itsPotentialConnectionFromObject = itsCurrentObject;
	    itsPotentialConnectionFromOutletNum = ((OutletSensibilityArea)area).getNumber();
	    prepareForDynamicConnect( ((OutletSensibilityArea)area).getNumber() );

	    // repaint();  @@@ ???
	  }
	else
	  {
	    if (itsCurrentObject.isSelected())
	      clickHappenedOnAnAlreadySelected = true;
	    else
	      clickHappenedOnAnAlreadySelected = false;

	    if ( e.getClickCount() > 1 )
	      itsCurrentObject.editContent();
	    else
	      ClickOnObject(itsCurrentObject, e, x, y);
	  }

	return;
      }

    ///if we are in a Connection
    ErmesConnection connection = displayList.getConnectionNear( x, y);

    if (connection != null)
      {
	if ( ! sketch.isLocked())
	  ClickOnConnection(connection, e,x,y);

	return;
      }

    
    //DOING_NOTHING, START_SELECT

    if (!e.isShiftDown()) 
      {
	sketch.resetFocus();
	ErmesSelection.patcherSelection.setOwner(sketch); 

	if (! ErmesSelection.patcherSelection.isEmpty())
	  {
	    ErmesSelection.patcherSelection.redraw(); 
	    ErmesSelection.patcherSelection.deselectAll();
	  }
      }

    editStatus = AREA_SELECT;
    currentRect.setBounds( x,y,0,0);
    previousRect.setBounds( x,y,0,0);
    currentPoint.setLocation( x,y);
  }

  public void mouseReleased( MouseEvent e)
  {
    int x = e.getX();
    int y = e.getY();

    sketch.cleanAnnotations(); // MDC    

    if ( sketch.isLocked() || e.isControlDown())
      {
	if (itsCurrentObject != null) 
	  itsCurrentObject.mouseUp( e, x, y);

	itsCurrentObject = null;
      }
    else if (editStatus == START_CONNECT)
      {
	itsCurrentObject = displayList.getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	    {
	      SensibilityArea area = itsCurrentObject.findConnectionSensibilityArea( x, y);

	      if (area instanceof InletOutletSensibilityArea)
		{
		  // (*** fd) It is actualy forbiden to connect an object to itself in the interaction
		  if (itsPotentialConnectionFromObject != itsCurrentObject)
		    {
		      try
			{
			  FtsConnection fc;
			  ErmesConnection connection;

			  fc = Fts.makeFtsConnection(itsPotentialConnectionFromObject.getFtsObject(),
						     itsPotentialConnectionFromOutletNum,
						     itsCurrentObject.getFtsObject(),
						     ((InletOutletSensibilityArea)area).getNumber());

			  connection = new ErmesConnection( sketch, 
							    itsPotentialConnectionFromObject,
							    itsPotentialConnectionFromOutletNum,
							    itsCurrentObject,
							    ((InletOutletSensibilityArea)area).getNumber(),
							    fc);

			  displayList.addConnection(connection);
			  connection.redraw();
			}
		      catch (FtsException ex)
			{
			  // In case of exception, just do not create the connection
			  // this is the proper error handling in this situation.
			}
		    }
		}
	    }

	    currentConnectPoint.setLocation( 0, 0);
	    startConnectPoint.setLocation( 0, 0);
	    previousConnectPoint.setLocation( 0, 0);

	    sketch.setCursor( Cursor.getDefaultCursor());

	    itsPotentialConnectionFromObject = null;
	    itsPotentialConnectionFromOutletNum = -1;

	    editStatus = DOING_NOTHING;
      }
    else if (editStatus == AREA_SELECT)
      {
	Rectangle aRect = NormalizedRect( currentRect);

	if (!aRect.isEmpty())
	  displayList.select(aRect);

	if (! ErmesSelection.patcherSelection.hasObjects())
	  { 
	    editStatus = DOING_NOTHING;

	    if (! aRect.isEmpty())
	      sketch.repaint(aRect.x, aRect.y, aRect.width, aRect.height);
	  }
	else
	  {
	    editStatus = START_SELECT;
	    sketch.redraw();
	  }
      }
    else if (editStatus == MOVING)
      {
	int aDeltaH, aDeltaV;

	if ( currentMovingRect != null)
	  currentMovingRect.setBounds( 0,0,0,0);

	editStatus = START_SELECT;

	if ( (currentMouseX - itsStartMovingPt.x != 0) || (currentMouseY - itsStartMovingPt.y != 0))
	  {
	    aDeltaH = currentMouseX - itsStartMovingPt.x;
	    aDeltaV = currentMouseY - itsStartMovingPt.y;

	    ErmesSelection.patcherSelection.moveAll( aDeltaH, aDeltaV);
	  }
	else
	  {
	    //if not moved, open edit mode

	    if (e.getClickCount() == 1)
	      {
		if (oldEditStatus == START_SELECT)
		  {
		    if (itsCurrentObject instanceof ErmesObjEditable)
		      {
			if (clickHappenedOnAnAlreadySelected) 
			  {
			    sketch.resetFocus();
			    ErmesSelection.patcherSelection.setOwner(sketch); 
			    ErmesSelection.patcherSelection.deselectAll();
			    ((ErmesObjEditable)itsCurrentObject).restartEditing();
			    editStatus = EDITING_OBJECT;
			  }
		      }
		  }
	      }
	  }
      }
    else if (editStatus == H_RESIZING_OBJECT || editStatus == V_RESIZING_OBJECT)
      {
	itsResizingObject.redraw();
	editStatus = START_SELECT;
      }
    else if (editStatus == DOING_NOTHING)
      {
      }

    Toolkit.getDefaultToolkit().sync();
  }

  public void mouseClicked( MouseEvent e)
  {
    sketch.cleanAnnotations();
  }

  public void mouseEntered( MouseEvent e)
  {
    sketch.cleanAnnotations();
  } 

  public void mouseExited( MouseEvent e)
  {
    sketch.cleanAnnotations();
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Keyboard Handling                                        //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  // KeyClient Handling

  public void setKeyEventClient( KeyEventClient keyEventClient)
  {
    if ( this.keyEventClient != null && this.keyEventClient != keyEventClient)
      this.keyEventClient.keyInputLost();

    this.keyEventClient = keyEventClient;

    if (this.keyEventClient != null)
      this.keyEventClient.keyInputGained();
  }

  // Modified to use inheritance and call the ErmesEditor method
  // for all the standard key bindings

  public void keyPressed( KeyEvent e)
  {
    ErmesSelection selection = ErmesSelection.patcherSelection;
    int aInt = e.getKeyCode();

    sketch.cleanAnnotations(); // MDC

    //arrows first:
    if ( isAnArrow( aInt)) 
      {
	if (e.isShiftDown()) 
	  {
	    if (e.isControlDown())
	      {
		if ( aInt == KeyEvent.VK_LEFT)
		  selection.resizeAll(-10, 0);
		else if (aInt == KeyEvent.VK_RIGHT) 
		  selection.resizeAll(10, 0);
		else if (aInt == KeyEvent.VK_UP) 
		  selection.resizeAll(0, -10);
		else if (aInt == KeyEvent.VK_DOWN) 
		  selection.resizeAll(0, 10);
	      }
	    else
	      {
		if (aInt == KeyEvent.VK_LEFT)
		  selection.moveAll( -10, 0);
		else if (aInt == KeyEvent.VK_RIGHT)
		  selection.moveAll( 10, 0);
		if (aInt == KeyEvent.VK_UP)
		  selection.moveAll( 0, -10);
		else if (aInt == KeyEvent.VK_DOWN)
		  selection.moveAll( 0, 10);
	      }
	  }
	else if (e.isControlDown()) 
	  {
	    if (e.isMetaDown())
	      {
		if ( aInt == KeyEvent.VK_LEFT)
		  {
		    // Do nothing, currently
		  }
		else if (aInt == KeyEvent.VK_RIGHT)
		  {
		    selection.resizeToMaxWidth();
		  }
		else if (aInt == KeyEvent.VK_UP) 
		  {
		    // Do nothing, currently
		  }
		else if (aInt == KeyEvent.VK_DOWN) 
		  {
		    selection.resizeToMaxHeight();
		  }
	      }
	    else
	      {
		if ( aInt == KeyEvent.VK_LEFT)
		  selection.resizeAll(-1, 0);
		else if (aInt == KeyEvent.VK_RIGHT) 
		  selection.resizeAll(1, 0);
		else if (aInt == KeyEvent.VK_UP) 
		  selection.resizeAll(0, -1);
		else if (aInt == KeyEvent.VK_DOWN) 
		  selection.resizeAll(0, 1);
	      }
	  }
	else if ( e.isMetaDown()) 
	  {
	    //align

	    if (ErmesSelection.patcherSelection.ownedBy(sketch))
	      {
		if (aInt == KeyEvent.VK_LEFT)
		  ErmesSelection.patcherSelection.alignLeft();
		else if (aInt == KeyEvent.VK_RIGHT) 
		  ErmesSelection.patcherSelection.alignRight();
		else if (aInt == KeyEvent.VK_UP) 
		  ErmesSelection.patcherSelection.alignTop();
		else if (aInt == KeyEvent.VK_DOWN) 
		  ErmesSelection.patcherSelection.alignBottom();
	      }
	  }
	else
	  {
	    if (aInt == KeyEvent.VK_LEFT)
	      selection.moveAll( -1, 0);
	    else if (aInt == KeyEvent.VK_RIGHT)
	      selection.moveAll( 1, 0);
	    if (aInt == KeyEvent.VK_UP)
	      selection.moveAll( 0, -1);
	    else if (aInt == KeyEvent.VK_DOWN)
	      selection.moveAll( 0, 1);
	  }
      }
    else if (e.isControlDown()) 
      {
	if (aInt == KeyEvent.VK_Z)
	  sketch.showErrorDescriptions();
      }
    else if ( keyEventClient != null)
      {
	keyEventClient.keyPressed( e);
      }
    else if ( ( aInt == KeyEvent.VK_DELETE)
	      || ( aInt== KeyEvent.VK_BACK_SPACE) )
      {
	/* This test should go away ... the state machine should
	   guarantee empty selection during field editing */

	if (ErmesSelection.patcherSelection.ownedBy(sketch))
	  if (! sketch.getEditField().HasFocus())
	    selection.deleteAll();
      }
  }

  public void keyTyped(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyTyped( e);
  }

  public void keyReleased(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyReleased( e);
  }

  public static boolean isAnArrow( int code) 
  {
    return code == KeyEvent.VK_LEFT 
      || code == KeyEvent.VK_RIGHT
      || code == KeyEvent.VK_UP
      || code == KeyEvent.VK_DOWN;
  }
}

