package ircam.jmax.editors.ermes;

import java.awt.*; 
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The graphic workbench for the patcher editor.
// It handles the interaction of the user with the objects,
// propagates the mouse events to the objects themselves,
// allow the selection, moving, erasing, resizing of objects.
// It keeps track of the toolbar state, it handles the 
// offscreen and much, much more...
// 

class ErmesSketchPad extends Panel implements AdjustmentListener, MouseMotionListener, MouseListener, FtsUpdateGroupListener {
  // The element list is implemented as a array, whose dimension
  // is doubled on reallocation; two methods are provided to add an element
  // and to remove an element; the access is done directly to the array
  // it is not implemented as a nested class to try to reduce to the minimum
  // every indirection in its use.

  static Toolkit theToolkit = Toolkit.getDefaultToolkit();

  boolean deleted = false; // set to true when the sketch pad is cleaned up.

  MaxVector itsElements = new MaxVector();

  Rectangle normalizedRect = new Rectangle();  

  private final Rectangle NormalizedRect(Rectangle theRect) 
  {
    normalizedRect.x = (theRect.width < 0)? theRect.x + theRect.width: theRect.x;
    normalizedRect.y = (theRect.height < 0)? theRect.y +theRect.height: theRect.y;
    normalizedRect.width = (theRect.width < 0)? - theRect.width: theRect.width;
    normalizedRect.height = (theRect.height < 0)? - theRect.height: theRect.height;
    
    return normalizedRect;
  }
  
  public boolean isInGroup = false;
  public boolean drawPending = false;
  public boolean copyPending = false;
  
  public void updateGroupStart() 
  {
    isInGroup = true;
  }
  
  public void updateGroupEnd() 
  {
    isInGroup = false;

    if (drawPending) 
      {
	Graphics g = getGraphics();
	DrawOffScreen( g);
	g.dispose();
      }
    else if (copyPending) 
      {
	Graphics g = getGraphics();
	CopyTheOffScreen( g);
	g.dispose();
      }
    drawPending = false;
    copyPending = false;

    theToolkit.sync();
  }

  ErmesSketchWindow itsSketchWindow;
  FtsObject itsPatcher;
  FtsPatcherData itsPatcherData;

