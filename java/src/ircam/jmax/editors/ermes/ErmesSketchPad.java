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



/**
 * The graphic workbench for the patcher editor.
 * It handles the interaction of the user with the objects,
 * propagates the mouse events to the objects themselves,
 * allow the selection, moving, erasing, resizing of objects.
 * It keeps track of the toolbar state, it handles the 
 * offscreen and much, much more...
 * 
 * Merged Back with the ErmesSketchHelper
 */

class ErmesSketchPad extends Panel implements AdjustmentListener,
  MouseMotionListener, MouseListener, FtsUpdateGroupListener
{
  // The element list is implemented as a array, whose dimension
  // is doubled on reallocation; two methods are provided to add an element
  // and to remove an element; the access is done directly to the array
  // it is not implemented as a nested class to try to reduce to the minimum
  // every indirection in its use.

  static Toolkit theToolkit = Toolkit.getDefaultToolkit();

  MaxVector itsElements = new MaxVector();

  // Was in the helper 

  Rectangle normalizedRect = new Rectangle();  

  //--------------------------------------------------------
  //	NormalizedRect
  //--------------------------------------------------------

  private final Rectangle NormalizedRect(Rectangle theRect) {
    
    normalizedRect.x = (theRect.width < 0)? theRect.x + theRect.width: theRect.x;
    normalizedRect.y = (theRect.height < 0)? theRect.y +theRect.height: theRect.y;
    normalizedRect.width = (theRect.width < 0)? - theRect.width: theRect.width;
    normalizedRect.height = (theRect.height < 0)? - theRect.height: theRect.height;
    
    return normalizedRect;
  }
  
  //2703...
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

    if (drawPending) {
      Graphics g = getGraphics();
      DrawOffScreen(g);
      g.dispose();
    }
    else if (copyPending) {
      Graphics g = getGraphics();
      CopyTheOffScreen(g);
      g.dispose();
    }
    drawPending = false;
    copyPending = false;

    theToolkit.sync();
  }


  ErmesSketchWindow itsSketchWindow;

  private final static int SKETCH_WIDTH = 1200;
  private final static int SKETCH_HEIGHT = 1200;
  static Dimension preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);

  final static int DOING_NOTHING = 0;		
  final static int START_ADD 	 = 1;
  final static int START_SELECT  = 2;
  final static int AREA_SELECT 	 = 3;		
  final static int MOVING 	 = 4;
  final static int MOVINGSEGMENT    = 5;	
  final static int RESIZING_OBJECT  = 6;
  final static int EDITING_OBJECT   = 7;
  final static int EDITING_COMMENT  = 8;
  final static int FromOutToIn 	    = 1;
  final static int FromInToOut 	    = -1;
  final static int NoDirections     = 0;
  final static int CENTER_JUSTIFICATION = 20;
  final static int LEFT_JUSTIFICATION = 21;
  final static int RIGHT_JUSTIFICATION = 22;
  final static int BOTH_RESIZING = 23;
  final static int HORIZONTAL_RESIZING = 24;
  final static int VERTICAL_RESIZING = 25;
  final static int START_CONNECT = 26;

  final static Dimension snapGrid = new Dimension(30, 50);
  final static Color sketchColor = new Color(230, 230, 230);	//the sketchPad gray...

  Font sketchFont = new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE);
  int sketchFontSize = ircam.jmax.utils.Platform.FONT_SIZE;

  private final static int MAX_NUM_SELECTED = 10;

  int itsJustificationMode = LEFT_JUSTIFICATION;
  int itsResizeMode = BOTH_RESIZING;

  ErmesObjInOutPop itsInPop = null;
  ErmesObjInOutPop itsOutPop = null;
		
  private int itsDirection = NoDirections;
  boolean itsRunMode = false;
  private boolean doSnapToGrid = false;
  boolean itsGraphicsOn = true;
  private boolean paintForTheFirstTime = true;
  
  private ErmesObjEditField itsEditField = null;
  private ErmesObjTextArea itsTextArea = null;
  private ErmesObject itsConnectingObj = null;  
  private ErmesObjInOutlet itsConnectingLet = null;
  private int currentMouseX, currentMouseY;	// used during the MOVING status
  private int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  
  MaxVector itsConnections;
  MaxVector itsInletList;
  MaxVector itsOutletList;
  static ErmesSelection currentSelection;
  static FtsClipboard ftsClipboard;
  Rectangle currentRect = new Rectangle();
  private Rectangle previousRect = new Rectangle();
  private Point	currentPoint = new Point();
  private Rectangle currentResizeRect = null;
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

  private ErmesObjInOutlet itsCurrentInOutlet = null;
  private ErmesObject itsCurrentObject = null;
  private ErmesObject itsStartDragObject = null;
  private ErmesObject itsResizingObject = null;

  // This is an housekeeping structure to implement the re-edit of an
  // editable object. The logic is:
  // 1) click on an object selects the object and go in MOVING status
  // 2) release the mouse (moves if moved), go in START_SELECT
  // 3) second click select the object, go in MOVING
  // 4) second release re-edit the object only if the first click was on the same, already selected object.
  boolean clickHappenedOnAnAlreadySelected = false;

  private MaxVector itsConnectingLetList;

  int editStatus = DOING_NOTHING;
  private int oldEditStatus = DOING_NOTHING;

  private String itsAddObjectDescription;
  boolean itsScrolled = false;
  private MaxVector dirtyInOutlets;
  private MaxVector dirtyConnections;
  private MaxVector dirtyObjects;
  private boolean dirtySketch = false;

  void addToDirtyInOutlets(ErmesObjInOutlet theInOutlet) {
    if (dirtySketch) return;

    if ((dirtyInOutlets != null) && (! dirtyInOutlets.contains(theInOutlet)))
      dirtyInOutlets.addElement(theInOutlet);
    else
      {
	dirtyInOutlets = new MaxVector();
	dirtyInOutlets.addElement(theInOutlet);
      }
  }

  void addToDirtyConnections(ErmesConnection theConnection) {
    if (dirtySketch) return;

    if ((dirtyConnections != null) && (! dirtyConnections.contains(theConnection)))
      dirtyConnections.addElement(theConnection);
    else
      {
	dirtyConnections = new MaxVector();
	dirtyConnections.addElement(theConnection);
      }
  }

  void addToDirtyObjects(ErmesObject theObject) {
    if (dirtySketch) return;
    if ((dirtyObjects != null) && (! dirtyObjects.contains(theObject)))
      dirtyObjects.addElement(theObject);
    else
      {
	dirtyObjects = new MaxVector();
	dirtyObjects.addElement(theObject);
      }
  }

  void markSketchAsDirty() {
    dirtySketch = true;
  }

  void removeDirtyInOutlet(ErmesObjInOutlet io) {
    if (dirtyInOutlets != null)
      dirtyInOutlets.removeElement(io);
  }

  private void emptyDirtyLists() {
    if (dirtyInOutlets != null)
      dirtyInOutlets.removeAllElements();

    if (dirtyObjects != null)
      dirtyObjects.removeAllElements();

    if (dirtyConnections != null)
      dirtyConnections.removeAllElements();

    dirtySketch = false;
  }

  /**
   * At this level, we can implement a politic of drawing.
   * (example, if there are not objects to paint, paint connections and
   * in/outlets directly, without using CopyTheOffScreen())
   */
  void paintDirtyList() {
    if (offGraphics == null) GetOffGraphics();
    if (dirtySketch) {
      Graphics g = getGraphics();
      DrawOffScreen(g);
      g.dispose();
      //repaint();
      emptyDirtyLists();
      return;
    }
    else {

      if (dirtyInOutlets != null)
	{
	  paintList(dirtyInOutlets, offGraphics);
	  dirtyInOutlets.removeAllElements();
	}
      //--
      if (!itsRunMode) { //objects UNDER connections 

	if (dirtyObjects != null)
	  {
	    paintList(dirtyObjects, offGraphics);
	    dirtyObjects.removeAllElements();
	  }

	if (dirtyConnections != null)
	  {
	    paintList(dirtyConnections, offGraphics);
	    dirtyConnections.removeAllElements();
	  }
      }
      else  { // connections UNDER objects
	if (dirtyConnections != null)
	  {
	    paintList(dirtyConnections, offGraphics);
	    dirtyConnections.removeAllElements();
	  }

	if (dirtyObjects != null)
	  {
	    paintList(dirtyObjects, offGraphics);
	    dirtyObjects.removeAllElements();
	  }
      }
      //--
      Graphics g = getGraphics();
      CopyTheOffScreen(g);
      g.dispose();

      //emptyDirtyLists is done in CopyTheOffScreen()
    }
  }
  

  private void paintList(MaxVector theList, Graphics theContext) {
    if (theContext == null) return;

    int size = theList.size();

    if (theList.size()!= 0)
      {
	Object[] objects = theList.getObjectArray();

	for (int i = 0; i < size; i++)
	  ((ErmesDrawable) objects[i]).Paint(theContext);
      }
  }

  
  final Font getSketchFont() {
    return sketchFont;
  }

  final int getSketchFontSize() {
    return sketchFontSize;
  }

  void ChangeFont(Font theFont){
    try {
      FontMetrics aFontMetrics = theToolkit.getFontMetrics(theFont);
      setFont(theFont);   
    }
    catch (Exception e) {
      new ErrorDialog(itsSketchWindow, "This font/fontsize does not exist on this platform");
      return;
    }
    sketchFont = theFont;
  }
  
  void ChangeNameFont(String theFontName){
    Font aFont;

    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for (int i = 0; i < size; i++) {
      ErmesObject aObject = (ErmesObject) objects[i];

      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(theFontName, sketchFont.getStyle(), aObject.getFont().getSize());

	try {
	  FontMetrics aFontMetrics = theToolkit.getFontMetrics(aFont);
	  setFont(aFont);   
	}
	catch (Exception exc) {
	  ErrorDialog aErr = new ErrorDialog(itsSketchWindow, "This font/fontsize does not exist on this platform");
	  aErr.setLocation(100, 100);
	  aErr.show();  
	  return;
	}
	
	aObject.setFont(aFont);
      }
    }
    repaint();
  }

  void ChangeSizeFont(int fontSize){
    ErmesObject aObject;
    Font aFont;

    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for (int i = 0; i < size; i++)    {
      aObject = (ErmesObject) objects[i];

      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(aObject.getFont().getName(), sketchFont.getStyle(), fontSize);
	try {
	  FontMetrics aFontMetrics = theToolkit.getFontMetrics(aFont);
	  setFont(aFont);   
	}
	catch (Exception exc) {
	  ErrorDialog aErr = new ErrorDialog(itsSketchWindow, "This font/fontsize does not exist on this platform");
	  aErr.setLocation(100, 100);
	  aErr.show();  
	  return;
	}
	
	aObject.setFont(aFont);
      }
    }
    repaint();
  }

  void inspectSelection() {
    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for (int i = 0; i < size; i++) {
      ErmesObject aObject = (ErmesObject) objects[i];
      aObject.inspect();
    }
  }

  void ChangeJustification(String theJustification){
    int aJustificationMode = 0;
    if(theJustification.equals("Center")) aJustificationMode = CENTER_JUSTIFICATION;
    else if(theJustification.equals("Left")) aJustificationMode = LEFT_JUSTIFICATION;
    else if(theJustification.equals("Right")) aJustificationMode = RIGHT_JUSTIFICATION;

    if (currentSelection.itsObjects.size()==0)
      itsJustificationMode = aJustificationMode;
    else
      {
	Object[] objects = currentSelection.itsObjects.getObjectArray();
	int size = currentSelection.itsObjects.size();

	for (int i = 0; i < size; i++) {
	  ErmesObject aObject = (ErmesObject) objects[i];

	  if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment))
	    aObject.setJustification(aJustificationMode);
	}
      }

    repaint();
  }

  void ChangeResizeMode(int theResizeMode){
    itsResizeMode = theResizeMode;
  }

  //--------------------------------------------------------
  //	ClickOnConnection
  //--------------------------------------------------------

  void ClickOnConnection(ErmesConnection connection, MouseEvent evt,int x, int y)
  {
    switch(editStatus) {
    case START_ADD:
      break;
    case DOING_NOTHING:
      deselectCurrentInOutlet(false);

      currentSelection.addConnection(connection); 
      connection.Select(false);
      paintDirtyList();
      editStatus = START_SELECT;///////
      
      break;
    case START_SELECT:
      if (!evt.isShiftDown()) {//lo shift non e' premuto
	deselectAll(false);
	currentSelection.addConnection(connection); 

	connection.Select(false);
	paintDirtyList();
      }
      else{
	if(!(currentSelection.itsConnections.contains(connection))){
	  currentSelection.addConnection(connection); 
	  connection.Select(true);
	}	
	else {
	  currentSelection.removeConnection(connection);
	  connection.Deselect(false);
	  
	  if((currentSelection.itsConnections.size()) == 0)
	    editStatus = DOING_NOTHING;
	  paintDirtyList();
	}
      }	
      break;	
    }
  }

	
  //--------------------------------------------------------
  //	ClickOnObject
  //--------------------------------------------------------

  void ClickOnObject(ErmesObject theObject, MouseEvent evt, int theX, int theY){
    
    if(!itsRunMode){
      switch(editStatus) {
      case START_ADD:
	break;
      case DOING_NOTHING:
	deselectCurrentInOutlet(false);
	currentSelection.addObject(theObject);
	theObject.Select(false);
	CheckCurrentFont();
	MoveSelected(theX,theY);
	paintDirtyList();
	break;
      case START_SELECT:
	if (evt.isShiftDown()) {
	  if(!currentSelection.itsObjects.contains(theObject)){
	    currentSelection.addObject(theObject);
      	    theObject.Select(true);
	    CheckCurrentFont();
	  }
	  else{
	    currentSelection.removeObject(theObject);	
	    theObject.Deselect(true);
	    if(currentSelection.itsObjects.isEmpty()) 
	      editStatus = DOING_NOTHING;
	  }
	}
	else if(theObject.itsSelected) {
	  MoveSelected(theX, theY);
	}
	else { 
	  deselectAll(true);
	  currentSelection.addObject(theObject);
	  theObject.Select(true);
	  CheckCurrentFont();
	  MoveSelected(theX,theY);
	}
	break;
      }
      ChangeObjectPrecedence(theObject);
    }
  }
  
  static int noOfCopy = 0;
  void CopyTheOffScreen(Graphics g) {
    //System.err.println(""+(noOfCopy++));
    /*System.err.println("--copy chiamato da");
    (new Throwable()).printStackTrace();*/
    if((g!= null)&&(offScreenPresent)) {
      if (isInGroup || copyPending) {
	copyPending = true;
      }
      else {
	g.drawImage(offImage,0,0, this);
	emptyDirtyLists();
      }    
    }
  }

  //--------------------------------------------------------
  //	DoNothing
  //	message received from the ToolBar when an object is DEselected
  //--------------------------------------------------------
  final void DoNothing() {
    editStatus = DOING_NOTHING;
  }
  
  void DrawLinesOffScreen() {	//now it checks if we have the offGraphics...

    Graphics g = getGraphics();
    Object[] objects = currentSelection.itsConnections.getObjectArray();
    int size = currentSelection.itsConnections.size();

    for (int i = 0; i < size; i++) {
      ErmesConnection aConnection = (ErmesConnection) objects[i];
      
      if (offScreenPresent) {
	aConnection.Paint(offGraphics);
      }
      else 
	if (g != null)
	  aConnection.Paint(g);
    }

    if (offScreenPresent)
      CopyTheOffScreen(g); 

    g.dispose();
  }
  
  void DrawOffScreen(Graphics g) {
    if (drawPending || isInGroup) {
      drawPending = true;
      return;
    }
    
    if (editStatus == AREA_SELECT) {	//we are only painting the selection rect
      return;
    }
    
    Dimension d = getPreferredSize();
    
    //Create the offscreen graphics context, if no good one exists.
    if ( (offGraphics == null)){	//first sketch created, allocate the offscreen buffer
      offDimension = d;
      offImage = createImage(d.width, d.height);
      offGraphics = offImage.getGraphics();
    }
    else //we already created an offscreen. To who it belongs?
      RequestOffScreen(this);
    if((d.width != offDimension.width)||(d.height != offDimension.height)){
      // This code is never executed, because getPreferredSize is a constant.

      offDimension = d;
      //creare un nuovo offGraphics con le nuove dimensioni
      //e copiarci dentro l'immagine del vecchio
      Image oldOffImage = offImage;
      offImage = createImage(d.width, d.height);
      offGraphics = offImage.getGraphics();
      offGraphics.drawImage(oldOffImage, 0, 0, this);
    }

    offGraphics.setFont(getFont());
    offGraphics.setColor(getBackground());
    offGraphics.fillRect(0, 0, d.width, d.height);	//prepare the offscreen to be used by me
    offGraphics.setColor(Color.black);
    //draw connections

    Object[] objects;
    int size;

    objects = itsElements.getObjectArray();
    size = itsElements.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject aObject = (ErmesObject) objects[i];

	Object[] inouts;
	int inouts_size;

	inouts = aObject.GetInletList().getObjectArray();
	inouts_size = aObject.GetInletList().size();

	for (int j = 0; j < inouts_size; j++)
	  ((ErmesObjInlet) inouts[j]).Paint(offGraphics);

	inouts = aObject.GetOutletList().getObjectArray();
	inouts_size = aObject.GetOutletList().size();

	for (int j = 0; j < inouts_size; j++)
	  ((ErmesObjOutlet) inouts[j]).Paint(offGraphics);

	//edit mode=objects BEFORE connections
	if (! itsRunMode)
	  aObject.Paint(offGraphics);
      }

    objects = itsConnections.getObjectArray();
    size = itsConnections.size();

    for (int i = 0; i < size; i++)
      ((ErmesConnection) objects[i]).Paint(offGraphics);

    if (itsRunMode)
      {
	objects = itsElements.getObjectArray();
	size = itsElements.size();

	for (int i = 0; i < size; i++)
	  ((ErmesObject) objects[i]).Paint(offGraphics);
      }

    CopyTheOffScreen(g);
  }
  
  //--------------------------------------------------------
  //	GetPatcherElements
  // 
  //--------------------------------------------------------

  final Graphics GetOffGraphics() {
    Dimension d = preferredSize;
    
    //Create the offscreen graphics context, if no good one exists.
    if ( (offGraphics == null)){	//first sketch of the day. We do this even if it doesn't belong
      offDimension = d;
      offImage = createImage(d.width, d.height);
      if (offImage != null) {		//this can happen...
      	offGraphics = offImage.getGraphics();	
	RequestOffScreen(this);	//a call to this function change the offscreen property.
      }
    }
    return offGraphics;
  }
  
  //--------------------------------------------------------
  //	GetSketchWindow
  //--------------------------------------------------------
  
  final ErmesSketchWindow GetSketchWindow(){
    return itsSketchWindow;
  }
  
  private int incrementalPasteOffsetX;
  private int incrementalPasteOffsetY;
  Point pasteDelta = new Point();
  int numberOfPaste = 0;
  private FtsObject anOldPastedObject;
  
  // note: the following function is a reduced version of InitFromFtsContainer.
  // better organization urges
  void PasteObjects(MaxVector objectVector, MaxVector connectionVector) {
    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    ErmesConnection aConnection;

    int objectX;    
    int objectY;
    int minX=0;
    int minY=0;

    numberOfPaste += 1;
    deselectAll(false);

    if (objectVector == null) return;
    fo = (FtsObject)objectVector.elementAt(0);
    minX = fo.getX();
    minY = fo.getY();
    if (numberOfPaste == 1) {
      anOldPastedObject = fo;
      incrementalPasteOffsetX = 20;
      incrementalPasteOffsetY = 20;
    }
    else if (numberOfPaste == 2) {
      
      incrementalPasteOffsetX = (anOldPastedObject.getX() - minX);
      incrementalPasteOffsetY = (anOldPastedObject.getY() - minY);
    }

    for (Enumeration waste = objectVector.elements(); waste.hasMoreElements();) {
      fo = (FtsObject)waste.nextElement();
      objectX = fo.getX();
      objectY = fo.getY();
      if (objectX < minX) minX = objectX;
      if (objectY < minY) minY = objectY;
    }

    for (Enumeration e = objectVector.elements(); e.hasMoreElements();) {
      fo = (FtsObject)e.nextElement();

      objectX = fo.getX();
      objectY = fo.getY();

      fo.put("x", objectX-minX+itsCurrentScrollingX+pasteDelta.x+numberOfPaste*incrementalPasteOffsetX);     
      fo.put("y", objectY-minY+itsCurrentScrollingY+pasteDelta.y+numberOfPaste*incrementalPasteOffsetY);
       
      aObject = AddObject(fo);
      currentSelection.addObject(aObject);
      aObject.Select(false);
       
      if (aObject != null) fo.setRepresentation(aObject);
    }

    // chiama tanti AddConnection...

    ErmesObject fromObj, toObj;
    
    for (Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) {
      fc = (FtsConnection)e2.nextElement();

      fromObj = (ErmesObject) fc.getFrom().getRepresentation();
      toObj = (ErmesObject) fc.getTo().getRepresentation();
      aConnection = AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
      currentSelection.addConnection(aConnection);
      aConnection.Select(false);
    }
    editStatus = START_SELECT;
    paintDirtyList();
  }
  
  
  void InitFromFtsContainer(FtsContainerObject theContainerObject)
  {
    FtsContainerObject aFtsPatcher = theContainerObject;
    MaxVector objectVector = aFtsPatcher.getObjects();	//usefull?
    FtsObject	fo;
    FtsConnection fc;

    for (Enumeration e = objectVector.elements(); e.hasMoreElements();)
      AddObject((FtsObject)e.nextElement());
		
    // chiama tanti AddConnection...
    MaxVector connectionVector = aFtsPatcher.getConnections();	//usefull?
    ErmesObject fromObj, toObj;
    ErmesConnection aConnection = null;
    
    for (Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();)
      {
	fc = (FtsConnection)e2.nextElement();

	fromObj = (ErmesObject) fc.getFrom().getRepresentation();
	toObj = (ErmesObject) fc.getTo().getRepresentation();
	aConnection = AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
      }

    repaint();
  }

  //--------------------------------------------------------
  //	InletConnect
  //--------------------------------------------------------
  
  void InletConnect(ErmesObject theObject, ErmesObjInOutlet theRequester) {	
    ErmesObjInlet aInlet;
    if (itsDirection == NoDirections) {
      itsConnectingObj = theObject;
      itsConnectingLet = theRequester;
      itsDirection = FromInToOut;
      theRequester.ChangeState(true, theRequester.connected, false);
    }
    else if (itsDirection == FromInToOut) {
      //deselection of old inlet
      if(itsConnectingLetList.size()!=0){
	for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	  aInlet = (ErmesObjInlet)e.nextElement();
	  aInlet.ChangeState(false, aInlet.connected, false);
	}
	ResetConnect();
      }
      else {
	itsConnectingObj.ConnectionAbort(itsConnectingLet, false);
	if (editStatus == START_CONNECT)
	  {
	    Graphics g = getGraphics();
	    CopyTheOffScreen(g);//to erase the pending line
	    g.dispose();
	  }
      }

      if(theRequester!=itsConnectingLet){
	//selection of new outlet
	itsConnectingObj = theObject;
	itsConnectingLet = theRequester;
	itsDirection = FromInToOut;
	theRequester.ChangeState(true, theRequester.connected, false);
      }
      //else nothing to do
    }
    else {// FromOutToIn
      if(itsConnectingLetList.size()!=0)
	MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true, false);
	theRequester.ChangeState(false, true/*false*/, false);
	TraceConnection((ErmesObjOutlet)itsConnectingLet, (ErmesObjInlet)theRequester, false);
	ResetConnect();
      }
    }

    editStatus = DOING_NOTHING;
    paintDirtyList();
  }
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------

  ErmesSketchPad(ErmesSketchWindow theSketchWindow) {    
    super();
    Fts.getServer().addUpdateGroupListener(this);
    setLayout(null);
    itsSketchWindow = theSketchWindow;
    itsConnections = new MaxVector();
    itsInletList = new MaxVector();
    itsOutletList = new MaxVector();
    if (currentSelection==null) currentSelection = new ErmesSelection(Fts.getSelection());
    try {
      if (ftsClipboard==null)
	ftsClipboard = (FtsClipboard) Fts.makeFtsObject(Fts.getServer().getRootObject(), "__clipboard");
    }
    catch (FtsException e) {
      System.out.println("warning: failed to create an Fts clipboard");
    }
    itsStartMovingPt = new Point(0,0);    
    itsStartInclusionRect = new Rectangle();  
    itsConnectingLetList = new MaxVector();
        
    itsEditField = new ErmesObjEditField(this);
    add(itsEditField);

    itsEditField.setVisible(false);
    itsEditField.setLocation(-200,-200);
    
    itsTextArea = new ErmesObjTextArea(this);
    add(itsTextArea);

    itsTextArea.setVisible(false);
    itsTextArea.setLocation(-200,-200);

    setBackground(sketchColor);
    addMouseMotionListener(this); 
    addMouseListener(this);
    addKeyListener(itsSketchWindow);

    
    // Initialization of the "fts class"  to "graphic object" table
  }
	
  static  void RequestOffScreen(ErmesSketchPad theSketchPad) {
    if (lastSketchWithOffScreen == theSketchPad || theSketchPad.offScreenPresent) return;
    if (lastSketchWithOffScreen!=null) {
      if (!lastSketchWithOffScreen.itsRunMode)
	lastSketchWithOffScreen.deselectAll(true);
      lastSketchWithOffScreen.offScreenPresent = false;
    }
    theSketchPad.offScreenPresent = true;
    lastSketchWithOffScreen = theSketchPad;
    theSketchPad.paintForTheFirstTime = true;
  }
  
  void CheckCurrentFont(){
    ErmesObject aObject;
    String aFontName;
    Integer aSize;
    Integer aJustification;
    MaxVector aUsedFontVector = new MaxVector();
    MaxVector aUsedSizeVector = new MaxVector();
    MaxVector aUsedJustificationVector = new MaxVector();
    for (Enumeration en = currentSelection.itsObjects.elements(); en.hasMoreElements();) {
      aObject = (ErmesObject)en.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)){
	aFontName = aObject.getFont().getName().toLowerCase();
	aSize = new Integer(aObject.getFont().getSize());
	aJustification = new Integer(aObject.getJustification());
	if(!aUsedFontVector.contains(aFontName)) aUsedFontVector.addElement(aFontName);
	if(!aUsedSizeVector.contains(aSize)) aUsedSizeVector.addElement(aSize);
	if(!aUsedJustificationVector.contains(aJustification)) 
	  aUsedJustificationVector.addElement(aJustification);
      }
    }
    if(aUsedFontVector.size()!=0){
      if(aUsedFontVector.size()==1) aFontName = (String) aUsedFontVector.elementAt(0);
      else aFontName = null;
      
      if(aUsedSizeVector.size()==1) aSize=(Integer)aUsedSizeVector.elementAt(0);
      else aSize = null;
      
      if(aUsedJustificationVector.size()==1)
	aJustification =(Integer)aUsedJustificationVector.elementAt(0);
      else aJustification = null;
      
      itsSketchWindow.SelectionUpdateMenu(aFontName, aSize, aJustification);
    }
  }


  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------

  static final private Dimension minSize = new Dimension(30, 20);

  public Dimension getMinimumSize() {
    return minSize;
  }
	
  void AddingObject(int x, int y)
  {
    FtsObject fo;
    ErmesObject aObject;

    if (doSnapToGrid)
      {
	Point aPoint = SnapToGrid(x, y);
	x = aPoint.x;
	y = aPoint.y;
      }

    try
      {
	fo = Fts.makeFtsObject(itsSketchWindow.itsPatcher, itsAddObjectDescription);

	fo.put("x", x);
	fo.put("y", y);
	fo.localPut("x", x);
	fo.localPut("y", y);

	aObject = AddObject(fo);

	repaint(); // ??

	if (aObject instanceof ErmesObjEditable)
	  ((ErmesObjEditable)aObject).startEditing();
      }
    catch (FtsException ftse)
      {
	System.err.println("ErmesSketchPad:mousePressed: INTERNAL ERROR: FTS Instantiation Error: " + ftse);
      }
  }


  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio

  public void mousePressed(MouseEvent e)
  {
    theToolkit.sync();

    itsSketchWindow.requestFocus();

    int x = e.getX();
    int y = e.getY();
    int i;

    cleanAnnotations(); // MDC    
    
    if ( !offScreenPresent)
      {
	Graphics g = getGraphics();
	RequestOffScreen(this);
	DrawOffScreen(g);
	g.dispose();
      }
    
    if ( itsRunMode || e.isControlDown()) 
      {
	// (fd) {
	itsSketchWindow.setKeyEventClient( null);
	// } (fd)

	if ( IsInObject(x,y))
	  {
	    itsCurrentObject.MouseDown(e,x,y);
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

    ///if we are in a InOutLet
    if ( IsInInOutLet(x,y))
      {
	if(itsToolBar.pressed)
	  itsToolBar.Unlock();

	deselectObjects( currentSelection.itsObjects, false);
	deselectConnections( currentSelection.itsConnections, false);
	currentSelection.removeAllElements();

	if ( e.isShiftDown())
	  {
	    MultiConnect(itsCurrentInOutlet);
	  }
	else
	  {
	    if ( !itsCurrentInOutlet.GetSelected() )
	      {// no previously selected
		itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
		if ( itsConnectingLet != null) 
		  { 
		    // we are going to START a connection, not to terminate one !!
		    editStatus = START_CONNECT;
		    prepareForDynamicConnect( itsCurrentInOutlet);
		  }
	      }
	    else 
	      {
		itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet, false); 
	      }
	  }
	paintDirtyList();
	return;
      }

    ///if we are in a Object
    if(IsInObject(x,y))
      {
	if(itsToolBar.pressed)
	  itsToolBar.Unlock();
	itsCurrentObject.MouseDown(e,x,y);
	return;
      }
    
    ///if we are in a Connection

    ErmesConnection connection = GetConnectionNear(x, y);

    if (connection != null)
      {
	if(itsToolBar.pressed)
	  itsToolBar.Unlock();
	connection.MouseDown(e,x,y);
	return;
      }
    
    if (!itsToolBar.locked)
      itsToolBar.Deselect();
    
    if (editStatus == START_ADD)
      {
	AddingObject(x,y);

	if (!itsToolBar.locked)
	  editStatus = DOING_NOTHING;	
      }
    else
      { //DOING_NOTHING, START_SELECT
	if (!e.isShiftDown()) 
	  {
	    deselectAll(true);
	  }
	editStatus = AREA_SELECT;
	currentRect.setBounds(x,y,0,0);
	previousRect.setBounds(x,y,0,0);
	currentPoint.setLocation(x,y);
      }
  }
       
  private void prepareForDynamicConnect(ErmesObjInOutlet io) 
  {
    startConnectPoint.setLocation(io.GetAnchorPoint());
    currentConnectPoint.setLocation(startConnectPoint);
    previousConnectPoint.setLocation(startConnectPoint);
    setCursor(Cursor.getDefaultCursor());
  }

  public void mouseReleased(MouseEvent e){
    int x = e.getX();
    int y = e.getY();

    cleanAnnotations(); // MDC    

    if (itsScrolled)
      itsScrolled=false;

    if (itsRunMode || e.isControlDown())
      {
	if (itsStartDragObject != null) itsStartDragObject.MouseUp(e, x, y);
	itsStartDragObject = null;
	//all the modifications done with the control down were not reported
	//in the offScreen. Do it now!
	//Optimization: use the dirty lists. Try to avoid to repaint in case
	//of CTRL click on the sketch when nothing was changed "pseudo run mode" 
	if (!itsRunMode)
	  repaint();
      }
    else if (editStatus == START_CONNECT)
      {
	if (IsInInOutLet(x,y))
	  {
	    if (itsCurrentInOutlet == itsConnectingLet)
	      {
		editStatus = DOING_NOTHING;
		return;
	      }
	    if (!itsCurrentInOutlet.GetSelected())
	      {
		// no previously selected
		itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
		setCursor(Cursor.getDefaultCursor());
	      }
	    else
	      itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet, false); 
	  }
	else
	  {
	    //mouse up while dragging lines, out of a in/outlet. Abort
	    currentConnectPoint.setLocation(0,0);
	    startConnectPoint.setLocation(0,0);
	    previousConnectPoint.setLocation(0,0);

	    Graphics g = getGraphics();
	    CopyTheOffScreen(getGraphics());
	    g.dispose();

	    editStatus = DOING_NOTHING;
	  }
      }
    else if (editStatus == AREA_SELECT)
      {
	Rectangle aRect = NormalizedRect(currentRect);
	if (!aRect.isEmpty())
	  { 
	    Object objects[] = itsElements.getObjectArray();
	    int size = itsElements.size();

	    for (int i = 0; i < size; i++)
	      {
		ErmesObject aObject = (ErmesObject) objects[i];

		if (aObject.Bounds().intersects(aRect))
		  {
		    aObject.Select(false);
		    currentSelection.addObject(aObject);
		  }
	      }	

	    CheckCurrentFont();	
	    SelectConnections(false);
	  }

	if (currentSelection.itsObjects.isEmpty())
	  { 
	    editStatus = DOING_NOTHING;
	    if (!aRect.isEmpty())
	      {
		Graphics g = getGraphics();
		CopyTheOffScreen(g);//a better solution?, repaint maybe ?
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

	if (currentMovingBigRect == null)
	  currentMovingBigRect = new Rectangle(0, 0, 0, 0);
	else
	  currentMovingBigRect.setBounds(0,0,0,0);

	if (itsMovingRectangles == null)
	  itsMovingRectangles = new MaxVector();
	else
	  itsMovingRectangles.removeAllElements();

	editStatus = START_SELECT;
	if ((currentMouseX-itsStartMovingPt.x!=0)||(currentMouseY-itsStartMovingPt.y!=0))
	  {
	    aDeltaH = currentMouseX-itsStartMovingPt.x;
	    aDeltaV = currentMouseY-itsStartMovingPt.y;
	    if(doSnapToGrid)
	      {
		Point aPoint = SnapToGrid(aDeltaH, aDeltaV);
		aDeltaH = aPoint.x;
		aDeltaV = aPoint.y;
	      }
	    MoveElements(aDeltaH, aDeltaV);
	    //MoveElemConnections(aDeltaH,aDeltaV);
	    repaint();
	  }
	else
	  {
	    //if not moved

	    if(e.getClickCount() == 1)
	      {
		if(oldEditStatus == START_SELECT)
		  {
		    if(itsCurrentObject instanceof ErmesObjEditableObject ||
		       itsCurrentObject instanceof ErmesObjComment)
		      {
			if(clickHappenedOnAnAlreadySelected) 
			  {
			    deselectAll(true);
			    currentSelection.addObject(itsCurrentObject);
			    if (itsCurrentObject instanceof ErmesObjEditableObject) 
			      {
				((ErmesObjEditableObject)itsCurrentObject).backupText();
				((ErmesObjEditableObject)itsCurrentObject).restartEditing();
			      }
			    else
			      ((ErmesObjComment)itsCurrentObject).restartEditing();
			  }
		      }
		  }
	      }
	  }
      }
    else if (editStatus == RESIZING_OBJECT)
      {
	itsCurrentObject.MouseUp(e,x,y);
	editStatus = START_SELECT;
	repaint();
      }
    else if (editStatus == DOING_NOTHING)
      {
      }

    theToolkit.sync();
  }

  public void mouseClicked(MouseEvent e){    cleanAnnotations(); } // MDC

  public void mouseEntered(MouseEvent e){    cleanAnnotations(); } // MDC   

  public void mouseExited(MouseEvent e){
    cleanAnnotations(); // MDC    
    if (itsRunMode) return;		
    if(itsSketchWindow.getCursor().getType()==Cursor.CROSSHAIR_CURSOR){ 
      itsSketchWindow.setCursor(Cursor.getDefaultCursor());
      itsCurrentInOutlet.itsAlreadyMoveIn = false;
    }
  }

  /**
   * Resize a group of object by the given amount of pixel in the given direction
   */
  void resizeSelection(int amount, int direction) {
    int x_amount = 0;
    int y_amount = 0;
    
    if (direction == Platform.LEFT_KEY) x_amount = -amount;
    else if (direction == Platform.RIGHT_KEY) x_amount = amount;
    if (direction == Platform.UP_KEY) y_amount = -amount;
    else if (direction == Platform.DOWN_KEY) y_amount = amount;

    ErmesObject aObject;
    for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if(aObject.canResizeBy(x_amount, y_amount)) aObject.resizeBy(x_amount, y_amount);
      else aObject.ResizeToText(x_amount, y_amount);
    }
    repaint();
  }
    
  /** 
   * Resize all the object in the selection to a size (width or height...)
   */ 
  void alignSizeSelection(int direction) {
    ErmesObject aObject;
    int max = 0;

    if (direction == Platform.LEFT_KEY) { //we're resetting the selection to the minimum (hor.) size
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	aObject.resizeBy(aObject.getMinimumSize().width-aObject.getItsWidth(), 0);
      }
      repaint();
    }    
    else if (direction == Platform.RIGHT_KEY) { //we're setting all the object's widths to the max      
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	if (aObject.getItsWidth() > max) max = aObject.getItsWidth();
      }
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	aObject.resizeBy(max-aObject.getItsWidth(), 0);
      }
      repaint();
    }
    else if (direction == Platform.UP_KEY) {
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	aObject.resizeBy(0, aObject.getMinimumSize().height-aObject.getItsHeight());
      }
      repaint();
    }
    else if (direction == Platform.DOWN_KEY) {
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	if (aObject.getItsHeight() > max) max = aObject.getItsHeight();
      }
      for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject) e.nextElement();
	aObject.resizeBy(0, max-aObject.getItsHeight());
      }
      repaint();
    }
  }

  void moveSelection(int amount, int direction) {
    int x_amount = 0;
    int y_amount = 0;
    ErmesObject aObject;

    if (direction == Platform.LEFT_KEY) x_amount = -amount;
    else if (direction == Platform.RIGHT_KEY) x_amount = amount;
    if (direction == Platform.UP_KEY) y_amount = -amount;
    else if (direction == Platform.DOWN_KEY) y_amount = amount;

    MoveElements(x_amount, y_amount);
    
    repaint();
    //DrawOffScreen(getGraphics());
  }
  
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  void SelectConnections(boolean paintNow){
    ErmesConnection aConnection;
    for (Enumeration en = itsConnections.elements() ; en.hasMoreElements() ;) {
      aConnection = (ErmesConnection) en.nextElement();
      if((aConnection.GetSourceObject().itsSelected)&&(aConnection.GetDestObject().itsSelected)){	
	aConnection.Select(false);
	currentSelection.addConnection(aConnection);
      }
    }
    if (paintNow) paintDirtyList();
  }



  void DynamicScrolling(int theX, int theY){
    Adjustable aHAdjustable =itsSketchWindow.itsScrollerView.getHAdjustable();
    Adjustable aVAdjustable =itsSketchWindow.itsScrollerView.getVAdjustable();
    if(theX>=aHAdjustable.getVisibleAmount()+aHAdjustable.getValue()){
      aHAdjustable.setValue(aHAdjustable.getValue()+aHAdjustable.getUnitIncrement());
    }
    if(theY>=aVAdjustable.getVisibleAmount()+aVAdjustable.getValue()){
      aVAdjustable.setValue(aVAdjustable.getValue()+aVAdjustable.getUnitIncrement());
    }
    if(theX<=aHAdjustable.getValue()){
      aHAdjustable.setValue(aHAdjustable.getValue()-aHAdjustable.getUnitIncrement());
    }
    if(theY<=aVAdjustable.getValue()){
      aVAdjustable.setValue(aVAdjustable.getValue()-aVAdjustable.getUnitIncrement());
    }
  }
  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio

  private Point startConnectPoint = new Point();
  private Point previousConnectPoint = new Point();
  private Point currentConnectPoint = new Point();

  public void mouseDragged(MouseEvent e){

    Rectangle aRect;
    int x = e.getX();
    int y = e.getY();

    cleanAnnotations(); // MDC
    
    if(itsRunMode || e.isControlDown()) {
      if(itsStartDragObject != null) itsStartDragObject.MouseDrag(e, x, y);
      return;
    }

    DynamicScrolling(x, y);
    
    if (editStatus == START_CONNECT) {
      currentConnectPoint.setLocation(x, y);

      Graphics g = getGraphics();
      update(g);
      g.dispose();

      if (IsInInOutLet(x, y)) {
	if (itsCurrentInOutlet != itsConnectingLet)
	  if (itsCurrentInOutlet.IsInlet()) setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
	  else setCursor(Cursor.getPredefinedCursor(Cursor.N_RESIZE_CURSOR));
      } else 
	if (itsSketchWindow.getCursor() != Cursor.getDefaultCursor()) 
	  setCursor(Cursor.getDefaultCursor());
      return; 
    }
    else if(editStatus == AREA_SELECT) {
      if((java.lang.Math.abs(x-currentPoint.x)<5)||(java.lang.Math.abs(y-currentPoint.y)<5)) 
	return;
      if(x>currentPoint.x) 
	if(y>currentPoint.y)	
	  currentRect.setBounds(currentPoint.x,currentPoint.y,x-currentPoint.x, y-currentPoint.y);
	else currentRect.setBounds(currentPoint.x, y, x-currentPoint.x, currentPoint.y-y);
      else if(y>currentPoint.y)  
	currentRect.setBounds(x, currentPoint.y, currentPoint.x-x, y-currentPoint.y); 
      else currentRect.setBounds(x, y, currentPoint.x-x, currentPoint.y-y);
      
      Graphics g = getGraphics();
      update(g);
      g.dispose();
      return;
    } 
    if(editStatus == RESIZING_OBJECT) {
      int aWidth, aHeight;

      if((java.lang.Math.abs(x-currentResizeRect.x)<itsResizingObject.getMinimumSize().width)|| 
	 (x<currentResizeRect.x)) 
	aWidth = currentResizeRect.width;
      else aWidth = x-currentResizeRect.x;

      if((java.lang.Math.abs(y-currentResizeRect.y)<itsResizingObject.getMinimumSize().height+1)||
	 (y<currentResizeRect.y)) aHeight = currentResizeRect.height;
      else aHeight = y-currentResizeRect.y;

      if(itsResizeMode == BOTH_RESIZING)
	currentResizeRect.setSize(aWidth, aHeight);
      else if(itsResizeMode == HORIZONTAL_RESIZING)
	currentResizeRect.setSize(aWidth, currentResizeRect.height);
      else if(itsResizeMode == VERTICAL_RESIZING)
	currentResizeRect.setSize(currentResizeRect.width, aHeight);

      Graphics g = getGraphics();
      update(g);
      g.dispose();

      return;
    } 
    else if (editStatus == MOVING){
      //repaint();
      if(itsStartInclusionRect.x+(x-itsStartMovingPt.x)>=0)
	currentMouseX = x;
      if(itsStartInclusionRect.y+(y-itsStartMovingPt.y)>=0)
	currentMouseY = y;
      repaint();
      return;
    }
    else return;
  }

  public void mouseMoved(MouseEvent e){
    int x = e.getX();
    int y = e.getY();

    cleanAnnotations(); // MDC

    if (itsRunMode) return;
    if(IsInInOutLet(x,y)) {
      itsSketchWindow.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
      if((e.isMetaDown())&&(!itsCurrentInOutlet.itsAlreadyMoveIn)){
	if (e.isShiftDown()) MultiConnect(itsCurrentInOutlet);
	else{
	  if (!itsCurrentInOutlet.GetSelected())
	    itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
	  else 
	    itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet, false); 
	}
	itsCurrentInOutlet.itsAlreadyMoveIn = true;
      }
      return;
    }
    else 
      if(IsInObject(x,y)) {
	if(itsCurrentObject.MouseMove(e,x,y)) return;
      }
    if (itsSketchWindow.getCursor() != Cursor.getDefaultCursor()) itsSketchWindow.setCursor(Cursor.getDefaultCursor());
    if(itsCurrentInOutlet!=null)
      if(itsCurrentInOutlet.itsAlreadyMoveIn) itsCurrentInOutlet.itsAlreadyMoveIn = false;
  }

  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListenr--fine
  

  final ErmesObjEditField GetEditField(){
    return itsEditField;
  }
  final ErmesObjTextArea GetTextArea(){
    return itsTextArea;
  }
  
  //--------------------------------------------------------
  //	MoveSelected
  //--------------------------------------------------------
  
  void MoveSelected(int theX, int theY) {
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

    if (currentSelection.itsObjects.size() < MAX_NUM_SELECTED)
      {
	ErmesObject aObject;
	Rectangle aRect;

	if (itsMovingRectangles == null)
	  itsMovingRectangles = new MaxVector();

	for (Enumeration e = currentSelection.itsObjects.elements() ; e.hasMoreElements() ;) {
	  aObject = (ErmesObject) e.nextElement();
	  aRect = aObject.Bounds();
	  /* (fd, mdc) the copy of the rectangle is made here, not in Bound */

	  itsMovingRectangles.addElement(new Rectangle(aRect.x, aRect.y, aRect.width - 1, aRect.height - 1));
	}
      }
  }

  void SelectAll()
  {
    if(itsRunMode) return;

    Object objects[] = itsElements.getObjectArray();
    int size = itsElements.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject aObject = (ErmesObject) objects[i];
	currentSelection.addObject(aObject);
	aObject.Select(false);
      }

    CheckCurrentFont();
    for (Enumeration e = itsConnections.elements() ; e.hasMoreElements() ;) {
      ErmesConnection aConnection = (ErmesConnection) e.nextElement();
      currentSelection.addConnection(aConnection);
      aConnection.Select(false);
    }
    paintDirtyList();
    //    repaint();
  }


  //--------------------------------------------------------
  //	OutletConnect
  //--------------------------------------------------------
  
  void OutletConnect(ErmesObject theObject/**/, ErmesObjInOutlet theRequester) {	
    ErmesObjOutlet aOutlet;	
    if (itsDirection == NoDirections) {
      itsConnectingLet = theRequester;
      itsConnectingObj = theObject;
      itsDirection = FromOutToIn;
      theRequester.ChangeState(true, theRequester.connected, true);
    }
    else if (itsDirection == FromOutToIn) {
      //deselection of old outlet
      if(itsConnectingLetList.size()!=0){
	for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	  aOutlet = (ErmesObjOutlet)e.nextElement();
	  aOutlet.ChangeState(false,aOutlet.connected, true);
	}
	ResetConnect();
      }
      else {
	itsConnectingObj.ConnectionAbort(itsConnectingLet, true);
	if (editStatus == START_CONNECT)
	  {
	    Graphics g = getGraphics();
	    CopyTheOffScreen(g);//to erase the pending line       update(g);
	    g.dispose();
	  }
      }
      if(theRequester!=itsConnectingLet){
	//selection of new outlet
	itsConnectingObj = theObject;
	itsConnectingLet = theRequester;
	itsDirection = FromOutToIn;
	theRequester.ChangeState(true, theRequester.connected, true);
      }
      //else nothing to do
    }
    else {// FromInToOut
      if(itsConnectingLetList.size()!=0)
	MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true, false);
	theRequester.ChangeState(false, true, false);
	TraceConnection((ErmesObjOutlet)theRequester, (ErmesObjInlet)itsConnectingLet, true);
	
	ResetConnect();
      }
    }

    editStatus = DOING_NOTHING;
  }
  

  boolean MultiConnect(ErmesObjInOutlet theInOutlet){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    if(itsConnectingLetList.size()==0){
      if(itsConnectingLet!=null){
	itsConnectingLetList.addElement(itsConnectingLet);
	MultiConnect(theInOutlet);
      }
      else{
	if(theInOutlet.IsInlet()) itsDirection = FromInToOut;
	else itsDirection = FromOutToIn;
	itsConnectingLetList.addElement(theInOutlet);
	theInOutlet.ChangeState(true, theInOutlet.connected, false);
      }
    }
    else{
      if(itsDirection == FromInToOut){
	if(theInOutlet.IsInlet()){
	  if(itsConnectingLetList.contains(theInOutlet)){
	    itsConnectingLetList.removeElement(theInOutlet);
	    theInOutlet.ChangeState(false, theInOutlet.connected, false);
	  }
	  else{
	    itsConnectingLetList.addElement(theInOutlet);
	    theInOutlet.ChangeState(true, theInOutlet.connected, false);
	    
	  }
	}
	else {
	  for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	    aInlet = (ErmesObjInlet)e.nextElement();
	    aInlet.ChangeState(false, true, false);
	    TraceConnection((ErmesObjOutlet)theInOutlet, aInlet, false);
	  }
	  ((ErmesObjOutlet)theInOutlet).ChangeState(false, true, false);
	  ResetConnect();
	  paintDirtyList();
	}
      }
      else{
	if(!theInOutlet.IsInlet()){
	  if(itsConnectingLetList.contains(theInOutlet)){
	    itsConnectingLetList.removeElement(theInOutlet);
	    theInOutlet.ChangeState(false, theInOutlet.connected, false);
	  }
	  else{
	    itsConnectingLetList.addElement(theInOutlet);
	    theInOutlet.ChangeState(true, theInOutlet.connected, true);

	  }
	}
	else {
	  for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	    aOutlet = (ErmesObjOutlet)e.nextElement();
	    aOutlet.ChangeState(false, true, false);
	    TraceConnection(aOutlet, (ErmesObjInlet)theInOutlet, false);
	  }
	  ((ErmesObjInlet)theInOutlet).ChangeState(false, true, false);
	  ResetConnect();
	}
      }
    }
    paintDirtyList();
    return true;
  }


  //--------------------------------------------------------
  //	preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
  void PrepareInChoice() {
    if (itsInPop != null) return;
    int temp = itsSketchWindow.itsPatcher.getNumberOfInlets();
    itsInPop = new ErmesObjInOutPop(temp);
    add(itsInPop);
  }

  final void RedefineInChoice() {
    itsInPop.Redefine(itsSketchWindow.itsPatcher.getNumberOfInlets());
  }

  void PrepareOutChoice() {
    if (itsOutPop != null) return; //it's OK, we did it already
    
    itsOutPop = new ErmesObjInOutPop(itsSketchWindow.itsPatcher.getNumberOfOutlets());
    add(itsOutPop);
  }
  
  void RedefineOutChoice() {
    itsOutPop.Redefine(itsSketchWindow.itsPatcher.getNumberOfOutlets());
  }

  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {

    if(itsScrolled){
      if (offScreenPresent) {
	if (paintForTheFirstTime) {
	  DrawOffScreen(g);
	  paintForTheFirstTime = false;
	}
	else {
	  erased = true;
	  erased1 = true;

	  // Graphics g = getGraphics();
	  CopyTheOffScreen(g);
	  // g.dispose();
	}
      }
      else {
	// Graphics g = getGraphics();
	  DrawOffScreen(g);
	  // g.dispose();
      }
    }
    else {
      DrawOffScreen(g);
    }
  }		
  
  //--------------------------------------------------------
  //	ResetConnect
  //--------------------------------------------------------
  
  boolean ResetConnect() {
    itsConnectingObj = null;
    itsConnectingLet = null;
    itsDirection = NoDirections;
    itsConnectingLetList.removeAllElements();
    return true;
  }

  void SetResizeState(ErmesObject theResizingObject){
    editStatus = RESIZING_OBJECT;
    itsResizingObject = theResizingObject;
    if (currentResizeRect == null) currentResizeRect = new Rectangle();
    currentResizeRect.x = itsResizingObject.getItsX();
    currentResizeRect.y = itsResizingObject.getItsY();
    currentResizeRect.width = itsResizingObject.getItsWidth();
    currentResizeRect.height = itsResizingObject.getItsHeight();
    previousResizeRect.setBounds(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
  }
  
  void SetRunMode(boolean theMode) {
    itsRunMode = theMode;
    setBackground(theMode?Color.white:sketchColor);
    if (theMode) {
      deselectAll(true);
    }
  }
    
  final boolean GetRunMode(){
    return itsRunMode;
  }

  //--------------------------------------------------------
    //	SetSnapToGrid
    //	set the snap to grid flag and snap the existing objects
    //--------------------------------------------------------
  void SetSnapToGrid(){
    ErmesObject aObject;
    Point aPoint;
    int aDeltaH, aDeltaV;
    doSnapToGrid = !doSnapToGrid;
    if(doSnapToGrid){
      for(int i = 0; i<currentSelection.itsObjects.size();i++){
	aObject = (ErmesObject)currentSelection.itsObjects.elementAt(i);
	aPoint = SnapToGrid(aObject.getItsX(), aObject.getItsY());
	aDeltaH = aPoint.x - aObject.getItsX();
	aDeltaV = aPoint.y - aObject.getItsY() ;
	aObject.MoveBy(aDeltaH, aDeltaV);
      }
    }
    repaint();
  }
  
  final void SetStartSelect() {
    editStatus = START_SELECT;
  }
  
  //--------------------------------------------------------
  //	SetToolBar
  //  ToolBar associated with the SketchPad
  //--------------------------------------------------------

  final void SetToolBar(ErmesSwToolbar theToolBar) {
    itsToolBar = theToolBar;
  }
  

  final void startAdd(String theDescription) {
    deselectAll(true);
    editStatus = START_ADD;
    itsAddObjectDescription = theDescription;
  }

  final void AddInlet(ErmesObjInlet theInlet){
    itsInletList.addElement(theInlet);
  }

  final void RemoveInlet(ErmesObjInlet theInlet){
    itsInletList.removeElement(theInlet);
  }
  
  final void AddOutlet(ErmesObjOutlet theOutlet){
    itsOutletList.addElement(theOutlet);
  }

  final void RemoveOutlet(ErmesObjOutlet theOutlet){
    itsOutletList.removeElement(theOutlet);
  }

  void RemoveInOutlets(ErmesObject theObject){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    for(Enumeration e = theObject.itsInletList.elements(); e.hasMoreElements();) {
      aInlet = (ErmesObjInlet)e.nextElement();
      itsInletList.removeElement(aInlet);
    }
    for(Enumeration e1 = theObject.itsOutletList.elements(); e1.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet)e1.nextElement();
      itsOutletList.removeElement(aOutlet);
    }
  }


  private boolean erased = false;
  private boolean erased1 = false;

  public void update(Graphics g) {
    if (itsRunMode) return;

    if (editStatus == START_CONNECT) {
      if (!erased) {
	g.setColor(Color.black);
	g.setXORMode(sketchColor);
	g.drawLine(startConnectPoint.x, startConnectPoint.y, 
		   previousConnectPoint.x, previousConnectPoint.y);
      }
      g.drawLine(startConnectPoint.x, startConnectPoint.y,
		 currentConnectPoint.x, currentConnectPoint.y);
      previousConnectPoint.setLocation(currentConnectPoint);
      erased = false;
    }
    else if (editStatus == AREA_SELECT) {
           //faster version
      if (!erased) {
	g.setColor(Color.black);
	g.setXORMode(sketchColor);
	g.drawRect(previousRect.x, previousRect.y, previousRect.width, previousRect.height);
      }
      //paint new:
      g.drawRect(currentRect.x, currentRect.y, currentRect.width, currentRect.height);
      previousRect.setBounds(currentRect.x, currentRect.y, currentRect.width, currentRect.height);
      erased = false;
	     
    }
    else if (editStatus == RESIZING_OBJECT) {
           //faster version
      if (!erased) {
	g.setColor(Color.black);
	g.setXORMode(sketchColor);
	g.drawRect(previousResizeRect.x, previousResizeRect.y, previousResizeRect.width, previousResizeRect.height);
      }
      //paint new:
      g.drawRect(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
      previousResizeRect.setBounds(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
      erased = false;

    }
    else if (editStatus == MOVING){
      //faster version
      Rectangle aRect;
      g.setColor(Color.black);
      g.setXORMode(sketchColor);
      int aNumSelected = currentSelection.itsObjects.size();

      if (aNumSelected<MAX_NUM_SELECTED)
	{
	  if (!erased1)
	    {
	      if (itsMovingRectangles != null)
		for(Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements();)
		  {
		    aRect = (Rectangle)e.nextElement();
		    g.drawRect(aRect.x,aRect.y, aRect.width, aRect.height);
		  }
	    }

	  if (itsMovingRectangles != null)
	    for(Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements();)
	      {
		aRect = (Rectangle)e.nextElement();
		aRect.x+=(currentMouseX-itsPreviousMouseX);
		aRect.y+=(currentMouseY-itsPreviousMouseY);
		g.drawRect(aRect.x,aRect.y, aRect.width, aRect.height);
	      }

	  itsPreviousMouseX = currentMouseX;
	  itsPreviousMouseY = currentMouseY;
	  erased1 = false;
	}
      else
	{
	  int oldX, oldY, oldWidth, oldHeight;

	  if ((currentMovingBigRect != null) && (! erased))
	    {
	      g.drawRect(currentMovingBigRect.x, 
			 currentMovingBigRect.y, 
			 currentMovingBigRect.width, 
			 currentMovingBigRect.height);
	    }

	  currentMovingBigRect = StartMoveInclusionRect();
	  currentMovingBigRect.x+=(currentMouseX-itsStartMovingPt.x);
	  currentMovingBigRect.y+=(currentMouseY-itsStartMovingPt.y);

	  g.drawRect(currentMovingBigRect.x, 
		     currentMovingBigRect.y, 
		     currentMovingBigRect.width, 
		     currentMovingBigRect.height);
	  erased = false;
	}
      return;
    }
    else
      paint(g);
  }

  /* keep track of the scrolling values */
  int itsCurrentScrollingX;
  int itsCurrentScrollingY;
  public void adjustmentValueChanged(AdjustmentEvent e){
    itsCurrentScrollingX = itsSketchWindow.itsScrollerView.getHAdjustable().getValue();
    itsCurrentScrollingY = itsSketchWindow.itsScrollerView.getVAdjustable().getValue();
    
    itsScrolled = true;
  }

  void AlignSelectedObjects(String thePosition){
    ErmesObject aObject;
    int aValue;
    if(thePosition.equals("Top")){
      aValue = MinYSelected();
      for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	aObject.MoveBy(0, aValue-aObject.getItsY());
      }
    }
    else if(thePosition.equals("Left")){
      aValue = MinXSelected();
      for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	aObject.MoveBy(aValue-aObject.getItsX(), 0);
      }
    }
    else if(thePosition.equals("Bottom")){
      aValue = MaxYSelected();
      for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	aObject.MoveBy(0, aValue-(aObject.getItsY()+aObject.getItsHeight()));
      }
    }
    else if(thePosition.equals("Right")){
      aValue = MaxXSelected();
      for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	aObject.MoveBy(aValue-(aObject.getItsX()+aObject.getItsWidth()), 0);
      }
    }

    Graphics g = getGraphics();
    DrawOffScreen(g);//repaint();	  CopyTheOffScreen(g);
    g.dispose();
  }
  
  int MinYSelected(){
    ErmesObject aObject;
    int aMinY = 10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinY >= aObject.getItsY()) aMinY = aObject.getItsY();
    }
    return aMinY;
  }
  
  int MinXSelected(){
    ErmesObject aObject;
    int aMinX = 10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinX >= aObject.getItsX()) aMinX = aObject.getItsX();
    }
    return aMinX;
  }

  int MaxYSelected(){
    ErmesObject aObject;
    int aMaxY = -10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxY<aObject.getItsY()+aObject.getItsHeight()) 
	aMaxY = aObject.getItsY()+aObject.getItsHeight();
    }
    return aMaxY;
  }

  int MaxXSelected(){
    ErmesObject aObject;
    int aMaxX = -10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxX < aObject.getItsX()+aObject.getItsWidth()) 
	aMaxX = aObject.getItsX()+aObject.getItsWidth();
    }
    return aMaxX;
   }

  // The following stuff was in ErmesSketchHelper

  //--------------------------------------------------------
  //	AddConnection
  //  adding a connection between two objects
  //--------------------------------------------------------

  final ErmesConnection AddConnection(ErmesObject fromObj, ErmesObject toObj, int fromOutlet, int toInlet, FtsConnection fc) {
    ErmesConnection aConnection = new ErmesConnection(fromObj, toObj, this, fromOutlet, toInlet, fc);
    aConnection.update(fc);
    return aConnection;
  }
	
  //--------------------------------------------------------
  //	AddObject
  //  adding an object of the given class name in the given location
  //--------------------------------------------------------

  ErmesObject AddObject(FtsObject theFtsObject)
  {
    ErmesObject aObject;

    aObject = ErmesObject.makeErmesObject(this, theFtsObject);
    itsElements.addElement(aObject);

    theFtsObject.setRepresentation(aObject);

    return aObject;

  }
  
  //--------------------------------------------------------
  //	ChangeObjectPrecedence
  //--------------------------------------------------------

  final void ChangeObjectPrecedence(ErmesObject theObject){
    itsElements.removeElement(theObject);
    itsElements.addElement(theObject);	
  }
  
  //--------------------------------------------------------
  //	DeleteObjectConnections
  //	delete the object's connections
  //--------------------------------------------------------

  void DeleteObjectConnections(ErmesObject theObject, boolean paintNow){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(Enumeration e = theObject.GetInletList().elements() ; e.hasMoreElements() ;) {
      aInlet = (ErmesObjInlet) e.nextElement();
      while (!aInlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aInlet.GetConnections().elementAt(0);
	DeleteConnection(aConnection, false);
      }
    }
    for(Enumeration e1 = theObject.GetOutletList().elements() ; e1.hasMoreElements() ;) {
      aOutlet = (ErmesObjOutlet) e1.nextElement();
      while (!aOutlet.GetConnections().isEmpty()) {
	aConnection = (ErmesConnection) aOutlet.GetConnections().elementAt(0);
	DeleteConnection(aConnection, false);
      }
    }
    if (paintNow) paintDirtyList();
  }
  

  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one connection routine
  //--------------------------------------------------------

  void DeleteConnection(ErmesConnection theConnection, boolean paintNow) {

    if (theConnection.itsFtsConnection != null)
      theConnection.itsFtsConnection.delete();	//delete from FTS

    DeleteGraphicConnection(theConnection, paintNow);
  }

  //--------------------------------------------------------
  //	DeleteConnection
  //	delete one connection routine
  //--------------------------------------------------------
  void DeleteGraphicConnection(ErmesConnection theConnection, boolean paintNow) {
    ErmesObjOutlet aOutlet = theConnection.GetOutlet();
    ErmesObjInlet aInlet = theConnection.GetInlet();
    
    currentSelection.removeConnection(theConnection);
    itsConnections.removeElement(theConnection);
    aOutlet.GetConnections().removeElement(theConnection);
    aInlet.GetConnections().removeElement(theConnection);
    if(aInlet.GetConnections().size()==0) aInlet.SetConnected(false, false);
    if(aOutlet.GetConnections().size()==0) aOutlet.SetConnected(false, false);

    markSketchAsDirty();
    if (paintNow) paintDirtyList();
  }



  //--------------------------------------------------------
  //	DeleteObject
  //	delete one object routine
  //--------------------------------------------------------
  void DeleteObject(ErmesObject theObject, boolean paintNow) {
    DeleteGraphicObject(theObject, paintNow);
    if (theObject.itsFtsObject != null) theObject.itsFtsObject.delete();
  }
  
  void DeleteGraphicObject(ErmesObject theObject, boolean paintNow) {
    DeleteObjectConnections(theObject, false);
    //removes theObject from the selected elements list	
    if(theObject instanceof FtsPropertyHandler)
      if(theObject.GetFtsObject()!=null) theObject.GetFtsObject().removeWatch(theObject);
    currentSelection.removeObject(theObject);
    //removes theObject from the element list (delete)
    itsElements.removeElement(theObject);
    RemoveInOutlets(theObject);
    markSketchAsDirty();
    
    if (paintNow) paintDirtyList();
  }
  
  //--------------------------------------------------------
  //	DeleteSelected
  //	delete routine
  //--------------------------------------------------------

  void DeleteSelected() {
    ErmesObject aObject;
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;

    if(!DeleteInOutletConnections(false)){
      while (!currentSelection.itsObjects.isEmpty()) {
	aObject = (ErmesObject) currentSelection.itsObjects.elementAt(0);
	DeleteObject(aObject, false);
      }
      
      while(!currentSelection.itsConnections.isEmpty()){
	int i=0;
	aConnection = (ErmesConnection) currentSelection.itsConnections.elementAt(0);
	DeleteConnection(aConnection, false);
      }
    }
    
    GetSketchWindow().DeselectionUpdateMenu();
    paintDirtyList();
  }
    
  boolean DeleteInOutletConnections(boolean paintNow){
    ErmesObjInOutlet aInOutlet;
    boolean ret = false;

    if(itsConnectingLetList.size()==0){
      if(itsConnectingLet!=null){
	DeleteThisInOutletConn(itsConnectingLet, false);
	ResetConnect();
	ret = true;
      }
    }
    else{
      for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	DeleteThisInOutletConn(aInOutlet, false);
      }
      ResetConnect();
      ret = true;
    }
    if (paintNow) paintDirtyList();
    return ret;
  }

  void DeleteThisInOutletConn(ErmesObjInOutlet theInOutlet, boolean paintNow){
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;
    
      while (theInOutlet.GetConnections().size() != 0) {
	aConnection = (ErmesConnection)theInOutlet.GetConnections().elementAt(0);
	DeleteConnection(aConnection, false);
      }
      
      theInOutlet.GetConnections().removeAllElements();
      theInOutlet.ChangeState(false, false, false);
      if (paintNow) paintDirtyList();
  }

  //--------------------------------------------------------
  //	deselectObjects
  //	deselect all the objects of a given selection
  //--------------------------------------------------------
  
  void deselectObjects(MaxVector theObjects, boolean paintNow){
    ErmesObject aObject;
    for (Enumeration e = theObjects.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Deselect(false);
    }
    if (paintNow) {
      paintDirtyList();
    }
  }
  //--------------------------------------------------------
  //	deselectConnections
  //	deselect all the connections of a given Selection
  //--------------------------------------------------------

  void deselectConnections(MaxVector theConnections, boolean paintNow){
    ErmesConnection aConnection;
    for (Enumeration e = theConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Deselect(false);
    }
    if (paintNow) {
      paintDirtyList();
    }
  }


  /**
   * Deselect everything selected in the sketch 
   * (objects, connections, current selected in/outlet).
   * This function handles the Focus change and updates the menus.
   */
  void deselectAll(boolean paintNow) {
    if (GetEditField() != null && GetEditField().HasFocus()) {
      GetEditField().transferFocus();
    }
    if(editStatus == EDITING_OBJECT){
      GetEditField().LostFocus();
    }

    if(currentSelection.itsObjects.size() != 0) 
      GetSketchWindow().DeselectionUpdateMenu();

    deselectCurrentInOutlet(false);
    deselectObjects(currentSelection.itsObjects, false);
    deselectConnections(currentSelection.itsConnections, false);
    
    currentSelection.removeAllElements();

    if (paintNow) {
      paintDirtyList();
    }
    if (GetEditField() != null && GetEditField().HasFocus()) {
      GetEditField().transferFocus();
    }
    if(editStatus == EDITING_OBJECT){
      GetEditField().LostFocus();
    }
  }

  void deselectCurrentInOutlet(boolean paintNow){
    ErmesObjInOutlet aInOutlet;
    if(itsConnectingLet!=null) 
      itsConnectingLet.ChangeState(false, itsConnectingLet.connected, false);
    if(itsConnectingLetList.size()!=0){
      for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	aInOutlet = (ErmesObjInOutlet)e.nextElement();
	aInOutlet.ChangeState(false, aInOutlet.GetConnected(), false);
      }
    }
    ResetConnect();
    if (paintNow) paintDirtyList();
  }

  
  //--------------------------------------------------------
  //	GetDrawableRect
  //--------------------------------------------------------
  Rectangle GetDrawableRect(Rectangle originalRect, Dimension drawingArea) {
    int x = originalRect.x;
    int y = originalRect.y;
    int width = originalRect.width;
    int height = originalRect.height;
    
    //Make sure rectangle width and height are positive.
    if (width < 0) {
      width = 0 - width;
      x = x - width + 1;
      if (x < 0) {
	width += x;
	x = 0;
      }
    }
    if (height < 0) {
      height = 0 - height;
      y = y - height + 1;
      if (y < 0) {
	height += y;
	y = 0;
      }
    }
    
    //The rectangle shouldn't extend past the drawing area.
    if ((x + width) > drawingArea.width) {
      width = drawingArea.width - x;
    }
    if ((y + height) > drawingArea.height) {
      height = drawingArea.height - y;
    }
    
    return new Rectangle(x, y, width, height);
  }
  
  
  //--------------------------------------------------------
  //	GetConnectionNear
  //--------------------------------------------------------

  // This function substitute IsInConnection; predicates should
  // not have side effect; by returning directly the connection
  // found we use one member variable less, and the code is more
  // readeble, because all the effects are explicit

  ErmesConnection  GetConnectionNear(int x, int y){
    ErmesConnection aConnection;
    Rectangle aRect;
    for(Enumeration e = itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection)e.nextElement();
      if(aConnection.IsNearToPoint(x, y))
	return aConnection;
    }
    return null;
  }
  
  //--------------------------------------------------------
  //	IsInInOutLet
  //--------------------------------------------------------
  boolean IsInInOutLet(int x, int y){
    Rectangle aRect;
    int size;
    Object[] objects;

    size = itsInletList.size();
    objects = itsInletList.getObjectArray();

    for (int i = 0; i < size; i++) {
      ErmesObjInlet aInlet = (ErmesObjInlet)objects[i];
      aRect = aInlet.getSensibleBounds();
      if(aRect.contains(x,y)) {
	itsCurrentInOutlet = aInlet;
	return true;
      }
    }

    size = itsOutletList.size();
    objects = itsOutletList.getObjectArray();

    for (int i = 0; i < size; i++) {
      ErmesObjOutlet aOutlet = (ErmesObjOutlet)objects[i];
      aRect = aOutlet.getSensibleBounds();
      if(aRect.contains(x,y)) {
	itsCurrentInOutlet = aOutlet;
	return true;
      }
    }

    return false;
  }
  
  //--------------------------------------------------------
  //	IsInObject
  //--------------------------------------------------------

  boolean IsInObject(int x, int y){
    Object objects[] = itsElements.getObjectArray();
    int size = itsElements.size();

    for (int i = 0; i < size; i++)
      {
	ErmesObject aObject = (ErmesObject) objects[i];

	if (aObject.Bounds().contains(x,y))
	  {
	    itsCurrentObject = aObject;
	    return true;
	  }
      }
    return false;
  }
  
  //--------------------------------------------------------
  //	MoveElements
  //	Move the selected elements
  //--------------------------------------------------------
  void MoveElements(int theDeltaH, int theDeltaV){
    ErmesObject aObject;

    Object[] objects = currentSelection.itsObjects.getObjectArray();
    int size = currentSelection.itsObjects.size();

    for (int i = 0; i < size; i++)
      {
	aObject = (ErmesObject) objects[i];
	aObject.MoveBy(theDeltaH, theDeltaV);
      }
  }

  //--------------------------------------------------------
  //	SnapToGrid
  //--------------------------------------------------------
  Point SnapToGrid(int x, int y){
    Point aPoint;
    int quot = (int)java.lang.Math.floor(x/snapGrid.width);
    int res = (x - quot*snapGrid.width);
    if(res>snapGrid.width/2)
      x = (quot+1)*snapGrid.width;
    else
      x = quot*snapGrid.width;
    quot = (int)java.lang.Math.floor(y/snapGrid.height);
    res = (y - quot*snapGrid.height);
    if(res>snapGrid.height/2)
      y = (quot+1)*snapGrid.height;
    else
      y = quot*snapGrid.height;
    return aPoint = new Point(x, y);
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

    for(int i=0; i<currentSelection.itsObjects.size(); i++)
      {
	aObject = (ErmesObject) currentSelection.itsObjects.elementAt(i);
	
	Rectangle aRect = aObject.Bounds();

	if (aRect.x < left)
	  left = aRect.x;

	if (aRect.y<top)
	  top = aRect.y;

	if (aRect.y+aRect.height>bottom)
	  bottom = aRect.y + aRect.height;

	if (aRect.x +aRect.width>right)
	  right = aRect.x + aRect.width;
      }

    return new Rectangle(left, top, right-left, bottom-top);
  }
	
  /**
   * an utility function to find the upper-left corner of a selection
   */
  Point selectionUpperLeft() {
    int minX;
    int minY;
    ErmesObject aObject;    

    if (currentSelection.itsObjects.size() == 0) return new Point(0,0);
    aObject = (ErmesObject)currentSelection.itsObjects.elementAt(0);
    minX = aObject.getItsX();
    minY = aObject.getItsY();
    for(int i=1; i<currentSelection.itsObjects.size(); i++){
      aObject = (ErmesObject) currentSelection.itsObjects.elementAt(i);
      if (aObject.getItsX() < minX) minX = aObject.getItsX();
      if (aObject.getItsY() < minY) minY = aObject.getItsY();
    }
    return new Point(minX, minY);
  }

  //--------------------------------------------------------
  //	TraceConnection
  //--------------------------------------------------------
  
  ErmesConnection TraceConnection(ErmesObjOutlet theSourceOutlet, ErmesObjInlet theDestInlet, boolean paintNow){
    ErmesConnection aConnection;
    try {
      aConnection = new ErmesConnection(this, theDestInlet, theSourceOutlet);}
    catch (FtsException e) {
      return null;
    }
    theSourceOutlet.AddConnection(aConnection);
    theDestInlet.AddConnection(aConnection); 
    itsConnections.addElement(aConnection);
    if (paintNow) aConnection.DoublePaint();
    else addToDirtyConnections(aConnection);
    return aConnection;
  }
  // End of old ErmesSketchHelper


  // Experimental MDC

  private boolean annotating = false;

  void setAnnotating()
  {
    annotating = true;
  }

  void showAnnotations(String property)
  {
    if (! annotating)
      {
	ErmesObject aObject;

	if (currentSelection.itsObjects.size() == 0)
	  {
	    Object objects[] = itsElements.getObjectArray();
	    int size = itsElements.size();

	    for (int i = 0; i < size; i++)
	      ((ErmesObject) objects[i]).showAnnotation(property);
	  }
	else
	  for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();)
	    {
	      aObject = (ErmesObject) e.nextElement();
	      aObject.showAnnotation(property);
	    }
      }
  }

  void cleanAnnotations()
  {
    if (annotating)
      {
	annotating = false;
	Graphics g = getGraphics();
	paint(g);
	g.dispose();
      }
  }
}










