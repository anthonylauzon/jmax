
//	LA VERA SKETCH
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
 */
public class ErmesSketchPad extends Panel implements AdjustmentListener, MouseMotionListener, MouseListener, ErmesDrawable{
  
  //2703...
  public boolean isInGroup = false;
  public boolean drawPending = false;
  public boolean copyPending = false;

  
  public void updateGroupStart() {
    isInGroup = true;
  }
  
  public void updateGroupEnd() {
    isInGroup = false;
    if (drawPending) {
      DrawOffScreen(getGraphics());
    }
    else if (copyPending) {
      CopyTheOffScreen(getGraphics());
    }
    drawPending = false;
    copyPending = false;
  }

  
  //ErmesDrawable interface:
  public boolean getDirty() {
    return true;
  }

  public void setDirty(boolean b) {
    //itsDirtyFlag = b;
  }

  public void Paint(Graphics g) {
    repaint();
  }
  //end of ErmesDrawable

  ErmesSketchWindow itsSketchWindow;
  Dimension preferredSize; 
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

  public static int debug_count_update = 1;
  public static int debug_count_paint  = 1;
  public final static int DEBUG_COUNT  = 50;
  //public static boolean stillLoading = false;
  final static Dimension snapGrid = new Dimension(30, 50);
  final public static Color sketchColor = new Color(230, 230, 230);	//the sketchPad gray...

  public Font sketchFont = new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.PLAIN, ircam.jmax.utils.Platform.FONT_SIZE);
  public int sketchFontSize = ircam.jmax.utils.Platform.FONT_SIZE;
  int SKETCH_WIDTH = 1200/*800*/;
  int SKETCH_HEIGHT = 1200/*800*/;

  int MAX_NUM_SELECTED = 10;

  int itsJustificationMode = LEFT_JUSTIFICATION;
  int itsResizeMode = BOTH_RESIZING;

  int inCount = 0;   //ref count of ErmesObjIn objects (used if this is a subpatcher)
  int outCount = 0;  //the same for ErmesObjOut objects
  ErmesObjInOutPop itsInPop = null;
  ErmesObjInOutPop itsOutPop = null;
		
  int itsDirection = NoDirections;
  public boolean itsRunMode = false;
  boolean doSnapToGrid = false;
  public boolean itsGraphicsOn = true;
  public boolean paintForTheFirstTime = true;
  
  ErmesObjEditField itsEditField = null;
  ErmesObjTextArea itsTextArea = null;
  ErmesObject itsConnectingObj = null;  
  ErmesObjInOutlet itsConnectingLet = null;
  int currentMouseX, currentMouseY;	// used during the MOVING status
  int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  
  Vector itsConnections;
  Vector itsInletList;
  Vector itsOutletList;
  static public ErmesSelection currentSelection;
  static public FtsClipboard ftsClipboard;
  Vector itsTempSelected;
  public Vector itsElements;
  //  static ErmesSelection itsSelection.itsConnections;
  Vector itsUpdateList;
  Vector itsPatcherElements;
  Rectangle currentRect = new Rectangle();
  Rectangle previousRect = new Rectangle();
  Point	currentPoint = new Point();
  Rectangle currentResizeRect = null;
