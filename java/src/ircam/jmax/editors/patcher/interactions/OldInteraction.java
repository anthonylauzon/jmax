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

  private void prepareForDynamicConnect( int outletNumber) 
  {
    startConnectPoint.setLocation( itsPotentialConnectionFromObject.getConnectionStartX( outletNumber),
				   itsPotentialConnectionFromObject.getConnectionStartY( outletNumber));

    currentConnectPoint.setLocation( startConnectPoint);
    previousConnectPoint.setLocation( startConnectPoint);
    sketch.setCursor( Cursor.getDefaultCursor());
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

  private Rectangle rect = new Rectangle(); // utility, stateless, local use rectangle

  private Rectangle currentRect = new Rectangle();
  private Rectangle previousRect = new Rectangle();
  private Point	    currentPoint = new Point();

  private Point itsStartMovingPt = new Point( 0,0);  

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

  private int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  void ClickOnConnection( ErmesConnection connection, MouseEvent evt,int x, int y)
  {
    /* First, if the shift is not pressed, and 
       we own the selection, and there is something selected
       deselect all */

    if ((! evt.isShiftDown()) && 
	ErmesSelection.patcherSelection.ownedBy(sketch) &&
	(! ErmesSelection.patcherSelection.isEmpty(sketch)))
      {
	ErmesSelection.patcherSelection.deselectAll(); 
	ErmesSelection.patcherSelection.redraw(); 	
      }

    /* Then, toggle the selection state of the connection
       argument, and redraw it */

    if (ErmesSelection.patcherSelection.isSelected( connection))
      ErmesSelection.patcherSelection.deselect( connection);
    else
      ErmesSelection.patcherSelection.select( connection);

    connection.redraw();

    /* To delete @@@*/

    if ( ErmesSelection.patcherSelection.isEmpty())
      editStatus = DOING_NOTHING;
  }

  private void ClickOnObject( ErmesObject object, MouseEvent evt, int theX, int theY)
  {
    if ((! evt.isShiftDown()) && 
	ErmesSelection.patcherSelection.ownedBy(sketch) &&
	(! ErmesSelection.patcherSelection.isEmpty(sketch)))


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

    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    itsPreviousMouseX = theX;
    itsPreviousMouseY = theY;
  }

  void SetResizeState( ErmesObject theResizingObject, int newStatus)
  {
    editStatus = newStatus;

    itsResizingObject = theResizingObject;

    currentRect.setBounds( itsResizingObject.getBounds());

    previousRect.setBounds( currentRect);
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

	if (area == null)
	  {
	    if (cursorState != cursorStateNormal)
	      {
		sketch.setCursor( Cursor.getDefaultCursor());
		cursorState = cursorStateNormal;
	      }
	  }
	else if (area instanceof HResizeSensibilityArea)
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
    
    if ( sketch.isLocked())
      {
	if ( itsCurrentObject != null)
	  itsCurrentObject.mouseDrag( e, x, y);
	return;
      }

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
	displayList.repaintRectangle();

	if ( x > currentPoint.x)
	  {
	    if ( y > currentPoint.y)	
	      displayList.setRectangle( currentPoint.x, currentPoint.y,
					   x - currentPoint.x, y - currentPoint.y);
	    else
	      displayList.setRectangle( currentPoint.x, y,
					   x - currentPoint.x, currentPoint.y - y);
	  }
	else
	  {
	    if ( y > currentPoint.y)
	      displayList.setRectangle( x, currentPoint.y, currentPoint.x - x, y - currentPoint.y); 
	    else
	      displayList.setRectangle( x, y, currentPoint.x - x, currentPoint.y - y);
	  }

	displayList.repaintRectangle();

	displayList.getRectangle(rect);
	displayList.selectExactly(rect);
	return;
      }

    case H_RESIZING_OBJECT:
      {
	int prevX = previousRect.x + previousRect.width;

	// (***fd) must resize selection, and maintain a vector of resize rectangles
	itsResizingObject.resizeBy( x - prevX, 0);
	currentRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = sketch.getGraphics();
	if ( !erased)
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

    case V_RESIZING_OBJECT:
      {
	int prevY = previousRect.y + previousRect.height;

	// (***fd) must resize selection, and maintain a vector of resize rectangles

	itsResizingObject.resizeBy( 0, y - prevY);
	currentRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = sketch.getGraphics();
	if ( !erased)
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

    case MOVING:
      int dx, dy;

      dx = (x - itsPreviousMouseX);
      dy = (y - itsPreviousMouseY);

      itsPreviousMouseX = x;
      itsPreviousMouseY = y;

      ErmesSelection.patcherSelection.moveAllBy( dx, dy);
      sketch.fixSize();
      break;
    }
  }

  public void mousePressed( MouseEvent e)
  {
    Toolkit.getDefaultToolkit().sync();

    sketch.resetFocus(); // ???
    sketch.requestFocus();

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
	// DONE
      }

    if ( sketch.isLocked()) 
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
	ErmesSelection.patcherSelection.setOwner(sketch); 

	if (! ErmesSelection.patcherSelection.isEmpty())
	  {
	    ErmesSelection.patcherSelection.redraw(); 
	    ErmesSelection.patcherSelection.deselectAll();
	  }
      }

    editStatus = AREA_SELECT;
    displayList.setRectangle(x, y, 0, 0);

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
	/// DONE
      }
    else if (editStatus == MOVING)
      {
	// DONE
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
    int aInt = e.getKeyCode();

    sketch.cleanAnnotations(); // MDC

    if ( keyEventClient != null)
      {
	keyEventClient.keyPressed( e);
      }
    else if ( (aInt == KeyEvent.VK_DELETE) ||
	      (aInt== KeyEvent.VK_BACK_SPACE) )
      {
	/* This code should move to an Action !!
	   It currently cannot because the keyEventClient
	   want to pre-empt DELETE ... (should do something;
	   also, the keyEventClient should disappear, and just be
	   a keyListener sullo sketch
	   */
	   
	ErmesSelection selection = ErmesSelection.patcherSelection;

	if (selection.ownedBy(sketch))
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
}