  private final static int SKETCH_WIDTH = 1200;
  private final static int SKETCH_HEIGHT = 1200;
  static Dimension preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);

  final static int DOING_NOTHING = 0;		
  final static int START_ADD 	 = 1;
  final static int START_SELECT  = 2;
  final static int AREA_SELECT 	 = 3;		
  final static int MOVING 	 = 4;
  final static int MOVINGSEGMENT    = 5;	
  final static int H_RESIZING_OBJECT = 6;
  final static int V_RESIZING_OBJECT = 60;
  final static int EDITING_OBJECT   = 7;
  final static int EDITING_COMMENT  = 8;
  final static int FromOutToIn 	    = 1;
  final static int FromInToOut 	    = -1;
  final static int NoDirections     = 0;
  final static int START_CONNECT = 26;

  Font sketchFont = new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE);
  int sketchFontSize = ircam.jmax.utils.Platform.FONT_SIZE;

  private final static int MAX_NUM_SELECTED = 10;

  ErmesObjInOutPop itsInPop = null;
  ErmesObjInOutPop itsOutPop = null;

  private boolean paintForTheFirstTime = true;
  
  private ErmesObjEditField itsEditField = null;
  private ErmesObjTextArea itsTextArea = null;
  private int currentMouseX, currentMouseY;	// used during the MOVING status
  private int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  MaxVector itsConnections;
  static ErmesSelection currentSelection =  new ErmesSelection();
  Rectangle currentRect = new Rectangle();
  private Rectangle previousRect = new Rectangle();
  private Point	currentPoint = new Point();
  private Rectangle currentResizeRect = new Rectangle();
  private Rectangle previousResizeRect = new Rectangle(); 

  private Rectangle currentMovingBigRect;
  private MaxVector itsMovingRectangles;

  //STATIC OFFSCREEN!!!
  static private ErmesSketchPad lastSketchWithOffScreen = null;
  boolean offScreenPresent = false;
  static Graphics offGraphics = null;
  static private Dimension offDimension;
  static private Image offImage;
		
  ErmesSwToolbar itsToolBar;
  private Point itsStartMovingPt;
  private Rectangle itsStartInclusionRect;

  // itsCurrentConnection delete; most of the time, it is easier
  // and faster to pass argument instead of having variables keeping
  // the context.

  // (***fd) would like to delete itsCurrentObject, but too messy...
  private ErmesObject itsCurrentObject = null;
  private ErmesObject itsStartDragObject = null;
  private ErmesObject itsResizingObject = null;

  private ErmesObject itsPotentialConnectionFromObject = null;
  private int itsPotentialConnectionFromOutletNum = -1;


  // This is an housekeeping structure to implement the re-edit of an
  // editable object. The logic is:
  // 1) click on an object selects the object and go in MOVING status
  // 2) release the mouse (moves if moved), go in START_SELECT
  // 3) second click select the object, go in MOVING
  // 4) second release re-edit the object only if the first click was on the same, already selected object.
  boolean clickHappenedOnAnAlreadySelected = false;

  int editStatus = DOING_NOTHING;
  private int oldEditStatus = DOING_NOTHING;

  private String itsAddObjectDescription;
  boolean duringScrolling = false;

  private MaxVector dirtyConnections = new MaxVector(); 
  private MaxVector dirtyObjects = new MaxVector();
  private boolean dirtySketch = false;

  void addToDirtyConnections( ErmesConnection theConnection) 
  {
    if (dirtySketch) 
      return;

    if ( !dirtyConnections.contains( theConnection))
      dirtyConnections.addElement( theConnection);
  }

  void addToDirtyObjects( ErmesObject theObject) 
  {
    if (dirtySketch) 
      return;

    if ( !dirtyObjects.contains( theObject))
      dirtyObjects.addElement( theObject);
  }

  void markSketchAsDirty() 
  {
    dirtySketch = true;
  }

  private void emptyDirtyLists() 
  {
    dirtyObjects.removeAllElements();
    dirtyConnections.removeAllElements();
    dirtySketch = false;
  }

  //
  // At this level, we can implement a politic of drawing.
  // (example, if there are not objects to paint, paint connections and
  // in/outlets directly, without using CopyTheOffScreen())
  //
  void paintDirtyList() 
  {
    if (offGraphics == null) 
      GetOffGraphics();

    if (dirtySketch) 
      {
	Graphics g = getGraphics();
	DrawOffScreen( g);
	g.dispose();

	emptyDirtyLists();
	return;
      }

    if ( !isLocked()) 
      { //objects UNDER connections 

	paintList( dirtyObjects, offGraphics);
	dirtyObjects.removeAllElements();

	paintList( dirtyConnections, offGraphics);
	dirtyConnections.removeAllElements();
      }
    else  
      { // connections UNDER objects

	paintList( dirtyConnections, offGraphics);
	dirtyConnections.removeAllElements();

	paintList( dirtyObjects, offGraphics);
	dirtyObjects.removeAllElements();
      }

    Graphics g = getGraphics();
    if ( g != null)
      {
	CopyTheOffScreen( g);
	g.dispose();
      }

    //emptyDirtyLists is done in CopyTheOffScreen()
  }
  
  private void paintList( MaxVector theList, Graphics theContext) 
  {
    if (theContext == null) 
      return;

    if (theList == null)
      new Throwable( "paintNullList").printStackTrace();

    Object[] objects = theList.getObjectArray();
    int size = theList.size();

    for ( int i = 0; i < size; i++)
      ((ErmesDrawable)objects[i]).Paint( theContext);
  }

  
  final Font getSketchFont() 
  {
    return sketchFont;
  }

  final int getSketchFontSize() 
  {
    return sketchFontSize;
  }

  void ChangeFont( Font theFont)
  {
    try 
      {
	FontMetrics aFontMetrics = theToolkit.getFontMetrics( theFont);
	setFont( theFont);   
      }
    catch (Exception e) 
      {
	new ErrorDialog( itsSketchWindow, "This font/fontsize does not exist on this platform");
	return;
      }

    sketchFont = theFont;
  }
  
  void ChangeNameFont( String theFontName)
  {
    if (currentSelection.getOwner() == this)
      {
	Font aFont;

	Object[] objects = currentSelection.itsObjects.getObjectArray();
	int size = currentSelection.itsObjects.size();

	for ( int i = 0; i < size; i++) 
	  {
	    ErmesObject aObject = (ErmesObject) objects[i];

	    if ( (aObject instanceof ErmesObjEditableObject)
		 || (aObject instanceof ErmesObjComment)
		 || (aObject instanceof ErmesObjInt)
		 || (aObject instanceof ErmesObjFloat)
		 || (aObject instanceof ErmesObjIn)
		 || (aObject instanceof ErmesObjOut))
	      {
		aFont = new Font( theFontName, sketchFont.getStyle(), aObject.getFont().getSize());

		try 
		  {
		    FontMetrics aFontMetrics = theToolkit.getFontMetrics( aFont);
		    setFont( aFont);   
		  }
		catch (Exception exc) 
		  {
		    ErrorDialog aErr = new ErrorDialog( itsSketchWindow, "This font/fontsize does not exist on this platform");
		    aErr.setLocation( 100, 100);
		    aErr.show();  
		    return;
		  }

		aObject.setFont( aFont);
	      }
	  }
	repaint();
      }
  }

  void ChangeSizeFont( int fontSize)
  {
    if (currentSelection.getOwner() == this)
      {
	ErmesObject aObject;
	Font aFont;

	Object[] objects = currentSelection.itsObjects.getObjectArray();
	int size = currentSelection.itsObjects.size();

	for ( int i = 0; i < size; i++)    
	  {
	    aObject = (ErmesObject) objects[i];

	    if ( (aObject instanceof ErmesObjEditableObject)
		 || (aObject instanceof ErmesObjComment)
		 || (aObject instanceof ErmesObjInt)
		 || (aObject instanceof ErmesObjFloat)
		 || (aObject instanceof ErmesObjIn)
		 || (aObject instanceof ErmesObjOut))
	      {
		aFont = new Font( aObject.getFont().getName(), sketchFont.getStyle(), fontSize);
		try 
		  {
		    FontMetrics aFontMetrics = theToolkit.getFontMetrics( aFont);
		    setFont( aFont);   
		  }
		catch (Exception exc) 
		  {
		    ErrorDialog aErr = new ErrorDialog( itsSketchWindow, "This font/fontsize does not exist on this platform");
		    aErr.setLocation( 100, 100);
		    aErr.show();  
		    return;
		  }
	
		aObject.setFont( aFont);
	      }
	  }
	repaint();
      }
  }

  static void inspectSelection() 
  {
    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for ( int i = 0; i < size; i++) 
      {
	ErmesObject aObject = (ErmesObject) objects[i];
	aObject.inspect();
      }
  }

  void ClickOnConnection( ErmesConnection connection, MouseEvent evt,int x, int y)
  {
    switch( editStatus) {

    case START_ADD:
      break;

    case DOING_NOTHING:
      currentSelection.select( connection); 
      paintDirtyList();
      editStatus = START_SELECT;
      break;

    case START_SELECT:
      if ( !evt.isShiftDown()) 
	{
	  resetFocus();
	  currentSelection.setOwner(this); 
	  currentSelection.deselectAll(); 
	  currentSelection.select( connection); 
	  paintDirtyList();
	}
      else
	{
	  if (! currentSelection.isSelected( connection))
	    currentSelection.select( connection); 
	  else 
	    {
	      currentSelection.deselect( connection);
	  
	      if ( currentSelection.itsConnections.size() == 0)
		editStatus = DOING_NOTHING;
	    }

	  paintDirtyList();
	}	
      break;	
    }
  }

  void ClickOnObject( ErmesObject theObject, MouseEvent evt, int theX, int theY)
  {
    if ( !isLocked())
      {
	switch( editStatus) {
	case START_ADD:
	  break;

	case DOING_NOTHING:
	  currentSelection.select( theObject);
	  CheckCurrentFont();
	  MoveSelected( theX,theY);
	  paintDirtyList();
	  break;

	case START_SELECT:
	  if ( evt.isShiftDown()) 
	    {
	      if ( !currentSelection.isSelected( theObject))
		{
		  currentSelection.select( theObject);
		  CheckCurrentFont();
		}
	      else
		{
		  currentSelection.deselect( theObject);	
		  if ( currentSelection.itsObjects.isEmpty()) 
		    editStatus = DOING_NOTHING;
		}

	      paintDirtyList();
	    }
	  else if ( theObject.itsSelected) 
	    {
	      MoveSelected( theX, theY);
	    }
	  else 
	    { 
	      resetFocus();
	      currentSelection.setOwner(this); 
	      currentSelection.deselectAll();
	      currentSelection.select( theObject);
	      paintDirtyList();
	      CheckCurrentFont();
	      MoveSelected( theX,theY);
	    }
	  break;
	}
	ChangeObjectPrecedence( theObject);
      }
  }
  
  static int noOfCopy = 0;

  void CopyTheOffScreen( Graphics g) 
  {
    if ( (g != null) && (offScreenPresent) )
      {
	if (isInGroup || copyPending) 
	  {
	    copyPending = true;
	  }
	else 
	  {
	    g.drawImage( offImage, 0,0, this);
	    emptyDirtyLists();
	  }    
      }
  }

  final void DoNothing() 
  {
    editStatus = DOING_NOTHING;
  }
  
  void DrawLinesOffScreen()
  {
    Graphics g = getGraphics();
    Object[] objects = currentSelection.itsConnections.getObjectArray();
    int size = currentSelection.itsConnections.size();

    for ( int i = 0; i < size; i++) 
      {
	ErmesConnection aConnection = (ErmesConnection) objects[i];
      
	if ( offScreenPresent) 
	  {
	    aConnection.Paint( offGraphics);
	  }
	else if (g != null)
	  aConnection.Paint( g);
      }

    if (offScreenPresent)
      CopyTheOffScreen( g); 

    g.dispose();
  }
  
  void DrawOffScreen( Graphics g) 
  {
    if (drawPending || isInGroup) 
      {
	drawPending = true;
	return;
      }
    
    if (editStatus == AREA_SELECT)
      return; //we are only painting the selection rect

    Dimension d = getPreferredSize();
    
    //Create the offscreen graphics context, if no good one exists.
    if (offGraphics == null)
      {
	//first sketch created, allocate the offscreen buffer
	offDimension = d;
	offImage = createImage( d.width, d.height);

	if (offImage == null)
	  System.err.println( "No offImage !! (1)");

	offGraphics = offImage.getGraphics();
      }

    RequestOffScreen(); 

    if ( (d.width != offDimension.width) || ( d.height != offDimension.height))
      {
	// This code is never executed, because getPreferredSize is a constant.
	offDimension = d;
	//creare un nuovo offGraphics con le nuove dimensioni
	//e copiarci dentro l'immagine del vecchio
	Image oldOffImage = offImage;
	offImage = createImage( d.width, d.height);
	if ( offImage == null)
	  System.err.println( "No offImage !! (2)");
	offGraphics = offImage.getGraphics();
	offGraphics.drawImage( oldOffImage, 0, 0, this);
      }

    offGraphics.setFont( getFont());
    offGraphics.setColor( getBackground());
    offGraphics.fillRect( 0, 0, d.width, d.height);	//prepare the offscreen to be used by me
    offGraphics.setColor( Color.black);
    
    // Draw objects and connections

    if (isLocked())
      {
	// Locked mode: connections BEFORE objects
	paintList( itsConnections, offGraphics);
	paintList( itsElements, offGraphics);
      }
    else
      {
	// Edit mode: objects BEFORE connections

	paintList( itsElements, offGraphics);
	paintList( itsConnections, offGraphics);
      }

    CopyTheOffScreen( g);
  }
  
  final Graphics GetOffGraphics()
  {
    //Create the offscreen graphics context, if no good one exists.
    if (offGraphics == null)
      {
	//first sketch of the day. We do this even if it doesn't belong

	offDimension = preferredSize;

	offImage = createImage( preferredSize.width, preferredSize.height);

	if (offImage == null)
	  {
	     System.err.println( "No offImage !! (3)");
	  }

	if (offImage != null)
	  {
	    //this can happen...
	    offGraphics = offImage.getGraphics();	
	    RequestOffScreen();	//a call to this function change the offscreen property.
	  }
      }

    return offGraphics;
  }
  
  final ErmesSketchWindow GetSketchWindow()
  {
    return itsSketchWindow;
  }
  
  private int incrementalPasteOffsetX;
  private int incrementalPasteOffsetY;
  int numberOfPaste = 0;
  private FtsObject anOldPastedObject = null;
  
  void resetPaste(int n)
  {
    numberOfPaste = n;
  }

  // note: the following function is a reduced version of InitFromFtsContainer.
  // better organization urges

  void PasteObjects( MaxVector objectVector, MaxVector connectionVector) 
  {
    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    ErmesConnection aConnection;

    numberOfPaste += 1;

    resetFocus();

    currentSelection.setOwner(this); 
    currentSelection.deselectAll();

    fo = (FtsObject)objectVector.elementAt( 0);

    if (numberOfPaste == 0) 
      {
	incrementalPasteOffsetX = 0;
	incrementalPasteOffsetY = 0;
      }
    else if (numberOfPaste == 1) 
      {
	anOldPastedObject = fo;

	incrementalPasteOffsetX = 20;
	incrementalPasteOffsetY = 20;
      }
    else if (numberOfPaste == 2) 
      {
	incrementalPasteOffsetX = (anOldPastedObject.getX() - fo.getX());
	incrementalPasteOffsetY = (anOldPastedObject.getY() - fo.getY());
      }

    for ( Enumeration e = objectVector.elements(); e.hasMoreElements();) 
      {
	fo = (FtsObject)e.nextElement();

	int newPosX = fo.getX() + numberOfPaste*incrementalPasteOffsetX;
	int newPosY = fo.getY() + numberOfPaste*incrementalPasteOffsetY;

	// Added check for negative positions 

	if (newPosX >= 0)
	  fo.setX( newPosX);
	else
	  fo.setX( 0);     

	if (newPosY >= 0)
	  fo.setY( newPosY);
	else
       	  fo.setY( 0);

	aObject = AddObject( fo);
	currentSelection.select( aObject);
      }

    // chiama tanti AddConnection...
    ErmesObject fromObj, toObj;
    
    for ( Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) 
      {
	fc = (FtsConnection)e2.nextElement();

	fromObj = getErmesObjectFor(fc.getFrom());
	toObj   = getErmesObjectFor(fc.getTo());
	aConnection = AddConnection( fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
	currentSelection.select( aConnection);
      }

    editStatus = START_SELECT;
    paintDirtyList();
  }
  
  void InitFromFtsContainer( FtsPatcherData theContainerObject)
  {
    FtsPatcherData aFtsPatcherData = theContainerObject;

    Object[] objects = aFtsPatcherData.getObjects().getObjectArray();
    int osize = aFtsPatcherData.getObjects().size();

    for ( int i = 0; i < osize; i++)
      AddObject( (FtsObject)objects[i]);
		
    // chiama tanti AddConnection...

    MaxVector connectionVector = aFtsPatcherData.getConnections();	//usefull?

    Object[] connections = aFtsPatcherData.getConnections().getObjectArray();
    int csize = aFtsPatcherData.getConnections().size();

    for ( int i = 0; i < csize; i++)
      {
	FtsConnection fc = (FtsConnection)connections[i];

	AddConnection( getErmesObjectFor(fc.getFrom()),
		       getErmesObjectFor(fc.getTo()),
		       fc.getFromOutlet(),
		       fc.getToInlet(),
		       fc);
      }

    repaint();
  }

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  ErmesSketchPad( ErmesSketchWindow theSketchWindow, FtsPatcherData thePatcherData) 
  {
    super();

    itsPatcherData = thePatcherData;
    itsPatcher = thePatcherData.getContainerObject(); // ???

    Fts.getServer().addUpdateGroupListener( this);

    setLayout( null);

    itsSketchWindow = theSketchWindow;
    itsConnections = new MaxVector();

    itsStartMovingPt = new Point( 0,0);    
    itsStartInclusionRect = new Rectangle();  
        
    itsEditField = new ErmesObjEditField( this);
    add( itsEditField);

    itsEditField.setVisible( false);
    itsEditField.setLocation( -200,-200);
    
    itsTextArea = new ErmesObjTextArea( this);
    add( itsTextArea);

    itsTextArea.setVisible( false);
    itsTextArea.setLocation( -200,-200);

    setBackground( Settings.sharedInstance().getEditBackgroundColor());
    addMouseMotionListener( this); 
    addMouseListener( this);
    addKeyListener( itsSketchWindow);

    setFont( new Font( ircam.jmax.utils.Platform.FONT_NAME, 
		       Font.PLAIN, 
		       ircam.jmax.utils.Platform.FONT_SIZE));

    InitFromFtsContainer( itsPatcherData);
    PrepareInChoice(); 
    PrepareOutChoice();
  }
	
  void RequestOffScreen() 
  {
    if ( lastSketchWithOffScreen == this || offScreenPresent) 
      return;

    if (lastSketchWithOffScreen != null) 
      lastSketchWithOffScreen.offScreenPresent = false;

    offScreenPresent = true;
    lastSketchWithOffScreen = this;
    paintForTheFirstTime = true;
  }
  

  /* Simplified: size and font are preselected iff there is only
     one object in the selection; the idea was nice, but creating
     two vector, an enumeration and a quadratic algorithm for
     every selection operation was a little bit too much (mdc) */

  void CheckCurrentFont()
  {
    ErmesObject aObject;
    String aFontName;
    Integer aSize;

    if (currentSelection.itsObjects.size() == 1)
      {
	aObject = (ErmesObject) currentSelection.itsObjects.elementAt(0);
	
	aFontName = aObject.getFont().getName().toLowerCase();
	aSize = new Integer( aObject.getFont().getSize());

	itsSketchWindow.SelectionUpdateMenu( aFontName, aSize);
      }
    else
      itsSketchWindow.SelectionUpdateMenu( null, null);
  }


  static final private Dimension minSize = new Dimension(30, 20);

  public Dimension getMinimumSize() 
  {
    return minSize;
  }
	
  void AddingObject( int x, int y)
  {
    FtsObject fo;
    ErmesObject aObject;

    try
      {
	fo = Fts.makeFtsObject( itsPatcher, itsAddObjectDescription);

	fo.setX( x);
	fo.setY( y);

	aObject = AddObject( fo);

	repaint(); // ??

	if (aObject instanceof ErmesObjEditable)
	  ((ErmesObjEditable)aObject).startEditing();
      }
    catch ( FtsException ftse)
      {
	System.err.println( "ErmesSketchPad:mousePressed: INTERNAL ERROR: FTS Instantiation Error: " + ftse);
	ftse.printStackTrace();
      }
  }

  ErmesObject getErmesObjectFor(FtsObject obj)
  {
    Object[] objects = itsElements.getObjectArray();
    int size = itsElements.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject aObject = (ErmesObject) objects[i];

	if (aObject.itsFtsObject == obj)
	  return aObject;
      }

    return null;
  }

  ErmesConnection getErmesConnectionFor(FtsConnection c)
  {
    Object[] objects = itsConnections.getObjectArray();
    int size = itsConnections.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesConnection aConnection = (ErmesConnection) objects[i];

	if (aConnection.itsFtsConnection == c)
	  return aConnection;
      }

    return null;
  }

  private void prepareForDynamicConnect( int outletNumber) 
  {
    startConnectPoint.setLocation( itsPotentialConnectionFromObject.getConnectionStartX( outletNumber),
				   itsPotentialConnectionFromObject.getConnectionStartY( outletNumber));

    currentConnectPoint.setLocation( startConnectPoint);
    previousConnectPoint.setLocation( startConnectPoint);
    setCursor( Cursor.getDefaultCursor());
  }


  //
  // Resize a group of object by the given amount of pixel in the given direction
  //
  void resizeSelection( int amount, int direction) 
  {
    int x_amount = 0;
    int y_amount = 0;
    
    if ( direction == Platform.LEFT_KEY)
      x_amount = -amount;
    else if (direction == Platform.RIGHT_KEY) 
      x_amount = amount;

    if (direction == Platform.UP_KEY) 
      y_amount = -amount;
    else if (direction == Platform.DOWN_KEY) 
      y_amount = amount;

    for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); ) 
      ((ErmesObject)e.nextElement()).resizeBy( x_amount, y_amount);

    repaint();
  }
    
  //
  // Resize all the object in the selection to a size (width or height...)
  //
  void alignSizeSelection( int direction)
  {
    ErmesObject aObject;
    int max = 0;

    if (direction == Platform.LEFT_KEY)
      {
// (fd) As the concept of minimum size is no longer valid for text objects, this code
// is for now commented out
//	//we're resetting the selection to the minimum (hor.) size
// 	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); ) 
// 	  {
// 	    aObject = (ErmesObject) e.nextElement();
// 	    aObject.resizeBy( aObject.getMinimumSize().width - aObject.getWidth(), 0);
// 	  }
// 	repaint();
      }    
    else if (direction == Platform.RIGHT_KEY)
      {
	//we're setting all the object's widths to the max      
	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject) e.nextElement();
	    if (aObject.getWidth() > max) 
	      max = aObject.getWidth();
	  }

	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) 
	  {
	    aObject = (ErmesObject) e.nextElement();
	    aObject.resizeBy( max-aObject.getWidth(), 0);
	  }
	repaint();
      }
    else if (direction == Platform.UP_KEY) 
      {
// (fd) As the concept of minimum size is no longer valid for text objects, this code
// is for now commented out
// 	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) 
// 	  {
// 	    aObject = (ErmesObject) e.nextElement();
// 	    aObject.resizeBy( 0, aObject.getMinimumSize().height - aObject.getHeight());
// 	  }
// 	repaint();
      }
    else if (direction == Platform.DOWN_KEY)
      {
	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject) e.nextElement();
	    if (aObject.getHeight() > max)
	      max = aObject.getHeight();
	  }

	for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); ) 
	  {
	    aObject = (ErmesObject) e.nextElement();
	    aObject.resizeBy( 0, max - aObject.getHeight());
	  }
	repaint();
      }
  }

  void moveSelection( int amount, int direction)
  {
    int x_amount = 0;
    int y_amount = 0;
    ErmesObject aObject;

    if (direction == Platform.LEFT_KEY)
      x_amount = -amount;
    else if (direction == Platform.RIGHT_KEY)
      x_amount = amount;
    if (direction == Platform.UP_KEY)
      y_amount = -amount;
    else if (direction == Platform.DOWN_KEY)
      y_amount = amount;

    MoveElements( x_amount, y_amount);
    
    repaint();
  }
  
  void SelectConnections()
  {
    ErmesConnection aConnection;

    for ( Enumeration en = itsConnections.elements() ; en.hasMoreElements(); )
      {
	aConnection = (ErmesConnection) en.nextElement();

	if ( (aConnection.getSourceObject().itsSelected)
	     && (aConnection.getDestObject().itsSelected))
	  {
	    currentSelection.select( aConnection);
	  }
      }
  }

  void DynamicScrolling( int theX, int theY)
  {
    Adjustable aHAdjustable = itsSketchWindow.itsScrollerView.getHAdjustable();
    Adjustable aVAdjustable = itsSketchWindow.itsScrollerView.getVAdjustable();
    boolean hasScrolled = false;

    if ( theX >= aHAdjustable.getVisibleAmount() + aHAdjustable.getValue())
      {
	hasScrolled = true;
	eraseShadows();
	aHAdjustable.setValue( aHAdjustable.getValue() + aHAdjustable.getUnitIncrement());
      }

    if ( theY >= aVAdjustable.getVisibleAmount() + aVAdjustable.getValue())
      {
	hasScrolled = true;
	eraseShadows();
	aVAdjustable.setValue( aVAdjustable.getValue() + aVAdjustable.getUnitIncrement());
      }

    if ( theX <= aHAdjustable.getValue())
      {
	hasScrolled = true;
	eraseShadows();
	aHAdjustable.setValue( aHAdjustable.getValue() - aHAdjustable.getUnitIncrement());
      }

    if ( theY <= aVAdjustable.getValue())
      {
	hasScrolled = true;
	eraseShadows();
	aVAdjustable.setValue( aVAdjustable.getValue() - aVAdjustable.getUnitIncrement());
      }

    if (hasScrolled)
      {
	duringScrolling = true;
      }
    else
      {
	if (duringScrolling)
	  { 
	    eraseShadows();
	  }

	duringScrolling = false;
      }
  }
  
  private void eraseShadows()
  {
    Graphics g = getGraphics();
    CopyTheOffScreen(g);
    g.dispose();
    erased = true;
    erased1 = true;
  }

  private Point startConnectPoint = new Point();
  private Point previousConnectPoint = new Point();
  private Point currentConnectPoint = new Point();


  // ----------------------------------------
  // Mouse methods
  // ----------------------------------------

  private static final int cursorStateNormal = 0;
  private static final int cursorStateHResize = 1;
  private static final int cursorStateVResize = 10;
  private static final int cursorStateInletOutlet = 2;
  private static final int cursorStateControl = 3;
  private static final int cursorStateConnect = 4;
  private int cursorState = cursorStateNormal;

  public void mouseMoved( MouseEvent e)
  {
    cleanAnnotations(); // MDC

    if (isLocked())
      return;

    int x = e.getX();
    int y = e.getY();

    itsCurrentObject = getObjectContaining( x, y);
    
    if (itsCurrentObject != null)
      {
	if ( e.isControlDown())
	  {
	    if (itsCurrentObject.isUIController())
	      {
		if (cursorState != cursorStateControl)
		  {
		    setCursor( Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
		    cursorState = cursorStateControl;
		  }
	      }
	    return;
	  }

	SensibilityArea area = itsCurrentObject.findSensibilityArea( x, y);

	if (area instanceof HResizeSensibilityArea)
	  {
	    if ( cursorState != cursorStateHResize)
	      {
		setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
		cursorState = cursorStateHResize;
	      }
	  }
	else if (area instanceof VResizeSensibilityArea)
	  {
	    if ( cursorState != cursorStateVResize)
	      {
		setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
		cursorState = cursorStateVResize;
	      }
	  }
	else if (area instanceof OutletSensibilityArea)
	  {
	    if ( cursorState != cursorStateInletOutlet)
	      {
		setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
		cursorState = cursorStateInletOutlet;
	      }
	  }
	// final case
	else if (area instanceof NothingSensibilityArea)
	  {
	    if (cursorState != cursorStateNormal)
	      {
		setCursor( Cursor.getDefaultCursor());
		cursorState = cursorStateNormal;
	      }
	  }
      }
    else   // not in an object
      {
	if (cursorState != cursorStateNormal)
	  {
	    setCursor( Cursor.getDefaultCursor());
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
	setCursor( Cursor.getDefaultCursor());
	cursorState = cursorStateNormal;
      }

    cleanAnnotations(); // MDC
    
    if ( isLocked() || e.isControlDown())
      {
	if ( itsStartDragObject != null)
	  itsStartDragObject.MouseDrag( e, x, y);
	return;
      }

    DynamicScrolling( x, y);
    
    switch (editStatus) {
    case START_CONNECT:
      {
	currentConnectPoint.setLocation( x, y);

	Graphics g = getGraphics();
	update( g);
	g.dispose();

	itsCurrentObject = getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	  {
	    SensibilityArea area = itsCurrentObject.findConnectionSensibilityArea( x, y);

	    if (area instanceof InletOutletSensibilityArea)
	      {
		if (itsCurrentObject != itsPotentialConnectionFromObject)
		  {
		    // (***fd) will add highlighting of the in/outlet
		    setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
		    cursorState = cursorStateConnect;

		    return;
		  }
	      }
	  }

	if (cursorState != cursorStateNormal)
	  {
	    setCursor( Cursor.getDefaultCursor());
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
      
	Graphics g = getGraphics();
	update( g);
	g.dispose();

	return;
      }

    case H_RESIZING_OBJECT:
      {
	int prevX = previousResizeRect.x + previousResizeRect.width;

	// (***fd) must resize selection, and maintain a vector of resize rectangles
	itsResizingObject.resizeBy( x - prevX, 0);
	currentResizeRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = getGraphics();
	update( g);
	g.dispose();

	return;
      } 

    case V_RESIZING_OBJECT:
      {
	int prevY = previousResizeRect.y + previousResizeRect.height;

	// (***fd) must resize selection, and maintain a vector of resize rectangles
	itsResizingObject.resizeBy( 0, y - prevY);
	currentResizeRect.setSize( itsResizingObject.getWidth(), itsResizingObject.getHeight());

	Graphics g = getGraphics();
	update( g);
	g.dispose();

	return;
      } 

    case MOVING:
      if ( itsStartInclusionRect.x + (x - itsStartMovingPt.x) >=0 )
	currentMouseX = x;

      if ( itsStartInclusionRect.y + (y - itsStartMovingPt.y) >= 0)
	currentMouseY = y;

      repaint();

      return;
    }
  }

  public void mousePressed( MouseEvent e)
  {
    theToolkit.sync();

    itsSketchWindow.requestFocus();

    if (cursorState != cursorStateNormal 
	&& cursorState != cursorStateHResize
	&& cursorState != cursorStateVResize)
      {
	setCursor( Cursor.getDefaultCursor());
	cursorState = cursorStateNormal;
      }

    int x = e.getX();
    int y = e.getY();

    cleanAnnotations(); // MDC    
    
    if ( !offScreenPresent)
      {
	Graphics g = getGraphics();
	RequestOffScreen();
	DrawOffScreen( g);
	g.dispose();
      }
    
    if ( isLocked() || e.isControlDown()) 
      {
	itsSketchWindow.setKeyEventClient( null);

	itsCurrentObject = getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	  {
	    itsCurrentObject.MouseDown( e, x, y);

	    itsStartDragObject = itsCurrentObject;	//object FROM WHICH we started drag
	  }

	return;
      }
    
    if (  editStatus == EDITING_OBJECT)
      {
	itsEditField.LostFocus();
	requestFocus();
      }

    if ( editStatus == EDITING_COMMENT)
      {
	itsTextArea.LostFocus();
	requestFocus();
      }

    ///if we are in a Object
    itsCurrentObject = getObjectContaining( x, y);

    if ( itsCurrentObject != null)
      {
	if ( itsToolBar.pressed)
	  itsToolBar.resetStaySelected();

	SensibilityArea area = itsCurrentObject.findSensibilityArea( x, y);

	if (area instanceof HResizeSensibilityArea)
	  {
	    SetResizeState( itsCurrentObject, H_RESIZING_OBJECT);
	  }
	else if (area instanceof VResizeSensibilityArea)
	  {
	    SetResizeState( itsCurrentObject, V_RESIZING_OBJECT);
	  }
	else if (area instanceof OutletSensibilityArea)
	  {
	    if ( itsToolBar.pressed)
	      itsToolBar.resetStaySelected();

	    currentSelection.setOwner(this); 
	    currentSelection.deselectAll();

	    editStatus = START_CONNECT;

	    itsPotentialConnectionFromObject = itsCurrentObject;
	    itsPotentialConnectionFromOutletNum = ((OutletSensibilityArea)area).getNumber();
	    prepareForDynamicConnect( ((OutletSensibilityArea)area).getNumber() );
	    
	    paintDirtyList();
	  }
	else
	  {
	    itsCurrentObject.MouseDown( e, x, y);
	  }

	return;
      }

    ///if we are in a Connection
    ErmesConnection connection = getConnectionNear( x, y);

    if (connection != null)
      {
	if (itsToolBar.pressed)
	  itsToolBar.resetStaySelected();
	connection.MouseDown( e,x,y);
	return;
      }

    // We are somewhere else
    if ( !itsToolBar.staySelected())
      itsToolBar.Deselect();
    
    if (editStatus == START_ADD)
      {
	AddingObject( x,y);

	if (!itsToolBar.staySelected())
	  editStatus = DOING_NOTHING;	
      }
    else
      { //DOING_NOTHING, START_SELECT
	if (!e.isShiftDown()) 
	  {
	    resetFocus();
	    currentSelection.setOwner(this); 
	    currentSelection.deselectAll(); // @@@
	    paintDirtyList();
	  }

	editStatus = AREA_SELECT;
	currentRect.setBounds( x,y,0,0);
	previousRect.setBounds( x,y,0,0);
	currentPoint.setLocation( x,y);
      }
  }

  public void mouseReleased( MouseEvent e)
  {
    int x = e.getX();
    int y = e.getY();

    cleanAnnotations(); // MDC    

    if ( duringScrolling)
      duringScrolling=false;

    if ( isLocked() || e.isControlDown())
      {
	if (itsStartDragObject != null) 
	  itsStartDragObject.MouseUp( e, x, y);
	itsStartDragObject = null;

	//all the modifications done with the control down were not reported
	//in the offScreen. Do it now!
	//Optimization: use the dirty lists. Try to avoid to repaint in case
	//of CTRL click on the sketch when nothing was changed "pseudo run mode" 

	if ( !isLocked())
	  repaint();
      }
    else if (editStatus == START_CONNECT)
      {
	itsCurrentObject = getObjectContaining( x, y);

	if ( itsCurrentObject != null)
	    {
	      SensibilityArea area = itsCurrentObject.findConnectionSensibilityArea( x, y);

	      if (area instanceof InletOutletSensibilityArea)
		{
		  // (*** fd) It is actualy forbiden to connect an object to itself in the interaction
		  if (itsPotentialConnectionFromObject != itsCurrentObject)
		    {
		      ErmesConnection aConnection = null;

		      try
			{
			  aConnection = new ErmesConnection( this, 
							     itsPotentialConnectionFromObject,
							     itsPotentialConnectionFromOutletNum,
							     itsCurrentObject,
							     ((InletOutletSensibilityArea)area).getNumber());
			}
		      catch (FtsException exception)
			{
			}

		      if (aConnection != null)
			{
			  aConnection.DoublePaint();
			}
		    }
		}
	    }

	    currentConnectPoint.setLocation( 0, 0);
	    startConnectPoint.setLocation( 0, 0);
	    previousConnectPoint.setLocation( 0, 0);

	    setCursor( Cursor.getDefaultCursor());

	    itsPotentialConnectionFromObject = null;
	    itsPotentialConnectionFromOutletNum = -1;

	    Graphics g = getGraphics();
	    CopyTheOffScreen( getGraphics());
	    g.dispose();

	    editStatus = DOING_NOTHING;
      }
    else if (editStatus == AREA_SELECT)
      {
	Rectangle aRect = NormalizedRect( currentRect);
	if (!aRect.isEmpty())
	  { 
	    Object objects[] = itsElements.getObjectArray();
	    int size = itsElements.size();

	    for (int i = 0; i < size; i++)
	      {
		ErmesObject aObject = (ErmesObject) objects[i];

		if (aObject.getBounds().intersects( aRect))
		  currentSelection.select( aObject);
	      }	

	    CheckCurrentFont();	
	    SelectConnections();
	  }

	if (currentSelection.itsObjects.isEmpty())
	  { 
	    editStatus = DOING_NOTHING;

	    if (!aRect.isEmpty())
	      {
		Graphics g = getGraphics();
		CopyTheOffScreen( g);//a better solution?, repaint maybe ?
		g.dispose();
	      }
	  }
	else
	  {
	    editStatus = START_SELECT;
	    paintDirtyList();
	  }
      }
    else if (editStatus == MOVING)
      {
	int aDeltaH, aDeltaV;

	if ( currentMovingBigRect == null)
	  currentMovingBigRect = new Rectangle( 0, 0, 0, 0);
	else
	  currentMovingBigRect.setBounds( 0,0,0,0);

	if (itsMovingRectangles == null)
	  itsMovingRectangles = new MaxVector();
	else
	  itsMovingRectangles.removeAllElements();

	editStatus = START_SELECT;
	if ( (currentMouseX - itsStartMovingPt.x != 0) || (currentMouseY - itsStartMovingPt.y != 0))
	  {
	    aDeltaH = currentMouseX - itsStartMovingPt.x;
	    aDeltaV = currentMouseY - itsStartMovingPt.y;

	    MoveElements( aDeltaH, aDeltaV);

	    repaint();
	  }
	else
	  {
	    //if not moved

	    if (e.getClickCount() == 1)
	      {
		if (oldEditStatus == START_SELECT)
		  {
		    if (itsCurrentObject instanceof ErmesObjEditable)
		      {
			if (clickHappenedOnAnAlreadySelected) 
			  {
			    resetFocus();
			    currentSelection.setOwner(this); 
			    currentSelection.deselectAll();
			    ((ErmesObjEditable)itsCurrentObject).restartEditing();
			  }
		      }
		  }
	      }
	  }
      }
    else if (editStatus == H_RESIZING_OBJECT || editStatus == V_RESIZING_OBJECT)
      {
	itsResizingObject.MouseUp( e,x,y);
	editStatus = START_SELECT;
	repaint();
      }
    else if (editStatus == DOING_NOTHING)
      {
      }

    theToolkit.sync();
  }

  public void mouseClicked( MouseEvent e)
  {
    cleanAnnotations(); // MDC
  }

  public void mouseEntered( MouseEvent e)
  {
    cleanAnnotations(); // MDC
  } 

  public void mouseExited( MouseEvent e)
  {
    cleanAnnotations(); // MDC
  }


  final ErmesObjEditField GetEditField()
  {
    return itsEditField;
  }
  
  final ErmesObjTextArea GetTextArea()
  {
    return itsTextArea;
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

    itsStartInclusionRect = StartMoveInclusionRect();

    if ( currentSelection.itsObjects.size() < MAX_NUM_SELECTED)
      {
	ErmesObject aObject;
	Rectangle aRect;

	if ( itsMovingRectangles == null)
	  itsMovingRectangles = new MaxVector();

	for ( Enumeration e = currentSelection.itsObjects.elements() ; e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject) e.nextElement();
	    aRect = aObject.getBounds();
	    /* (fd, mdc) the copy of the rectangle is made here, not in Bound */
	    itsMovingRectangles.addElement( new Rectangle( aRect.x, aRect.y, aRect.width - 1, aRect.height - 1));
	  }
      }
  }

  void SelectAll()
  {
    currentSelection.setOwner(this); 
    currentSelection.deselectAll();

    if ( isLocked())
      return;

    Object objects[] = itsElements.getObjectArray();
    int size = itsElements.size();

    for ( int i = 0; i < size; i++)
      currentSelection.select((ErmesObject) objects[i]);

    CheckCurrentFont();
    for ( Enumeration e = itsConnections.elements() ; e.hasMoreElements(); )
      currentSelection.select( (ErmesConnection) e.nextElement());

    // paintDirtyList();

    repaint();
  }

  public void showObject( Object obj)
  {
    // Should select or highlight obj if it is an FtsObject

    if (obj instanceof FtsObject) 
      {
	ErmesObject aObject = getErmesObjectFor((FtsObject) obj);

	if (aObject != null)
	  {
	    currentSelection.setOwner(this); 
	    currentSelection.deselectAll();
	    currentSelection.select( aObject);
	    CheckCurrentFont();
	    paintDirtyList();
	    repaint(); // ?? black magic
	  }
      }
  }


  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
  
  void PrepareInChoice() 
  {
    itsInPop = new ErmesObjInOutPop( itsSketchWindow, itsPatcher.getNumberOfInlets());
    add( itsInPop);
  }

  final void RedefineInChoice()
  {
    itsInPop.Redefine( itsPatcher.getNumberOfInlets());
  }

  void PrepareOutChoice()
  {
    itsOutPop = new ErmesObjInOutPop( itsSketchWindow, itsPatcher.getNumberOfOutlets());
    add( itsOutPop);
  }
  
  void RedefineOutChoice()
  {
    itsOutPop.Redefine( itsPatcher.getNumberOfOutlets());
  }

  public void paint( Graphics g)
  {
    if (deleted) //should be kept?
      return;    // introduced by cvs update

    if ( duringScrolling)
      {
	if (offScreenPresent)
	  {
	    if (paintForTheFirstTime)
	      {
		DrawOffScreen( g);
		paintForTheFirstTime = false;
	      }
	    else
	      {
		erased = true;
		erased1 = true;

		CopyTheOffScreen( g);
	      }
	  }
	else
	  {
	    DrawOffScreen( g);
	  }
      }
    else 
      {
	DrawOffScreen( g);
      }

    // Needed in very heavy charge situations
    theToolkit.sync();
  }		
  
  void SetResizeState( ErmesObject theResizingObject, int newStatus)
  {
    editStatus = newStatus;

    itsResizingObject = theResizingObject;

    currentResizeRect.setBounds( itsResizingObject.getBounds());

    previousResizeRect.setBounds( currentResizeRect);
  }
  
  final void SetStartSelect()
  {
    editStatus = START_SELECT;
  }
  
  final void SetToolBar( ErmesSwToolbar theToolBar)
  {
    itsToolBar = theToolBar;
  }

  final void startAdd( String theDescription)
  {
    resetFocus();
    currentSelection.setOwner(this); 
    currentSelection.deselectAll();
    editStatus = START_ADD;
    itsAddObjectDescription = theDescription;
  }

  private boolean erased = false;
  private boolean erased1 = false;

  public void update( Graphics g)
  {
    // if (deleted || isLocked()) ????????
    // return;

    if (deleted)
      return;

    if (editStatus == START_CONNECT) 
      {
	if (!erased) 
	  {
	    g.setColor( Color.black);
	    g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	    g.drawLine( startConnectPoint.x, startConnectPoint.y, previousConnectPoint.x, previousConnectPoint.y);
	  }
	g.drawLine( startConnectPoint.x, startConnectPoint.y, currentConnectPoint.x, currentConnectPoint.y);
	previousConnectPoint.setLocation( currentConnectPoint);
	erased = false;
      }
    else if (editStatus == AREA_SELECT)
      {
	//faster version
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
      }
    else if ( editStatus == H_RESIZING_OBJECT || editStatus == V_RESIZING_OBJECT)
      {
	//faster version
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
      }
    else if (editStatus == MOVING)
      {
	//faster version
	Rectangle aRect;
	g.setColor( Color.black);
	g.setXORMode( Settings.sharedInstance().getEditBackgroundColor());
	int aNumSelected = currentSelection.itsObjects.size();

	if (aNumSelected<MAX_NUM_SELECTED)
	  {
	    if (!erased1)
	      {
		if (itsMovingRectangles != null)
		  for( Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements(); )
		    {
		      aRect = (Rectangle)e.nextElement();
		      g.drawRect( aRect.x,aRect.y, aRect.width, aRect.height);
		    }
	      }

	    if (itsMovingRectangles != null)
	      for( Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements(); )
		{
		  aRect = (Rectangle)e.nextElement();
		  aRect.x += (currentMouseX - itsPreviousMouseX);
		  aRect.y += (currentMouseY - itsPreviousMouseY);
		  g.drawRect( aRect.x, aRect.y, aRect.width, aRect.height);
		}

	    itsPreviousMouseX = currentMouseX;
	    itsPreviousMouseY = currentMouseY;
	    erased1 = false;
	  }
	else
	  {
	    int oldX, oldY, oldWidth, oldHeight;

	    if ( (currentMovingBigRect != null) && !erased)
	      {
		g.drawRect( currentMovingBigRect.x, 
			    currentMovingBigRect.y, 
			    currentMovingBigRect.width, 
			    currentMovingBigRect.height);
	      }

	    currentMovingBigRect = StartMoveInclusionRect();
	    currentMovingBigRect.x += (currentMouseX-itsStartMovingPt.x);
	    currentMovingBigRect.y += (currentMouseY-itsStartMovingPt.y);

	    g.drawRect( currentMovingBigRect.x, 
			currentMovingBigRect.y, 
			currentMovingBigRect.width, 
			currentMovingBigRect.height);
	    erased = false;
	  }
	return;
      }
    else
      paint (g);
  }

  int itsCurrentScrollingX;
  int itsCurrentScrollingY;

  public void adjustmentValueChanged( AdjustmentEvent e)
  {
    itsCurrentScrollingX = itsSketchWindow.itsScrollerView.getHAdjustable().getValue();
    itsCurrentScrollingY = itsSketchWindow.itsScrollerView.getVAdjustable().getValue();
    
    duringScrolling = true;
  }

  void AlignSelectedObjects( String thePosition)
  {
    ErmesObject aObject;
    int aValue;

    if (thePosition.equals( "Top"))
      {
	aValue = MinYSelected();
	for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject)e.nextElement();
	    aObject.MoveBy( 0, aValue - aObject.getY());
	  }
      }
    else if (thePosition.equals( "Left"))
      {
	aValue = MinXSelected();
	for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject)e.nextElement();
	    aObject.MoveBy( aValue - aObject.getX(), 0);
	  }
      }
    else if (thePosition.equals( "Bottom"))
      {
	aValue = MaxYSelected();
	for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject)e.nextElement();
	    aObject.MoveBy( 0, aValue - (aObject.getY()+aObject.getHeight()) );
	  }
      }
    else if (thePosition.equals( "Right"))
      {
	aValue = MaxXSelected();
	for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
	  {
	    aObject = (ErmesObject)e.nextElement();
	    aObject.MoveBy( aValue - (aObject.getX() + aObject.getWidth()), 0);
	  }
      }

    Graphics g = getGraphics();
    DrawOffScreen( g);//repaint();	  CopyTheOffScreen(g);
    g.dispose();
  }
  
  int MinYSelected()
  {
    ErmesObject aObject;
    int aMinY = 10000;

    for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
      {
	aObject = (ErmesObject)e.nextElement();
	if (aMinY >= aObject.getY())
	  aMinY = aObject.getY();
      }

    return aMinY;
  }
  
  int MinXSelected()
  {
    ErmesObject aObject;
    int aMinX = 10000;

    for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
      {
	aObject = (ErmesObject)e.nextElement();
	if (aMinX >= aObject.getX())
	  aMinX = aObject.getX();
      }

    return aMinX;
  }

  int MaxYSelected()
  {
    ErmesObject aObject;
    int aMaxY = -10000;

    for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
      {
	aObject = (ErmesObject)e.nextElement();
	if (aMaxY < aObject.getY() + aObject.getHeight()) 
	  aMaxY = aObject.getY() + aObject.getHeight();
      }

    return aMaxY;
  }

  int MaxXSelected()
  {
    ErmesObject aObject;
    int aMaxX = -10000;

    for( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements(); )
      {
	aObject = (ErmesObject)e.nextElement();
	if (aMaxX < aObject.getX() + aObject.getWidth()) 
	  aMaxX = aObject.getX() + aObject.getWidth();
      }

    return aMaxX;
  }

  // The following stuff was in ErmesSketchHelper

  //--------------------------------------------------------
  //	AddConnection
  //  adding a connection between two objects
  //--------------------------------------------------------

  final ErmesConnection AddConnection( ErmesObject fromObj, ErmesObject toObj, int fromOutlet, int toInlet, FtsConnection fc) 
  {
    return new ErmesConnection( this, fromObj, toObj, fromOutlet, toInlet, fc);
  }
	
  //--------------------------------------------------------
  //	AddObject
  //  adding an object of the given class name in the given location
  //--------------------------------------------------------
  ErmesObject AddObject( FtsObject theFtsObject)
  {
    ErmesObject aObject;

    aObject = ErmesObject.makeErmesObject( this, theFtsObject);
    itsElements.addElement( aObject);
    return aObject;

  }
  
  //--------------------------------------------------------
  //	ChangeObjectPrecedence
  //--------------------------------------------------------
  // (***fd) is probably messed, because the object should be put
  // at the beginning of the vector, not at the end
  
  final void ChangeObjectPrecedence( ErmesObject theObject)
  {
    // Need the new paint structure to work well,
    // because of a bad interaction with the dirty list.
    //itsElements.removeElement( theObject);
    // itsElements.insertElementAt( theObject, 0);
  }
  
  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one connection routine
  //--------------------------------------------------------

  void DeleteConnection( ErmesConnection theConnection)
  {
    if (theConnection.itsFtsConnection != null)
      theConnection.itsFtsConnection.delete();

    currentSelection.deselect( theConnection);
    itsConnections.removeElement( theConnection);

    markSketchAsDirty();
  }

  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one connection routine
  //--------------------------------------------------------
  void DeleteGraphicConnection( ErmesConnection theConnection, boolean paintNow)
  {
    /* Removing from the selection may be redundant if the deleting was started
       from the UI, but is needed if the deleted has been started by FTS */

    currentSelection.deselect( theConnection);
    itsConnections.removeElement( theConnection);

    markSketchAsDirty();

    if (paintNow)
      paintDirtyList();
  }


  //--------------------------------------------------------
  //	DeleteObject
  //	delete one object routine
  //--------------------------------------------------------
  void DeleteObject( ErmesObject theObject, boolean paintNow) 
  {
    DeleteGraphicObject( theObject, paintNow);

    if (theObject.itsFtsObject != null)
      theObject.itsFtsObject.delete();
  }
  
  void DeleteGraphicObject( ErmesObject theObject, boolean paintNow) 
  {
    //removes theObject from the selected elements list	

    currentSelection.deselect( theObject);

    //removes theObject from the element list (delete)
    itsElements.removeElement( theObject);

    theObject.dispose();

    markSketchAsDirty();
    
    if (paintNow)
      paintDirtyList();
  }
  
  //--------------------------------------------------------
  //	DeleteSelected
  //	delete routine
  //--------------------------------------------------------

  void DeleteSelected()
  {
    while (! currentSelection.itsConnections.isEmpty())
      {
	ErmesConnection aConnection = (ErmesConnection) currentSelection.itsConnections.elementAt( 0);
	DeleteConnection( aConnection);
      }
    
    while (! currentSelection.itsObjects.isEmpty()) 
      {
	ErmesObject aObject = (ErmesObject) currentSelection.itsObjects.elementAt( 0);
	DeleteObject( aObject, false);
      }
      
    GetSketchWindow().DeselectionUpdateMenu();
    paintDirtyList();
  }
    
  //--------------------------------------------------------
  //	cleanAll
  //	Make the cleanup needed before closing the 
  //    sketchpad
  //--------------------------------------------------------

  void cleanAll()
  {
    if (currentSelection.getOwner() == this)
      currentSelection.deselectAll();

    Object[] objects = itsElements.getObjectArray();
    int size       = itsElements.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject object = (ErmesObject) objects[i];

	object.dispose();
      }

    Fts.getServer().removeUpdateGroupListener( this);
    removeMouseMotionListener( this); 
    removeMouseListener( this);
    removeKeyListener( itsSketchWindow);

    remove( itsInPop);
    remove( itsOutPop);
    remove( itsEditField);
    remove( itsTextArea);

    if (lastSketchWithOffScreen == this)
      {
	lastSketchWithOffScreen = null;
      }

    // Clean up to help the gc, and found the bugs.

    itsSketchWindow = null;// should not be needed, here to get the grabber !!
    itsToolBar = null;

    itsElements     = null;
    itsConnections  = null;

    itsMovingRectangles  = null;
    itsPatcher = null;
    itsInPop = null;
    itsOutPop = null;
    itsEditField = null;
    itsTextArea = null;
    dirtyConnections = null;
    dirtyObjects = null;
    anOldPastedObject = null;

    deleted = true;
  }

  //
  // Deselect everything selected in the sketch 
  // (objects, connections, current selected in/outlet).
  // This function handles the Focus change and updates the menus.
  //

  void resetFocus()
  {
    if (GetEditField() != null && GetEditField().HasFocus())
      {
	GetEditField().transferFocus();
      }

    if (editStatus == EDITING_OBJECT)
      {
	GetEditField().LostFocus();
      }
  }

  //--------------------------------------------------------
  //	GetDrawableRect
  //--------------------------------------------------------
  Rectangle GetDrawableRect( Rectangle originalRect, Dimension drawingArea)
  {
    int x = originalRect.x;
    int y = originalRect.y;
    int width = originalRect.width;
    int height = originalRect.height;
    
    //Make sure rectangle width and height are positive.
    if (width < 0)
      {
	width = -width;
	x = x - width + 1;
	if (x < 0)
	  {
	    width += x;
	    x = 0;
	  }
      }
    if (height < 0) 
      {
	height = -height;
	y = y - height + 1;
	if (y < 0)
	  {
	    height += y;
	    y = 0;
	  }
      }
    
    //The rectangle shouldn't extend past the drawing area.
    if ((x + width) > drawingArea.width)
      {
	width = drawingArea.width - x;
      }

    if ((y + height) > drawingArea.height)
      {
	height = drawingArea.height - y;
      }
    
    return new Rectangle( x, y, width, height);
  }
  
  
  //--------------------------------------------------------
  //	getConnectionNear
  //--------------------------------------------------------

  // This function substitute IsInConnection; predicates should
  // not have side effect; by returning directly the connection
  // found we use one member variable less, and the code is more
  // readeble, because all the effects are explicit

  ErmesConnection getConnectionNear( int x, int y)
  {
    ErmesConnection aConnection;
    Rectangle aRect;

    for( Enumeration e = itsConnections.elements(); e.hasMoreElements();)
      {
	aConnection = (ErmesConnection)e.nextElement();
	if (aConnection.IsNearToPoint( x, y))
	  return aConnection;
      }

    return null;
  }
  
  //--------------------------------------------------------
  //	getObjectContaining
  //--------------------------------------------------------
  // Same remark as previous method
  ErmesObject getObjectContaining( int x, int y)
  {
    Object objects[] = itsElements.getObjectArray();
    int size = itsElements.size();

    for ( int i = 0; i < size; i++)
      {
	ErmesObject aObject = (ErmesObject) objects[i];

	if (aObject.getBounds().contains( x,y))
	  {
	    return aObject;
	  }
      }

    return null;
  }
  
  //--------------------------------------------------------
  //	MoveElements
  //	Move the selected elements
  //--------------------------------------------------------
  void MoveElements( int theDeltaH, int theDeltaV)
  {
    ErmesObject aObject;

    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for (int i = 0; i < size; i++)
      {
	aObject = (ErmesObject) objects[i];
	aObject.MoveBy( theDeltaH, theDeltaV);
      }
  }

  //--------------------------------------------------------
  //	StartMoveInclusionRect
  //--------------------------------------------------------
  Rectangle StartMoveInclusionRect()
  {
    int top, left, bottom, right;
    ErmesObject aObject;

    left = Integer.MAX_VALUE;
    top = Integer.MAX_VALUE;
    bottom = 0;
    right = 0;

    // Note that in the original code the rectangle have the following
    // offsets:
    //     itsArea.setBounds(currentRect.x-3, currentRect.y-3, currentRect.width+6, currentRect.height+6);

    for( int i = 0; i < currentSelection.itsObjects.size(); i++)
      {
	aObject = (ErmesObject) currentSelection.itsObjects.elementAt( i);
	
	Rectangle aRect = aObject.getBounds();

	if (aRect.x < left)
	  left = aRect.x;

	if (aRect.y < top)
	  top = aRect.y;

	if ( aRect.y + aRect.height > bottom)
	  bottom = aRect.y + aRect.height;

	if ( aRect.x  + aRect.width > right)
	  right = aRect.x + aRect.width;
      }

    return new Rectangle( left, top, right-left, bottom-top);
  }
	
  //
  // an utility function to find the upper-left corner of a selection
  //
  Point selectionUpperLeft()
  {
    int minX;
    int minY;
    ErmesObject aObject;    

    if (currentSelection.itsObjects.size() == 0)
      return new Point( 0,0);

    aObject = (ErmesObject)currentSelection.itsObjects.elementAt( 0);

    minX = aObject.getX();
    minY = aObject.getY();

    for( int i=1; i < currentSelection.itsObjects.size(); i++)
      {
	aObject = (ErmesObject) currentSelection.itsObjects.elementAt( i);
	if (aObject.getX() < minX)
	  minX = aObject.getX();
	if (aObject.getY() < minY)
	  minY = aObject.getY();
      }
    
    return new Point( minX, minY);
  }


  // Experimental MDC

  private boolean annotating = false;

  void setAnnotating()
  {
    annotating = true;
  }

  void showErrorDescriptions()
  {
    if (! annotating)
      {
	ErmesObject aObject;

	if (currentSelection.itsObjects.size() == 0)
	  {
	    Object objects[] = itsElements.getObjectArray();
	    int size = itsElements.size();

	    for ( int i = 0; i < size; i++)
	      ((ErmesObject) objects[i]).showErrorDescription();
	  }
	else
	  for ( Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();)
	    {
	      aObject = (ErmesObject) e.nextElement();
	      aObject.showErrorDescription();
	    }
      }
  }

  void cleanAnnotations()
  {
    if (annotating)
      {
	annotating = false;
	Graphics g = getGraphics();
	paint( g);
	g.dispose();
      }
  }

  // The waiting/stopWaiting service

  private int waiting = 0;

  void waiting()
  {
    if (waiting >= 0)
      setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    waiting++;
  }

  void stopWaiting()
  {
    waiting--;

    if (waiting <= 0)
      setCursor( Cursor.getDefaultCursor());
  }

  // ----------------------------------------------------------------------
  // Mode handling
  // ----------------------------------------------------------------------
  private boolean locked = false;

  protected void setLocked( boolean locked)
  {
    this.locked = locked;

    if (locked)
      {
	setBackground( Settings.sharedInstance().getLockBackgroundColor());

	if (currentSelection.getOwner() == this)
	  currentSelection.deselectAll();
      }
    else
      setBackground( Settings.sharedInstance().getEditBackgroundColor());
  }

  protected final boolean isLocked()
  {
    return locked;
  }
}