Rectangle previousResizeRect = new Rectangle(); 

  Rectangle currentMovingBigRect = new Rectangle();
  Rectangle previousMovingBigRect = new Rectangle();
  Vector itsMovingRectangles = new Vector();

  public boolean offScreenValid = true;
  
  //STATIC OFFSCREEN!!!

  static ErmesSketchPad lastSketchWithOffScreen = null;
  boolean offScreenPresent = false;
  static Graphics offGraphics = null;
  static Dimension offDimension;
  static Image offImage;
		
		
  ErmesSwToolbar itsToolBar;
  Point itsStartMovingPt;
  Rectangle itsStartInclusionRect;
  ErmesObjInOutlet itsCurrentInOutlet = null;
  ErmesObject itsCurrentObject = null;
  ErmesObject itsStartDragObject = null;
  ErmesObject itsResizingObject = null;
  ErmesConnection itsCurrentConnection = null;
  // This is an housekeeping structure to implement the re-edit of an
  // editable object. The logic is:
  // 1) click on an object selects the object and go in MOVING status
  // 2) release the mouse (moves if moved), go in START_SELECT
  // 3) second click select the object, go in MOVING
  // 4) second release re-edit the object only if the first click was on the same, already selected object.
  boolean clickHappenedOnAnAlreadySelected = false;

  Vector itsConnectingLetList;

  int editStatus = DOING_NOTHING;
  int oldEditStatus = DOING_NOTHING;

  static Hashtable nameTable = new Hashtable(16, (float) 0.5); // substitute name lists

  // Static initializer for the hashTable
  static
  {
    // Initialization of the "fts class"  to "graphic object" table

    nameTable.put("messbox", ircam.jmax.editors.ermes.ErmesObjMessage.class);
    nameTable.put("button", ircam.jmax.editors.ermes.ErmesObjBang.class);
    nameTable.put("toggle", ircam.jmax.editors.ermes.ErmesObjToggle.class);
    nameTable.put("intbox", ircam.jmax.editors.ermes.ErmesObjInt.class);
    nameTable.put("floatbox", ircam.jmax.editors.ermes.ErmesObjFloat.class);
    nameTable.put("comment", ircam.jmax.editors.ermes.ErmesObjComment.class);
    nameTable.put("slider", ircam.jmax.editors.ermes.ErmesObjSlider.class);
    nameTable.put("inlet", ircam.jmax.editors.ermes.ErmesObjIn.class);
    nameTable.put("outlet", ircam.jmax.editors.ermes.ErmesObjOut.class);
    // nameTable.put("patcher", ircam.jmax.editors.ermes.ErmesObjExternal.class);
    nameTable.put("patcher", ircam.jmax.editors.ermes.ErmesObjPatcher.class);
  }

  int itsAddObject;
  String itsAddObjectName;
  Rectangle resizeRect = new Rectangle();
  public ErmesSketchHelper itsHelper;

  boolean itsScrolled = false;
  Vector dirtyInOutlets = new Vector();
  Vector dirtyConnections = new Vector();
  Vector dirtyObjects = new Vector();
  private boolean dirtySketch = false;

  public void addToDirtyInOutlets(ErmesObjInOutlet theInOutlet) {
    if (dirtySketch) return;
    if (!dirtyInOutlets.contains(theInOutlet))
      dirtyInOutlets.addElement(theInOutlet);
  }

  public void addToDirtyConnections(ErmesConnection theConnection) {
    if (dirtySketch) return;
    if (!dirtyConnections.contains(theConnection))
      dirtyConnections.addElement(theConnection);
  }

  public void addToDirtyObjects(ErmesObject theObject) {
    if (dirtySketch) return;
    if (!dirtyObjects.contains(theObject))
    dirtyObjects.addElement(theObject);
  }

  public void markSketchAsDirty() {
    dirtySketch = true;
  }

  public void removeDirtyInOutlet(ErmesObjInOutlet io) {
    dirtyInOutlets.removeElement(io);
  }

  private void emptyDirtyLists() {
    dirtyInOutlets.removeAllElements();
    dirtyObjects.removeAllElements();
    dirtyConnections.removeAllElements();
    dirtySketch = false;
  }

  /**
   * At this level, we can implement a politic of drawing.
   * (example, if there are not objects to paint, paint connections and
   * in/outlets directly, without using CopyTheOffScreen())
   */
  public void paintDirtyList() {
    if (offGraphics == null) GetOffGraphics();
    if (dirtySketch) {
      DrawOffScreen(getGraphics());
      //repaint();
      emptyDirtyLists();
      return;
    }
    else {
      paintList(dirtyInOutlets, offGraphics);
      dirtyInOutlets.removeAllElements();
      //--
      if (!itsRunMode) { //objects UNDER connections 
	paintList(dirtyObjects, offGraphics);
	dirtyObjects.removeAllElements();
	paintList(dirtyConnections, offGraphics);
	dirtyConnections.removeAllElements();
      }
      else  { // connections UNDER objects
	paintList(dirtyConnections, offGraphics);
	dirtyConnections.removeAllElements();
	paintList(dirtyObjects, offGraphics);
	dirtyObjects.removeAllElements();
      }
      //--
      CopyTheOffScreen(getGraphics());
      //emptyDirtyLists is done in CopyTheOffScreen()
      
    }
  }
  

  private void paintList(Vector theList, Graphics theContext) {
    if (theContext == null) return;
    if (theList.size()!= 0) {
       for (Enumeration e = theList.elements(); e.hasMoreElements();) {
	 ((ErmesDrawable) e.nextElement()).Paint(theContext);
       }
    }
  }

  //--------------------------------------------------------
  //	AddObjectForName
  //  adding an object given itsName and parameters.
  //	It is used by scripts
  //--------------------------------------------------------
  public ErmesObject AddObjectByName(String theName, int x, int y, String args) {
    ErmesObject aObject = null;
    ErmesObjOutlet aOutlet;
    int i;
    Rectangle aRect;
    
    try { 
      aObject = (ErmesObject) Class.forName(theName).newInstance();	
    }  catch(ClassNotFoundException e) {return null;}
    catch(IllegalAccessException e) {return null;}
    catch(InstantiationException e) {return null;}
    //finally {
    aObject.Init(this, x, y, args);
      //something went wrong...
    //};
      itsElements.addElement(aObject);
      aObject.DoublePaint();

      if(theName == "ircam.jmax.editors.ermes.ErmesObjPatcher")
	itsPatcherElements.addElement(aObject);
      if (!itsToolBar.locked && editStatus != EDITING_OBJECT) editStatus = DOING_NOTHING;	

      return aObject;
  }
  
  //--------------------------------------------------------
  //	AddConnectionByInOut
  //  adding a connection given the source, dest, inlet, outlet.
  //	Used by scripts
  //--------------------------------------------------------
  public ErmesConnection AddConnectionByInOut(ErmesObject srcObj, int srcOut, ErmesObject destObj, int destIn)
  {
    ErmesObjOutlet out;
    ErmesObjInlet in;
    ErmesObject aObject = null;

    out = (ErmesObjOutlet) srcObj.itsOutletList.elementAt(srcOut);
    in  = (ErmesObjInlet) destObj.itsInletList.elementAt(destIn);

    in.ChangeState(false, true, false); //warning: how many repaint() this function costs?
    out.ChangeState(false, true, false);//warning: how many repaint() this function costs?

    ErmesConnection aConnection = itsHelper.TraceConnection(out, in, true);
    return aConnection;
  }
  
  public Font getSketchFont() {
    return sketchFont;
  }

  public int getSketchFontSize() {
    return sketchFontSize;
  }

  public void ChangeFont(Font theFont){
    try {
      FontMetrics aFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics(theFont);
      setFont(theFont);   
    }
    catch (Exception e) {
      new ErrorDialog(itsSketchWindow, "This font/fontsize does not exist on this platform");
      return;
    }
    sketchFont = theFont;
  }
  
  public void ChangeNameFont(String theFontName){
    ErmesObject aObject;
    Font aFont;

    for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(theFontName, sketchFont.getStyle(), aObject.getFont().getSize());
	try {
	  FontMetrics aFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics(aFont);
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

  public void ChangeSizeFont(int fontSize){
    ErmesObject aObject;
    Font aFont;
    for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(aObject.getFont().getName(), sketchFont.getStyle(), fontSize);
	try {
	  FontMetrics aFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics(aFont);
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

  public void ChangeJustification(String theJustification){
    int aJustificationMode = 0;
    if(theJustification.equals("Center")) aJustificationMode = CENTER_JUSTIFICATION;
    else if(theJustification.equals("Left")) aJustificationMode = LEFT_JUSTIFICATION;
    else if(theJustification.equals("Right")) aJustificationMode = RIGHT_JUSTIFICATION;

    if(currentSelection.itsObjects.size()==0) itsJustificationMode = aJustificationMode;

    ErmesObject aObject;
    for (Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment))
	aObject.setJustification(aJustificationMode);
    }
    repaint();
  }

  public void ChangeResizeMode(String theResizeMode){
    if(theResizeMode.equals("Both")) itsResizeMode = BOTH_RESIZING;
    else if(theResizeMode.equals("Horizontal")) itsResizeMode = HORIZONTAL_RESIZING;
    else if(theResizeMode.equals("Vertical")) itsResizeMode = VERTICAL_RESIZING;
  }
  //--------------------------------------------------------
  //	ClickOnConnection
  //--------------------------------------------------------

  public boolean ClickOnConnection(MouseEvent evt,int x, int y){
    switch(editStatus) {
    case START_ADD:
      break;
    case DOING_NOTHING:
      itsHelper.deselectCurrentInOutlet(false);

      currentSelection.addConnection(itsCurrentConnection); 
      itsCurrentConnection.Select(false);
      paintDirtyList();
      editStatus = START_SELECT;///////
      
      break;
    case START_SELECT:
      if (!evt.isShiftDown()) {//lo shift non e' premuto
	itsHelper.deselectAll(false);
	currentSelection.addConnection(itsCurrentConnection); 

	itsCurrentConnection.Select(false);
	paintDirtyList();
      }
      else{
	if(!(currentSelection.itsConnections.contains(itsCurrentConnection))){
	  currentSelection.addConnection(itsCurrentConnection); 
	  itsCurrentConnection.Select(true);
	}	
	else {
	  currentSelection.removeConnection(itsCurrentConnection);
	  itsCurrentConnection.Deselect(false);
	  
	  if((currentSelection.itsConnections.size()) == 0)
	    editStatus = DOING_NOTHING;
	  paintDirtyList();
	}
      }	
      break;	
    }
    return true;
  }

	
  //--------------------------------------------------------
  //	ClickOnObject
  //--------------------------------------------------------

  boolean ClickOnObject(ErmesObject theObject, MouseEvent evt, int theX, int theY){
    
    if(!itsRunMode){
      switch(editStatus) {
      case START_ADD:
	break;
      case DOING_NOTHING:
	itsHelper.deselectCurrentInOutlet(false);
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
	  itsHelper.deselectAll(true);
	  currentSelection.addObject(theObject);
	  theObject.Select(true);
	  CheckCurrentFont();
	  MoveSelected(theX,theY);
	}
	break;
      }
      itsHelper.ChangeObjectPrecedence(theObject);
    }
    return true;
  }
  
  static int noOfCopy = 0;
  public void CopyTheOffScreen(Graphics g) {
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
  public void DoNothing() {
    editStatus = DOING_NOTHING;
  }
  
  public void DrawLinesOffScreen() {	//now it checks if we have the offGraphics...
    ErmesConnection aConnection;
    for (Enumeration e =itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection) e.nextElement();
      if (offScreenPresent) {
	aConnection.Paint(offGraphics);
      }
      else 
	if (getGraphics() != null) aConnection.Paint(getGraphics());
    }
    if (offScreenPresent) CopyTheOffScreen(getGraphics()); 
  }
  
  public void DrawOffScreen(Graphics g) {
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
    ErmesObject aObject;
    ErmesConnection aConnection;
    ErmesObjOutlet aOutlet;
    ErmesObjInlet aInlet;
    
    for (Enumeration e =itsElements.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      for (Enumeration e1 = aObject.GetInletList().elements(); e1.hasMoreElements();) {
	aInlet = (ErmesObjInlet) e1.nextElement();
	aInlet.Paint(offGraphics);
      }
      for (Enumeration e1 = aObject.GetOutletList().elements(); e1.hasMoreElements();) {
	aOutlet = (ErmesObjOutlet) e1.nextElement();
	aOutlet.Paint(offGraphics);
      }
      if (!itsRunMode) aObject.Paint(offGraphics);//edit mode=objects BEFORE connections
    }

    for (Enumeration e =itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Paint(offGraphics);
    }
    
    if (itsRunMode) for (Enumeration e =itsElements.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      aObject.Paint(offGraphics);
    }


    CopyTheOffScreen(g);
  }
  
  //--------------------------------------------------------
  //	GetElements
  //  return the elements list
  //--------------------------------------------------------
  public Vector GetElements(){
    return itsElements;
  }
  
  //--------------------------------------------------------
  //	GetPatcherElements
  // 
  //--------------------------------------------------------
  public Vector GetPatcherElements(){
    return itsPatcherElements;
  }

  public Graphics GetOffGraphics() {
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
  
  public ErmesSketchWindow GetSketchWindow(){
    return itsSketchWindow;
  }
  
  int incrementalPasteOffsetX;
  int incrementalPasteOffsetY;
  Point pasteDelta = new Point();
  int numberOfPaste = 0;
  FtsObject anOldPastedObject;
  
  // note: the following function is a reduced version of InitFromFtsContainer.
  // better organization urges
  void PasteObjects(Vector objectVector, Vector connectionVector) {
    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    ErmesConnection aConnection;

    int objectX;    
    int objectY;
    int minX=0;
    int minY=0;

    numberOfPaste += 1;
    itsHelper.deselectAll(false);

    if (objectVector == null) return;
    fo = (FtsObject)objectVector.elementAt(0);
    minX = ((Integer)fo.get("x")).intValue();
    minY = ((Integer)fo.get("y")).intValue();
    if (numberOfPaste == 1) {
      anOldPastedObject = fo;
      incrementalPasteOffsetX = 20;
      incrementalPasteOffsetY = 20;
    }
    else if (numberOfPaste == 2) {
      
      incrementalPasteOffsetX = (((Integer)(anOldPastedObject.get("x"))).intValue()-minX);
      incrementalPasteOffsetY = (((Integer)(anOldPastedObject.get("y"))).intValue()-minY);
    }

    for (Enumeration waste = objectVector.elements(); waste.hasMoreElements();) {
      fo = (FtsObject)waste.nextElement();
      objectX = ((Integer)fo.get("x")).intValue();
      objectY = ((Integer)fo.get("y")).intValue();
      if (objectX < minX) minX = objectX;
      if (objectY < minY) minY = objectY;
    }

    for (Enumeration e = objectVector.elements(); e.hasMoreElements();) {
      fo = (FtsObject)e.nextElement();
      // Note that the representation is now found from the fts className,
      // made unique; the new file format will allow for specifing
      // additional information, like a non default graphic representation
      // the code will need a small change here
      
      Class objectClass = itsHelper.SearchFtsName(fo);
      if (objectClass==null) continue;

      objectX = ((Integer)fo.get("x")).intValue();
      objectY = ((Integer)fo.get("y")).intValue();

      fo.put("x", objectX-minX+itsCurrentScrollingX+pasteDelta.x+numberOfPaste*incrementalPasteOffsetX);     
      fo.put("y", objectY-minY+itsCurrentScrollingY+pasteDelta.y+numberOfPaste*incrementalPasteOffsetY);
       
      aObject = itsHelper.AddObject(objectClass, fo);
      currentSelection.addObject(aObject);
      aObject.Select(false);
       
      if (objectClass == ircam.jmax.editors.ermes.ErmesObjPatcher.class)
	itsPatcherElements.addElement(aObject);
      
      if (aObject != null) fo.setRepresentation(aObject);
    }

    // chiama tanti AddConnection...

    ErmesObject fromObj, toObj;
    
    for (Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) {
      fc = (FtsConnection)e2.nextElement();

      fromObj = (ErmesObject) fc.getFrom().getRepresentation();
      toObj = (ErmesObject) fc.getTo().getRepresentation();
      aConnection = itsHelper.AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
      currentSelection.addConnection(aConnection);
      aConnection.Select(false);
    }
    editStatus = START_SELECT;
    paintDirtyList();
  }
  
  
  public void InitFromFtsContainer(FtsContainerObject theContainerObject){
	
    FtsContainerObject aFtsPatcher = theContainerObject;
    // chiama tanti AddObject...
    Vector objectVector = aFtsPatcher.getObjects();	//usefull?


    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    for (Enumeration e = objectVector.elements(); e.hasMoreElements();) {
      fo = (FtsObject)e.nextElement();
      // Note that the representation is now found from the fts className,
      // made unique; the new file format will allow for specifing
      // additional information, like a non default graphic representation
      // the code will need a small change here

      Class objectClass = itsHelper.SearchFtsName(fo);
      if (objectClass==null) continue;
      aObject = itsHelper.AddObject(objectClass, fo);
    
      if (objectClass == ircam.jmax.editors.ermes.ErmesObjPatcher.class)
	itsPatcherElements.addElement(aObject);

      if (aObject != null) fo.setRepresentation(aObject);
    }
		
    // chiama tanti AddConnection...
    Vector connectionVector = aFtsPatcher.getConnections();	//usefull?
    ErmesObject fromObj, toObj;
    ErmesConnection aConnection = null;
    
    for (Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) {
      fc = (FtsConnection)e2.nextElement();

      fromObj = (ErmesObject) fc.getFrom().getRepresentation();
      toObj = (ErmesObject) fc.getTo().getRepresentation();
      aConnection = itsHelper.AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
    }
    repaint();
    
  }

  //--------------------------------------------------------
  //	InletConnect
  //--------------------------------------------------------
  
  boolean InletConnect(ErmesObject theObject, ErmesObjInOutlet theRequester) {	
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
	if (editStatus == START_CONNECT) CopyTheOffScreen(getGraphics());//to erase the pending line 
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
      if(itsConnectingLetList.size()!=0) MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true, false);
	theRequester.ChangeState(false, true/*false*/, false);
	itsHelper.TraceConnection((ErmesObjOutlet)itsConnectingLet, (ErmesObjInlet)theRequester, false);
	ResetConnect();
      }
    }

    editStatus = DOING_NOTHING;
    paintDirtyList();
    return true;
  }
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesSketchPad(ErmesSketchWindow theSketchWindow) {    
    super();
    itsHelper = new ErmesSketchHelper(this);
    //Fts.getServer().addUpdateGroupListener(this);
    setLayout(null);
    preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);
    itsSketchWindow = theSketchWindow;
    itsConnections = new Vector();
    itsInletList = new Vector();
    itsOutletList = new Vector();
    if (currentSelection==null) currentSelection = new ErmesSelection(Fts.getSelection());
    try {
      if (ftsClipboard==null)
	ftsClipboard = (FtsClipboard) Fts.makeFtsObject(Fts.getServer().getRootObject(), "__clipboard");
    }
    catch (FtsException e) {
      System.out.println("warning: failed to create an Fts clipboard");
    }
    itsElements = new Vector();
    itsStartMovingPt = new Point(0,0);    
    itsStartInclusionRect = new Rectangle();  
    itsUpdateList = new Vector();
    itsPatcherElements = new Vector();
    itsConnectingLetList = new Vector();
        
    itsEditField = new ErmesObjEditField(this);
    add(itsEditField);
    validate();
    itsEditField.setVisible(false);
    itsEditField.setLocation(-200,-200);
    
    itsTextArea = new ErmesObjTextArea(this);
    add(itsTextArea);
    validate();
    itsTextArea.setVisible(false);
    itsTextArea.setLocation(-200,-200);

    setBackground(sketchColor);
    addMouseMotionListener(this);
    addMouseListener(this);
    addKeyListener(itsSketchWindow);

    
    // Initialization of the "fts class"  to "graphic object" table
  }
	
  static public void RequestOffScreen(ErmesSketchPad theSketchPad) {
    if (lastSketchWithOffScreen == theSketchPad || theSketchPad.offScreenPresent) return;
    if (lastSketchWithOffScreen!=null) {
      if (!lastSketchWithOffScreen.itsRunMode)
	lastSketchWithOffScreen.itsHelper.deselectAll(true);
      lastSketchWithOffScreen.offScreenPresent = false;
    }
    theSketchPad.offScreenPresent = true;
    lastSketchWithOffScreen = theSketchPad;
    theSketchPad.paintForTheFirstTime = true;
  }
  
  public void CheckCurrentFont(){
    ErmesObject aObject;
    String aFontName;
    Integer aSize;
    Integer aJustification;
    Vector aUsedFontVector = new Vector();
    Vector aUsedSizeVector = new Vector();
    Vector aUsedJustificationVector = new Vector();
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
  public Dimension getMinimumSize() {
    return new Dimension(30, 20);
  }
	
  void AddingObject(int x, int y){
    Rectangle aRect = null;
    ErmesObject aObject = null;
    ErmesObjOutlet aOutlet;

    if(doSnapToGrid){
      Point aPoint = itsHelper.SnapToGrid(x, y);
      x = aPoint.x;
      y = aPoint.y;
    }

    // Code to prevent inlets/outlets in a top level patcher deleted,
    // as discussed a while ago (we actually may want to edit a subpatcher
    // as a single file).

    try
      {
	aObject = (ErmesObject) Class.forName(itsAddObjectName).newInstance();
      }
    catch (ClassNotFoundException e1)
      {
	System.err.println("ErmesSketchPad:mousePressed: INTERNAL ERROR: Class not found: " + e1);
	return;
      }
    catch (IllegalAccessException e2)
      {
	System.err.println("ErmesSketchPad:mousePressed: INTERNAL ERROR: Illegal Access: " + e2);
	return;
      }
    catch (InstantiationException e3)
      {
	System.err.println("ErmesSketchPad:mousePressed: INTERNAL ERROR: Instantiation Error: " + e3);
	return;
      }
    
    aObject.Init(this, x, y, "");
    itsElements.addElement(aObject);
    //System.err.println("dirty list: "+dirtyInOutlets.size()+" inoutlets"+
    //		       dirtyConnections.size()+" connections"+
    //		       dirtyObjects.size()+" objects");
    paintDirtyList();
    if(itsAddObjectName == "ircam.jmax.editors.ermes.ErmesObjPatcher")
      itsPatcherElements.addElement(aObject);
    if (!itsToolBar.locked && editStatus != EDITING_OBJECT) editStatus = DOING_NOTHING;	

  }



  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio

  public void mousePressed(MouseEvent e){

    MaxApplication.setCurrentWindow(itsSketchWindow);
    itsSketchWindow.requestFocus();//???
    
    int x = e.getX();
    int y = e.getY();
    int i;
    
    if(!offScreenPresent){
      RequestOffScreen(this);
      DrawOffScreen(getGraphics());
    }
    
    if (itsRunMode || e.isControlDown()) {
      if(itsHelper.IsInObject(x,y)) {
	itsCurrentObject.MouseDown(e,x,y);
	itsStartDragObject = itsCurrentObject;	//object FROM WHICH we started drag
      }
      return;
    }
    
    if(editStatus == EDITING_OBJECT){
      itsEditField.LostFocus();
      requestFocus();
    }
    if(editStatus == EDITING_COMMENT){
      itsTextArea.LostFocus();
      requestFocus();
    }
    ///if we are in a InOutLet
    if(itsHelper.IsInInOutLet(x,y)){
      if(itsToolBar.pressed) itsToolBar.Unlock();
      itsHelper.deselectObjects(currentSelection.itsObjects, false);
      itsHelper.deselectConnections(currentSelection.itsConnections, false);
      if (e.isShiftDown()){
	MultiConnect(itsCurrentInOutlet);
      }
      else{
	if (!itsCurrentInOutlet.GetSelected()) {// no previously selected
	  itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
	  if (itsConnectingLet != null) { //we are going to START a connection
	    //(not to terminate one!!)
	    editStatus = START_CONNECT;
	    prepareForDynamicConnect(itsCurrentInOutlet);
	  }
	}
	else {
	  itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet, false); 
	}
      }
      paintDirtyList();
      return;
    }
    ///if we are in a Object
    if(itsHelper.IsInObject(x,y)){
      if(itsToolBar.pressed) itsToolBar.Unlock();
      itsCurrentObject.MouseDown(e,x,y);
      return;
    }
    
    ///if we are in a Connection
    if(itsHelper.IsInConnection(x,y)) {
      if(itsToolBar.pressed) itsToolBar.Unlock();
      itsCurrentConnection.MouseDown(e,x,y);
      return;
    }
    
    if (!itsToolBar.locked) itsToolBar.Deselect();
    
    if(editStatus == START_ADD){
      AddingObject(x,y);
    }
    else{ //DOING_NOTHING, START_SELECT
      if (!e.isShiftDown()) {
	itsHelper.deselectAll(true);
      }
      editStatus = AREA_SELECT;
      currentRect.setBounds(x,y,0,0);
      previousRect.setBounds(x,y,0,0);
      currentPoint.setLocation(x,y);
    }

  }
       
  private void prepareForDynamicConnect(ErmesObjInOutlet io) {
    startConnectPoint.setLocation(io.GetAnchorPoint());
    currentConnectPoint.setLocation(startConnectPoint);
    previousConnectPoint.setLocation(startConnectPoint);
    setCursor(Cursor.getDefaultCursor());
  }

  public void mouseReleased(MouseEvent e){
    int x = e.getX();
    int y = e.getY();

    MaxApplication.setCurrentWindow(itsSketchWindow);
    if(itsScrolled) itsScrolled=false;

    if (itsRunMode || e.isControlDown()) {
      if (itsStartDragObject != null) itsStartDragObject.MouseUp(e, x, y);
      itsStartDragObject = null;
      //all the modifications done with the control down were not reported
      //in the offScreen. Do it now!
      //Optimization: use the dirty lists. Try to avoid to repaint in case
      //of CTRL click on the sketch when nothing was changed "pseudo run mode" 
      if (!itsRunMode) repaint();
      return;
    }

    if (editStatus == START_CONNECT) {
      if (itsHelper.IsInInOutLet(x,y)) {
	if (itsCurrentInOutlet == itsConnectingLet) {
	  editStatus = DOING_NOTHING;
	  return;
	}
 	if (!itsCurrentInOutlet.GetSelected()) {// no previously selected
	  itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
	  setCursor(Cursor.getDefaultCursor());
	}
	else {
	  itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet, false); 
	}
      }
      else { //mouse up while dragging lines, out of a in/outlet. Abort
	currentConnectPoint.setLocation(0,0);
	startConnectPoint.setLocation(0,0);
	previousConnectPoint.setLocation(0,0);
	CopyTheOffScreen(getGraphics());
	editStatus = DOING_NOTHING;
      }
    }
    else if (editStatus == AREA_SELECT) {

      Rectangle aRect = itsHelper.NormalizedRect(currentRect);
      if (!aRect.isEmpty()) { 
	for (Enumeration en = itsElements.elements() ; en.hasMoreElements() ;) {
	  ErmesObject aObject = (ErmesObject) en.nextElement();
	  if (aObject.Bounds().intersects(aRect)) {
	    aObject.Select(false);
	    currentSelection.addObject(aObject);
	  }
	}	
	CheckCurrentFont();	
	SelectConnections(false);
      }
	

      if (currentSelection.itsObjects.isEmpty()) { 
	editStatus = DOING_NOTHING;
	if (!aRect.isEmpty())
	  CopyTheOffScreen(getGraphics());//a better solution?
      }
      else {
	editStatus = START_SELECT;
	paintDirtyList();
      }
      return;
    }
    else if (editStatus == MOVING) {
      int aDeltaH, aDeltaV;
      currentMovingBigRect.setBounds(0,0,0,0);
      previousMovingBigRect.setBounds(0,0,0,0);
      itsMovingRectangles.removeAllElements();
      editStatus = START_SELECT;
      if((currentMouseX-itsStartMovingPt.x!=0)||(currentMouseY-itsStartMovingPt.y!=0)){
	aDeltaH = currentMouseX-itsStartMovingPt.x;
	aDeltaV = currentMouseY-itsStartMovingPt.y;
	if(doSnapToGrid){
	  Point aPoint = itsHelper.SnapToGrid(aDeltaH, aDeltaV);
	  aDeltaH = aPoint.x;
	  aDeltaV = aPoint.y;
	}
	itsHelper.MoveElements(aDeltaH, aDeltaV);
	//itsHelper.MoveElemConnections(aDeltaH,aDeltaV);
	repaint();
      }
      else {//se non ha mosso
	if(e.getClickCount() == 1){
	  if(oldEditStatus == START_SELECT){
	    /*for problems during Inits, ErmesObjComment is not, for now,
	      a subclass of ErmesObjEditableObject as it should, but a direct
	      child of ErmesObject.
	      The situation is going to change soon (today is 03/05/98)*/
	    if(itsCurrentObject instanceof ErmesObjEditableObject ||
	       itsCurrentObject instanceof ErmesObjComment){
	      if(clickHappenedOnAnAlreadySelected) {
		itsHelper.deselectAll(true);
		currentSelection.addObject(itsCurrentObject);
		if (itsCurrentObject instanceof ErmesObjEditableObject) {
		  ((ErmesObjEditableObject)itsCurrentObject).backupText();
		  ((ErmesObjEditableObject)itsCurrentObject).RestartEditing();
		}
		else ((ErmesObjComment)itsCurrentObject).RestartEditing();
	      }
	    }
	  }
	}
      }
    }
    else if (editStatus == RESIZING_OBJECT){
      itsCurrentObject.MouseUp(e,x,y);
      editStatus = START_SELECT;
      repaint();
    }
    else if(editStatus == DOING_NOTHING) return;
  }

  public void mouseClicked(MouseEvent e){}

  public void mouseEntered(MouseEvent e){}

  public void mouseExited(MouseEvent e){
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

    itsHelper.MoveElements(x_amount, y_amount);
    
    repaint();
    //DrawOffScreen(getGraphics());
  }
  
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  public void SelectConnections(boolean paintNow){
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



  public boolean DynamicScrolling(int theX, int theY){
    Adjustable aHAdjustable =itsSketchWindow.itsScrollerView.getHAdjustable();
    Adjustable aVAdjustable =itsSketchWindow.itsScrollerView.getVAdjustable();
    if(theX>=aHAdjustable.getVisibleAmount()+aHAdjustable.getValue()){
      aHAdjustable.setValue(aHAdjustable.getValue()+aHAdjustable.getUnitIncrement());
      return true;
    }
    if(theY>=aVAdjustable.getVisibleAmount()+aVAdjustable.getValue()){
      aVAdjustable.setValue(aVAdjustable.getValue()+aVAdjustable.getUnitIncrement());
      return true;
    }
    if(theX<=aHAdjustable.getValue()){
      aHAdjustable.setValue(aHAdjustable.getValue()-aHAdjustable.getUnitIncrement());
      return true;
    }
    if(theY<=aVAdjustable.getValue()){
      aVAdjustable.setValue(aVAdjustable.getValue()-aVAdjustable.getUnitIncrement());
      return true;
    }
    else return false;
  }
  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio

  Point startConnectPoint = new Point();
  Point previousConnectPoint = new Point();
  Point currentConnectPoint = new Point();
  public void mouseDragged(MouseEvent e){

    Rectangle aRect;
    int x = e.getX();
    int y = e.getY();
    
    if(itsRunMode || e.isControlDown()) {
      if(itsStartDragObject != null) itsStartDragObject.MouseDrag(e, x, y);
      return;
    }

    DynamicScrolling(x, y);
    
    if (editStatus == START_CONNECT) {
      currentConnectPoint.setLocation(x, y);
      update(getGraphics());
      if (itsHelper.IsInInOutLet(x, y)) {
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
      
      update(getGraphics());
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
      update(getGraphics());
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
    if (itsRunMode) return;
    if(itsHelper.IsInInOutLet(x,y)) {
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
      if(itsHelper.IsInObject(x,y)) {
	if(itsCurrentObject.MouseMove(e,x,y)) return;
      }
    if (itsSketchWindow.getCursor() != Cursor.getDefaultCursor()) itsSketchWindow.setCursor(Cursor.getDefaultCursor());
    if(itsCurrentInOutlet!=null)
      if(itsCurrentInOutlet.itsAlreadyMoveIn) itsCurrentInOutlet.itsAlreadyMoveIn = false;
  }

  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListenr--fine
  

  public ErmesObjEditField GetEditField(){
    return itsEditField;
  }
  public ErmesObjTextArea GetTextArea(){
    return itsTextArea;
  }
  
  //--------------------------------------------------------
  //	MoveSelected
  //--------------------------------------------------------
  
  public void MoveSelected(int theX, int theY) {
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
    itsStartInclusionRect = itsHelper.StartMoveInclusionRect();
    if(currentSelection.itsObjects.size()<MAX_NUM_SELECTED) FillVectorObjectRectangles();
  }

  void FillVectorObjectRectangles(){
    ErmesObject aObject;
    Rectangle aRect;
    for (Enumeration e = currentSelection.itsObjects.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      aRect = aObject.Bounds();
      aRect.width -= 1;
      aRect.height -=1; 
      itsMovingRectangles.addElement(aRect);
    }
  }

  public void SelectAll(){
    ErmesObject aObject;
    ErmesConnection aConnection;
    if(itsRunMode) return;

    for (Enumeration e = itsElements.elements() ; e.hasMoreElements() ;) {
      aObject = (ErmesObject) e.nextElement();
      currentSelection.addObject(aObject);
      aObject.Select(false);
    }
    CheckCurrentFont();
    for (Enumeration e = itsConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      currentSelection.addConnection(aConnection);
      aConnection.Select(false);
    }
    paintDirtyList();
    //    repaint();
  }


  //--------------------------------------------------------
  //	OutletConnect
  //--------------------------------------------------------
  
  boolean OutletConnect(ErmesObject theObject/**/, ErmesObjInOutlet theRequester) {	
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
	if (editStatus == START_CONNECT) CopyTheOffScreen(getGraphics());//to erase the pending line 
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
      if(itsConnectingLetList.size()!=0) MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true, false);
	theRequester.ChangeState(false, true, false);
	itsHelper.TraceConnection((ErmesObjOutlet)theRequester, (ErmesObjInlet)itsConnectingLet, true);
	
	ResetConnect();
      }
    }

    editStatus = DOING_NOTHING;
    return true;
  }
  

  public boolean MultiConnect(ErmesObjInOutlet theInOutlet){
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
	    itsHelper.TraceConnection((ErmesObjOutlet)theInOutlet, aInlet, false);
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
	    itsHelper.TraceConnection(aOutlet, (ErmesObjInlet)theInOutlet, false);
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

  public void RedefineInChoice() {
    int temp = itsSketchWindow.itsPatcher.getNumberOfInlets();
    itsInPop.Redefine(temp);
  }

  void PrepareOutChoice() {
    if (itsOutPop != null) return; //it's OK, we did it already
    
    int temp = itsSketchWindow.itsPatcher.getNumberOfOutlets();
    itsOutPop = new ErmesObjInOutPop(temp);
    add(itsOutPop);
  }
  
  public void RedefineOutChoice() {
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
	  CopyTheOffScreen(getGraphics());
	}
      }
      else {
	DrawOffScreen(getGraphics());
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

  public void SetResizeState(ErmesObject theResizingObject){
    editStatus = RESIZING_OBJECT;
    itsResizingObject = theResizingObject;
    if (currentResizeRect == null) currentResizeRect = new Rectangle();
    currentResizeRect.x = itsResizingObject.getItsX();
    currentResizeRect.y = itsResizingObject.getItsY();
    currentResizeRect.width = itsResizingObject.getItsWidth();
    currentResizeRect.height = itsResizingObject.getItsHeight();
    previousResizeRect.setBounds(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
  }
  
  public void SetRunMode(boolean theMode) {
    itsRunMode = theMode;
    setBackground(theMode?Color.white:sketchColor);
    if (theMode) {
      itsHelper.deselectAll(true);
    }
  }
    
  public boolean GetRunMode(){
    return itsRunMode;
  }

  //--------------------------------------------------------
    //	SetSnapToGrid
    //	set the snap to grid flag and snap the existing objects
    //--------------------------------------------------------
  public void SetSnapToGrid(){
    ErmesObject aObject;
    Point aPoint;
    int aDeltaH, aDeltaV;
    doSnapToGrid = !doSnapToGrid;
    if(doSnapToGrid){
      for(int i = 0; i<currentSelection.itsObjects.size();i++){
	aObject = (ErmesObject)currentSelection.itsObjects.elementAt(i);
	aPoint = itsHelper.SnapToGrid(aObject.getItsX(), aObject.getItsY());
	aDeltaH = aPoint.x - aObject.getItsX();
	aDeltaV = aPoint.y - aObject.getItsY() ;
	aObject.MoveBy(aDeltaH, aDeltaV);
      }
    }
    repaint();
  }
  
  public void SetStartSelect() {
    editStatus = START_SELECT;
  }
  
  //--------------------------------------------------------
  //	SetToolBar
  //  ToolBar associated with the SketchPad
  //--------------------------------------------------------
  public void SetToolBar(ErmesSwToolbar theToolBar) {
    itsToolBar = theToolBar;
  }
  
  //--------------------------------------------------------
  //	StartAdd
  //	message received from the ToolBar when an object is selected
  //--------------------------------------------------------
  public void StartAdd(int theObject) {
    itsHelper.deselectAll(true);
    editStatus = START_ADD;
    itsAddObject = theObject;
  }
  
  public void startAdd(String theObject) {
    itsHelper.deselectAll(true);
    editStatus = START_ADD;
    itsAddObjectName = theObject;
  }

  public void AddInlet(ErmesObjInlet theInlet){
    itsInletList.addElement(theInlet);
  }

  public void RemoveInlet(ErmesObjInlet theInlet){
    itsInletList.removeElement(theInlet);
  }
  
  public void AddOutlet(ErmesObjOutlet theOutlet){
    itsOutletList.addElement(theOutlet);
  }

  public void RemoveOutlet(ErmesObjOutlet theOutlet){
    itsOutletList.removeElement(theOutlet);
  }

  public void RemoveInOutlets(ErmesObject theObject){
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


  boolean erased = false;
  boolean erased1 = false;

  public void update(Graphics g) {
    if (itsRunMode) return;
    if (editStatus == START_CONNECT) {
      if (!erased) {
	g.setColor(Color.black);
	g.setXORMode(sketchColor);
	g.drawLine(startConnectPoint.x, startConnectPoint.y, 
		   previousConnectPoint.x, previousConnectPoint.y);
	erased = true;
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
	erased = true;
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
	erased = true;
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
      if(aNumSelected<MAX_NUM_SELECTED){
	if(!erased1){
	  for(Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements();) {
	    aRect = (Rectangle)e.nextElement();
	    g.drawRect(aRect.x,aRect.y, aRect.width, aRect.height);
	  }
	  erased1= true;
	}
	for(Enumeration e = itsMovingRectangles.elements(); e.hasMoreElements();) {
	  aRect = (Rectangle)e.nextElement();
	  aRect.x+=(currentMouseX-itsPreviousMouseX);
	  aRect.y+=(currentMouseY-itsPreviousMouseY);
	  g.drawRect(aRect.x,aRect.y, aRect.width, aRect.height);
	}
	itsPreviousMouseX = currentMouseX;
	itsPreviousMouseY = currentMouseY;
	erased1 = false;
      }
      else{
	previousMovingBigRect.setBounds(currentMovingBigRect.x,
					currentMovingBigRect.y,
					currentMovingBigRect.width,
					currentMovingBigRect.height);
	currentMovingBigRect = itsHelper.StartMoveInclusionRect();
	currentMovingBigRect.x+=(currentMouseX-itsStartMovingPt.x);
	currentMovingBigRect.y+=(currentMouseY-itsStartMovingPt.y);
	if(!erased){
	  g.drawRect(previousMovingBigRect.x, 
		     previousMovingBigRect.y, 
		     previousMovingBigRect.width, 
		     previousMovingBigRect.height);
	  erased = true;
	}
	g.drawRect(currentMovingBigRect.x, 
		   currentMovingBigRect.y, 
		   currentMovingBigRect.width, 
		   currentMovingBigRect.height);
	erased = false;
      }
      return;
    }
    else paint(g);
  }

  /* keep track of the scrolling values */
  int itsCurrentScrollingX;
  int itsCurrentScrollingY;
  public void adjustmentValueChanged(AdjustmentEvent e){
    itsCurrentScrollingX = itsSketchWindow.itsScrollerView.getHAdjustable().getValue();
    itsCurrentScrollingY = itsSketchWindow.itsScrollerView.getVAdjustable().getValue();
    
    itsScrolled = true;
  }

  public void AlignSelectedObjects(String thePosition){
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
    DrawOffScreen(getGraphics());//repaint();
  }
  
  public int MinYSelected(){
    ErmesObject aObject;
    int aMinY = 10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinY >= aObject.getItsY()) aMinY = aObject.getItsY();
    }
    return aMinY;
  }
  
  public int MinXSelected(){
    ErmesObject aObject;
    int aMinX = 10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinX >= aObject.getItsX()) aMinX = aObject.getItsX();
    }
    return aMinX;
  }

  public int MaxYSelected(){
    ErmesObject aObject;
    int aMaxY = -10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxY<aObject.getItsY()+aObject.getItsHeight()) 
	aMaxY = aObject.getItsY()+aObject.getItsHeight();
    }
    return aMaxY;
  }

   public int MaxXSelected(){
    ErmesObject aObject;
    int aMaxX = -10000;
    for(Enumeration e = currentSelection.itsObjects.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxX < aObject.getItsX()+aObject.getItsWidth()) 
	aMaxX = aObject.getItsX()+aObject.getItsWidth();
    }
    return aMaxX;
   }
}










