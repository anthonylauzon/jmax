
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
public class ErmesSketchPad extends Panel implements AdjustmentListener, MouseMotionListener, MouseListener/*, FtsUpdateGroupListener*/{
  
  //2703...
  public boolean isInGroup = false;
  public boolean drawPending = false;
  public boolean copyPending = false;
  
  /*
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
  }*/
  //...2703
  
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
  public static int debug_count_update = 1;
  public static int debug_count_paint  = 1;
  public final static int DEBUG_COUNT  = 50;
  //public static boolean stillLoading = false;
  final static Dimension snapGrid = new Dimension(30, 50);
  final public static Color sketchColor = new Color(230, 230, 230);	//the sketchPad gray...

  public final int PIXEL_LEFT = 0;
  public final int PIXEL_RIGHT = 1;

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
  public boolean doAutorouting = true;
  //  public boolean itsSelectionRouting = true;
  public boolean itsGraphicsOn = true;
  public boolean paintForTheFirstTime = true;
  
  ErmesObjEditField itsEditField = null;
  ErmesObjTextArea itsTextArea = null;
  ErmesObject itsConnectingObj = null;  
  ErmesObjInOutlet itsConnectingLet = null;
  int currentMouseX, currentMouseY;	// used during the MOVING status
  int itsPreviousMouseX, itsPreviousMouseY;// used during the MOVING status

  
  ErmesRegion itsElementRgn;
  ErmesRegion itsHSegmRgn;
  ErmesRegion itsVSegmRgn;
  Vector itsConnections;
  Vector itsInletList;
  Vector itsOutletList;
  public Vector itsSelectedList;
  Vector itsTempSelected;
  public Vector itsElements;
  Vector itsSelectedConnections;
  Vector itsConnectionSetList;
  Vector itsUpdateList;
  Vector itsPatcherElements;
  Rectangle currentRect = new Rectangle();
  Rectangle previousRect = new Rectangle();
  Point	currentPoint = new Point();
  Rectangle currentResizeRect = null;
Rectangle previousResizeRect = new Rectangle(); 

  Rectangle currentMovingBigRect = new Rectangle();
  Rectangle previousMovingBigRect = new Rectangle();
  //Rectangle currentMovingLittleRect = new Rectangle();
  //Rectangle previousMovingLittleRect = new Rectangle();
  Vector itsMovingRectangles = new Vector();

  public boolean offScreenValid = true;
  
  //2203ErmesObjMessThread itsMessThread = null;

  //  boolean itsFirstClick = true;
  //STATIC OFFSCREEN!!!

  static ErmesSketchPad lastSketchWithOffScreen = null;
  boolean offScreenPresent = false;
  static Graphics offGraphics = null;
  static Dimension offDimension;
  static Image offImage;
		
		
  ErmesSwToolbar itsToolBar;
  ErmesConnSegment itsSelectedSegment;
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


  // debug utility, to be removed //
  public void paintAllRegions() {

    itsElementRgn.paintRegion(getGraphics());
    itsHSegmRgn.paintRegion(getGraphics());
    itsVSegmRgn.paintRegion(getGraphics());
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
      //there was an error "aObject may not have been initialized"
      aObject = (ErmesObject) Class.forName(theName).newInstance();	
    }  catch(ClassNotFoundException e) {return null;}
    catch(IllegalAccessException e) {return null;}
    catch(InstantiationException e) {return null;}
    //finally {
    aObject.Init(this, x, y, args);
      //something went wrong...
    //return null;
    //};
      itsElements.addElement(aObject);
      if (offScreenPresent) {
	aObject.Paint(offGraphics);
	CopyTheOffScreen(getGraphics());
      }
      else aObject.Paint(getGraphics());
      if(theName == "ircam.jmax.editors.ermes.ErmesObjPatcher")
	itsPatcherElements.addElement(aObject);
      if (!itsToolBar.locked && editStatus != EDITING_OBJECT) editStatus = DOING_NOTHING;	
      aRect = new Rectangle(aObject.currentRect.x, aObject.currentRect.y, 
			    aObject.currentRect.width, aObject.currentRect.height);
      aRect.grow(3,6);
      itsElementRgn.Add(aRect);
      for (Enumeration e = aObject.GetOutletList().elements(); e.hasMoreElements();) {
	aOutlet = (ErmesObjOutlet)e.nextElement();
	itsConnectionSetList.addElement(aOutlet.GetConnectionSet());
      }
      
      ToSave();
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

    in.ChangeState(false, true); //warning: how many repaint() this function costs?
    out.ChangeState(false, true);//warning: how many repaint() this function costs?
    ToSave();
    return itsHelper.TraceConnection(out, in);
  }
  
  //--------------------------------------------------------
  //	AddSameObjectAs
  //  adding an object given a template of it, and initialization arguments
  //	(mainly for messages and externs substitutions)
  //--------------------------------------------------------
  public ErmesObject AddSameObjectAs(ErmesObject theObject, int x, int y, String args) {

    ErmesObject aObject = null;	//wasting time...
    Rectangle aRect;
    ErmesObjOutlet aOutlet;
    int i;
		
    String theName = theObject.getClass().getName();
    if(doSnapToGrid){
      Point aPoint = itsHelper.SnapToGrid(x, y);
      x = aPoint.x;
      y = aPoint.y;
    }
    try {
      //there was an error "aObject may not have been initialized"
      aObject = (ErmesObject) Class.forName(theName).newInstance();
    } catch(ClassNotFoundException e) {i = 0;}
    catch(IllegalAccessException e) {i = 1;}
    catch(InstantiationException e) {i = 2;}
    finally {
      aObject.Init(this, x, y, args);
      itsElements.addElement(aObject);
      aObject.Paint(getGraphics());
      if (!itsToolBar.locked) editStatus = DOING_NOTHING;	
      aRect = aObject.Bounds();
      aRect.grow(3,6);
      itsElementRgn.Add(aRect);
      for (Enumeration e = aObject.GetOutletList().elements(); e.hasMoreElements();) {
	aOutlet = (ErmesObjOutlet)e.nextElement();
	itsConnectionSetList.addElement(aOutlet.GetConnectionSet());
      }
    }
    ToSave();
    return aObject;
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
    for (Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(theFontName, sketchFont.getStyle(), aObject.GetFont().getSize());
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
       
	RemoveElementRgn(aObject);
	aObject.ChangeFont(aFont);
	SaveOneElementRgn(aObject);
      }
    }
    ToSave();
    repaint();
  }

  public void ChangeSizeFont(int fontSize){
    ErmesObject aObject;
    Font aFont;
    for (Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)||
	 (aObject instanceof ErmesObjIn)||(aObject instanceof ErmesObjOut)){
	aFont = new Font(aObject.GetFont().getName(), sketchFont.getStyle(), fontSize);
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
	
	RemoveElementRgn(aObject);
	aObject.ChangeFont(aFont);
	SaveOneElementRgn(aObject);
      }
    }
    ToSave();
    repaint();
  }

  public void ChangeJustification(String theJustification){
    int aJustificationMode = 0;
    if(theJustification.equals("Center")) aJustificationMode = CENTER_JUSTIFICATION;
    else if(theJustification.equals("Left")) aJustificationMode = LEFT_JUSTIFICATION;
    else if(theJustification.equals("Right")) aJustificationMode = RIGHT_JUSTIFICATION;

    if(itsSelectedList.size()==0) itsJustificationMode = aJustificationMode;

    ErmesObject aObject;
    for (Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment))
	aObject.ChangeJustification(aJustificationMode);
    }
    ToSave();
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
      itsHelper.DeselectInOutlet();
      if(evt.getClickCount()>1){   
	if(!evt.isShiftDown()) itsCurrentConnection.ChangeRoutingMode();
	else  itsCurrentConnection.ReroutingConn();
	
	itsSketchWindow.UpdateRoutingMenuWithSelection();

	editStatus = START_SELECT;
	ToSave();
	repaint();
      }
      else{
	itsSelectedConnections.addElement(itsCurrentConnection); 
	itsCurrentConnection.Select();
	editStatus = START_SELECT;///////
	itsSketchWindow.UpdateRoutingMenuWithSelection();
	if (offScreenPresent) {
	  itsCurrentConnection.Paint(offGraphics);
	  CopyTheOffScreen(getGraphics());
	}
	else
	  itsCurrentConnection.Paint(getGraphics());

	if((!itsCurrentConnection.GetErrorState())&&(itsCurrentConnection.GetAutorouted())
	   &&(itsHelper.IsMovable(itsSelectedSegment))){
	  MoveSegment(x,y);
	  RemoveConnRgn(itsCurrentConnection);
	  itsCurrentConnection.GetConnectionSet().RemoveRgn(itsCurrentConnection);
	  ToSave();
	}
      }
      break;
    case START_SELECT:
      if (!evt.isShiftDown()) {//lo shift non e' premuto
	itsHelper.DeselectAll(itsCurrentConnection);
	if(evt.getClickCount()>1){
	  itsCurrentConnection.ChangeRoutingMode();
	  ToSave();
	  repaint();
	}
	else{
	  itsSelectedConnections.addElement(itsCurrentConnection); 
	  //itsCurrentConnection.Select();
	  if((!itsCurrentConnection.GetErrorState())&(itsCurrentConnection.GetAutorouted())&&
	     (itsHelper.IsMovable(itsSelectedSegment))){
	    MoveSegment(x,y);
	    RemoveConnRgn(itsCurrentConnection);
	    itsCurrentConnection.GetConnectionSet().RemoveRgn(itsCurrentConnection);
	    ToSave();
	  }
	  itsCurrentConnection.Select();
	  itsCurrentConnection.Repaint();
	  itsSketchWindow.UpdateRoutingMenuWithSelection();
	}
      }
      else{//se c'e' lo shift premuto
	if(evt.getClickCount()>1){
	  itsCurrentConnection.ReroutingConn();
	  itsSketchWindow.UpdateRoutingMenuWithSelection();
	  ToSave();
	}
	else{
	  if(!(itsSelectedConnections.contains(itsCurrentConnection))){
	    itsSelectedConnections.addElement(itsCurrentConnection); 
	    itsCurrentConnection.Select();
	  }	
	  else {
	    itsSelectedConnections.removeElement(itsCurrentConnection);
	    itsCurrentConnection.Deselect();

	    if((itsSelectedConnections.size()) == 0)
	      editStatus = DOING_NOTHING;
	  }
	  itsSketchWindow.UpdateRoutingMenuWithSelection();
	}
	repaint();
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
	itsHelper.DeselectInOutlet();
	itsSelectedList.addElement(theObject);
	theObject.Select();
	CheckCurrentFont();
	if (offScreenPresent) {
	  theObject.Paint(offGraphics);
	  CopyTheOffScreen(getGraphics());   
	}
	else 
	  theObject.Paint(getGraphics());
	MoveSelected(theX,theY);
	ToSave();
	break;
      case START_SELECT:
	if (evt.isShiftDown()) {
	  if(!itsSelectedList.contains(theObject)){
	    itsSelectedList.addElement(theObject);
      	    theObject.Select();
	    CheckCurrentFont();
	    if (offScreenPresent) {
	      theObject.Paint(offGraphics);
	      CopyTheOffScreen(getGraphics());
	    }
	    else 
	      theObject.Paint(getGraphics());
	  }
	  else{
	    itsSelectedList.removeElement(theObject);	
	    theObject.Deselect();
	    if (offScreenPresent) {
	      theObject.Paint(offGraphics);
	      CopyTheOffScreen(getGraphics()); 
	    }
	    else 
	      theObject.Paint(getGraphics());
	    if(itsSelectedList.isEmpty()) 
	      editStatus = DOING_NOTHING;
	  }
	}
	else if(theObject.itsSelected) {
	  MoveSelected(theX, theY);
	  ToSave();
	}
	else {
	  itsHelper.DeselectAll();
	  // select the object involved
	  itsSelectedList.addElement(theObject);
	  theObject.Select();
	  CheckCurrentFont();
	  if (offScreenPresent) {
	    theObject.Paint(offGraphics);
	    CopyTheOffScreen(getGraphics());
	  }
	  else 
	    theObject.Paint(getGraphics());
	  MoveSelected(theX,theY);
	  ToSave();
	}
	break;
      }
      itsHelper.ChangeObjectPrecedence(theObject);
    }
    return true;
  }
  
  public void CopyTheOffScreen(Graphics g) {
    //this is a security check; should never happen, but 
    //a problem has detected during loads, when the Sketch is not visible yet, and some
    //component start to paint. This problem should be insulated
    if((g!= null)&&(offScreenPresent)) {
      if (isInGroup || copyPending) {
	copyPending = true;
      }
      else g.drawImage(offImage,0,0, this);
    }
  }

  //--------------------------------------------------------
  //	DoNothing
  //	message received from the ToolBar when an object is DEselected
  //--------------------------------------------------------
  public void DoNothing() {
    //itsToolBar.Deselect();
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
	aOutlet.GetConnectionSet().DrawCircles(offGraphics);
      }
      aObject.Paint(offGraphics);
    }
    for (Enumeration e =itsConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection) e.nextElement();
      aConnection.Paint(offGraphics);
    }
    
    //g.drawImage(offImage,0,0, this);
    CopyTheOffScreen(g);
  }
  
  //--------------------------------------------------------
  //	GetElementRegion
  //  return the elements region
  //--------------------------------------------------------
  public ErmesRegion GetElementRegion(){
    return itsElementRgn;
  }

  //--------------------------------------------------------
  //	GetElements
  //  return the elements list
  //--------------------------------------------------------
  public Vector GetElements(){
    return itsElements;
  }
  
  //--------------------------------------------------------
  //	GetHSegmRgn
  //  return the horizontal segments region
  //--------------------------------------------------------
  public ErmesRegion GetHSegmRgn(){
    return itsHSegmRgn;
  }

  //--------------------------------------------------------
  //	GetPatcherElements
  // 
  //--------------------------------------------------------
  public Vector GetPatcherElements(){
    return itsPatcherElements;
  }

  //--------------------------------------------------------
  //	GetVSegmRgn
  //  return the vertical segments region
  //--------------------------------------------------------
  public ErmesRegion GetVSegmRgn(){
    return itsVSegmRgn;
  }


  public Graphics GetOffGraphics() {
    Dimension d = preferredSize;
    
    //Create the offscreen graphics context, if no good one exists.
    if ( (offGraphics == null)){	//first sketch of the day. We do this even if it doesn't belong
      // || (d.width != offDimension.width)
      // || (d.height != offDimension.height)  
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
  //	GetSelectedList
  //  return the selected elements list
  //--------------------------------------------------------
  public Vector GetSelectedList(){
    return itsSelectedList;
  }
	
  //--------------------------------------------------------
  //	GetSketchWindow
  //--------------------------------------------------------
  
  public ErmesSketchWindow GetSketchWindow(){
    return itsSketchWindow;
  }
  
  /*2203 public ErmesObjMessThread GetMessThread(){
    return itsMessThread;
    }*/

  // note: the following function is a reduced version of InitFromFtsContainer.
  // better organization urges
  void PasteObjects(Vector objectVector, Vector connectionVector) {
    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    ErmesConnection aConnection;

    int objectX;    
    int objectY;
    
    itsHelper.DeselectAll();

    if (objectVector == null) return;
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
      fo.put("x", objectX+10);//offset by 10      
      fo.put("y", objectY+10);//offset by 10
      
      aObject = itsHelper.AddObject(objectClass, fo);
      itsSelectedList.addElement(aObject);
      aObject.Select();
       
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
      itsSelectedConnections.addElement(aConnection);
      aConnection.Select();
    }
    editStatus = START_SELECT;
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

      // MDC: this test has been added to allow loading patches with errors
      // in connections, so the debug can be done :->
      // Actually the error reporting should be redone !!!

      if (fc.checkConsistency()){
	fromObj = (ErmesObject) fc.getFrom().getRepresentation();
	toObj = (ErmesObject) fc.getTo().getRepresentation();
	aConnection = itsHelper.AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
      } else {
	System.out.println("Cannot connect object " + fc.getFrom() + " outlet " + fc.getFromOutlet() 
			   + " to object " + fc.getTo() + " inlet " + fc.getToInlet());
      }
    }
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
      theRequester.ChangeState(true, theRequester.connected);
    }
    else if (itsDirection == FromInToOut) {
      //deselection of old inlet
      if(itsConnectingLetList.size()!=0){
	for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	  aInlet = (ErmesObjInlet)e.nextElement();
	  aInlet.ChangeState(false, aInlet.connected);
	}
	ResetConnect();
      }
      else itsConnectingObj.ConnectionAbort(itsConnectingLet);
      
      if(theRequester!=itsConnectingLet){
	//selection of new outlet
	itsConnectingObj = theObject;
	itsConnectingLet = theRequester;
	itsDirection = FromInToOut;
	theRequester.ChangeState(true, theRequester.connected);
      }
      //else nothing to do
    }
    else {// FromOutToIn
      if(itsConnectingLetList.size()!=0) MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true);
	theRequester.ChangeState(false, true);
	itsHelper.TraceConnection((ErmesObjOutlet)itsConnectingLet, (ErmesObjInlet)theRequester);
	ResetConnect();
      }
    }
    return true;
  }
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesSketchPad(ErmesSketchWindow theSketchWindow) {    
    super();
    itsHelper = new ErmesSketchHelper(this);
    //FtsServer.getServer().addUpdateGroupListener(this);
    setLayout(null);
    preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);
    itsSketchWindow = theSketchWindow;
    itsElementRgn = new ErmesRegion();
    itsHSegmRgn = new ErmesRegion();
    itsVSegmRgn = new ErmesRegion();
    itsConnections = new Vector();
    itsInletList = new Vector();
    itsOutletList = new Vector();
    itsSelectedList = new Vector();
    itsElements = new Vector();
    itsSelectedConnections = new Vector();
    itsSelectedSegment = new ErmesConnSegment();
    itsStartMovingPt = new Point(0,0);    
    itsStartInclusionRect = new Rectangle();  
    itsConnectionSetList = new Vector();
    itsUpdateList = new Vector();
    itsPatcherElements = new Vector();
    itsConnectingLetList = new Vector();
        
    itsEditField = new ErmesObjEditField(this);
    //2203itsMessThread = new ErmesObjMessThread(this, "aFlash");
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
    if (lastSketchWithOffScreen == theSketchPad) return;
    if (lastSketchWithOffScreen!=null)
      lastSketchWithOffScreen.offScreenPresent = false;
    theSketchPad.offScreenPresent = true;
    lastSketchWithOffScreen = theSketchPad;
    theSketchPad.paintForTheFirstTime = true;
    //no check for now: change the OffScreen property
  }
  
  public void CheckCurrentFont(){
    ErmesObject aObject;
    String aFontName;
    Integer aSize;
    Integer aJustification;
    Vector aUsedFontVector = new Vector();
    Vector aUsedSizeVector = new Vector();
    Vector aUsedJustificationVector = new Vector();
    for (Enumeration en = itsSelectedList.elements(); en.hasMoreElements();) {
      aObject = (ErmesObject)en.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment)||
	 (aObject instanceof ErmesObjInt)||(aObject instanceof ErmesObjFloat)){
	aFontName = aObject.GetFont().getName().toLowerCase();
	aSize = new Integer(aObject.GetFont().getSize());
	aJustification = new Integer(aObject.GetJustification());
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
      
    //bug 1003.8: waiting for lock on Maurizio's modifs
    boolean isTopPatcher = (!((ErmesSketchWindow)itsSketchWindow).isSubPatcher);
    if (isTopPatcher && (itsAddObjectName.equals("ircam.jmax.editors.ermes.ErmesObjIn") || itsAddObjectName.equals("ircam.jmax.editors.ermes.ErmesObjOut"))) {
      //forbidden to add such objects in a top level patch
      ErrorDialog aErr = new ErrorDialog(itsSketchWindow, "Can't instantiate inlets/outlets in a Top level patcher");
      aErr.setLocation(100, 100);
      aErr.show();
      editStatus = DOING_NOTHING;
      return;
    }
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
    aObject.Paint(offGraphics);
    CopyTheOffScreen(getGraphics());
    if(itsAddObjectName == "ircam.jmax.editors.ermes.ErmesObjPatcher")
      itsPatcherElements.addElement(aObject);
    if (!itsToolBar.locked && editStatus != EDITING_OBJECT) editStatus = DOING_NOTHING;	
    aRect = new Rectangle(aObject.currentRect.x, aObject.currentRect.y, aObject.currentRect.width, aObject.currentRect.height);
    aRect.grow(3,6);
    itsElementRgn.Add(aRect);
    for (Enumeration en = aObject.GetOutletList().elements(); en.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet)en.nextElement();
      itsConnectionSetList.addElement(aOutlet.GetConnectionSet());
    }
    ToSave();
  }




  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}

  public void mousePressed(MouseEvent e){
    MaxApplication.setCurrentWindow(itsSketchWindow); //demo only: 4/03/98 0:55
    itsSketchWindow.requestFocus();//???
    
    //create the object whose name is 
    int x = e.getX();
    int y = e.getY();
    int i;
    
    if(!offScreenPresent){
      RequestOffScreen(this);
      DrawOffScreen(getGraphics());
      //itsFirstClick = false;
    }
    
    if (itsRunMode) {
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
      itsHelper.DeselectObjAndConn();
      if (e.isShiftDown()){
	MultiConnect(itsCurrentInOutlet);
      }
      else{
	if (!itsCurrentInOutlet.GetSelected()) {// no previously selected
	  itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
	}
	else {
	  itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet); 
	}
      }
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
    else{
      if (!e.isShiftDown()) itsHelper.DeselectAll();
      editStatus = AREA_SELECT;
      currentRect.setBounds(x,y,0,0);
      previousRect.setBounds(x,y,0,0);
      currentPoint.setLocation(x,y);
    }
  }
       
  public void mouseReleased(MouseEvent e){

    int x = e.getX();
    int y = e.getY();
    
    //(opt.) resetting the "firstclick" flag if safer but heavy 
    //(a repaint foreach mouseup...)
    //itsFirstClick = true;
    //RequestOffScreen(this);
    MaxApplication.setCurrentWindow(itsSketchWindow);
    if(itsScrolled) itsScrolled=false;

    if (itsRunMode) {

      if (itsStartDragObject != null) itsStartDragObject.MouseUp(e, x, y);
      itsStartDragObject = null;
      return;//why not?
    }
    if (editStatus == AREA_SELECT) {
      
      Rectangle aRect = itsHelper.NormalizedRect(currentRect);
      if (!aRect.isEmpty()) { 
	for (Enumeration en = itsElements.elements() ; en.hasMoreElements() ;) {
	  ErmesObject aObject = (ErmesObject) en.nextElement();
	  if (aObject.Bounds().intersects(aRect)) {
	    aObject.Select();
	    aObject.Paint(offGraphics);
	    itsSelectedList.addElement(aObject);
	  }
	}	
	CheckCurrentFont();
	
	SelectConnections();
	
	if (offScreenPresent) {
	  CopyTheOffScreen(getGraphics());
	}
	else {//should never happen, but..
	  DrawOffScreen(getGraphics());
	}
      }
      else if (!e.isShiftDown()) itsHelper.DeselectAll();
	       
      //currentRect = null;
      //currentPoint = null;
      if (itsSelectedList.isEmpty()) editStatus = DOING_NOTHING;
      else editStatus = START_SELECT;
      return;
    }
    else if (editStatus == MOVING) {
      int aDeltaH, aDeltaV;
      currentMovingBigRect.setBounds(0,0,0,0);
      //currentMovingLittleRect.setBounds(0,0,0,0);
      previousMovingBigRect.setBounds(0,0,0,0);
      //previousMovingLittleRect.setBounds(0,0,0,0);
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
	itsHelper.SaveElementRgn();
	itsHelper.MoveElemConnections(aDeltaH,aDeltaV);
	repaint();
      }
      //repaint();
      else {//se non ha mosso
	if(e.getClickCount() == 1){
	  if(oldEditStatus == START_SELECT){
	    if(itsCurrentObject instanceof ErmesObjEditableObject){
	      if(clickHappenedOnAnAlreadySelected) {
		itsHelper.DeselectAllInEditing(itsCurrentObject);
		((ErmesObjEditableObject)itsCurrentObject).RestartEditing();
	      }
	    }
	    else if(itsCurrentObject instanceof ErmesObjComment){
	      if(clickHappenedOnAnAlreadySelected) {
		itsHelper.DeselectAllInEditing(itsCurrentObject);
		((ErmesObjComment)itsCurrentObject).RestartEditing();
	      }
	    }
	  }
	}
      }
    }
    else if (editStatus == MOVINGSEGMENT){
      if(itsHelper.IsMovable(itsSelectedSegment)) {
	itsHelper.MoveDraggedSegment(currentMouseX-itsStartMovingPt.x, currentMouseY-itsStartMovingPt.y);
	ErmesConnection aConnection = itsSelectedSegment.GetConnection();
	SaveConnectionRgn(aConnection);
	aConnection.GetConnectionSet().SaveRgn(aConnection);
	aConnection.GetConnectionSet().UpdateCircles();
      }
      editStatus = START_SELECT;
      repaint();
    }
    else if (editStatus == RESIZING_OBJECT){
      itsCurrentObject.MouseUp(e,x,y);
      editStatus = START_SELECT;
      repaint();
    }
    else if(editStatus == DOING_NOTHING) return;
  }

  public void mouseEntered(MouseEvent e){}

  public void mouseExited(MouseEvent e){
    if (itsRunMode) return;		
    if(itsSketchWindow.getCursor().getType()==Cursor.CROSSHAIR_CURSOR){ 
      itsSketchWindow.setCursor(Cursor.getDefaultCursor());
      itsCurrentInOutlet.itsAlreadyMoveIn = false;
    }
  }

  void arrowsPressed(int whichArrow) {
    if (editStatus != START_SELECT) return;

    if (whichArrow == Platform.LEFT_KEY) {
      itsHelper.MoveElements(-1, 0);
      itsHelper.SaveElementRgn();
      itsHelper.MoveElemConnections(-1,0);
    }
    else if (whichArrow == Platform.RIGHT_KEY) {
      itsHelper.MoveElements(1, 0);
      itsHelper.SaveElementRgn();
      itsHelper.MoveElemConnections(1,0);
    }
    else if (whichArrow == Platform.UP_KEY) {
      itsHelper.MoveElements(0, -1);
      itsHelper.SaveElementRgn();
      itsHelper.MoveElemConnections(0,-1);
    }
    else if (whichArrow == Platform.DOWN_KEY) {
      itsHelper.MoveElements(0, 1);
      itsHelper.SaveElementRgn();
      itsHelper.MoveElemConnections(0,1);
    }
    repaint();
    
  }
  
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  public void SelectConnections(){
    ErmesConnection aConnection;
    for (Enumeration en = itsConnections.elements() ; en.hasMoreElements() ;) {
      aConnection = (ErmesConnection) en.nextElement();
      if((aConnection.GetSourceObject().itsSelected)&&(aConnection.GetDestObject().itsSelected)){	
	aConnection.Select();
	aConnection.Paint(offGraphics);
	itsSelectedConnections.addElement(aConnection);
	itsSketchWindow.UpdateRoutingMenuWithSelection();
      }
    }	
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

  public void mouseDragged(MouseEvent e){

    Rectangle aRect;
    int x = e.getX();
    int y = e.getY();
    
    if(itsRunMode) {
      if(itsStartDragObject != null) itsStartDragObject.MouseDrag(e, x, y);
      return;
    }

    DynamicScrolling(x, y);
    
    if(editStatus == AREA_SELECT) {
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
	 (x<currentResizeRect.x)) aWidth = currentResizeRect.width;
      else aWidth = x-currentResizeRect.x;

      if((java.lang.Math.abs(y-currentResizeRect.y)<itsResizingObject.getMinimumSize().height)||
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

      /*if((java.lang.Math.abs(x-currentResizeRect.x)<itsResizingObject.getMinimumSize().width)||
	(java.lang.Math.abs(y-currentResizeRect.y)<itsResizingObject.getMinimumSize().height)||
	(x<currentResizeRect.x)||(y<currentResizeRect.y)) return;
	if(itsResizeMode == BOTH_RESIZING)
	currentResizeRect.setSize(x-currentResizeRect.x, y-currentResizeRect.y);
	else if(itsResizeMode == HORIZONTAL_RESIZING)
	currentResizeRect.setSize(x-currentResizeRect.x, currentResizeRect.height);
	else if(itsResizeMode == VERTICAL_RESIZING)
	currentResizeRect.setSize(currentResizeRect.width, y-currentResizeRect.y);
	update(getGraphics());
	return;*/
    } 
    else if (editStatus == MOVING){
      repaint();
      if(itsStartInclusionRect.x+(x-itsStartMovingPt.x)>=0)
	currentMouseX = x;
      if(itsStartInclusionRect.y+(y-itsStartMovingPt.y)>=0)
	currentMouseY = y;
      repaint();
      return;
    }
    else if(editStatus == MOVINGSEGMENT){
      currentMouseX = x;
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
      if((e.isControlDown())&&(!itsCurrentInOutlet.itsAlreadyMoveIn)){
	if (e.isShiftDown()) MultiConnect(itsCurrentInOutlet);
	else{
	  if (!itsCurrentInOutlet.GetSelected())
	    itsCurrentInOutlet.GetOwner().ConnectionRequested(itsCurrentInOutlet);
	  else 
	    itsCurrentInOutlet.GetOwner().ConnectionAbort(itsCurrentInOutlet); 
	}
	itsCurrentInOutlet.itsAlreadyMoveIn = true;
      }
      return;
    }
    else 
      if(itsHelper.IsInObject(x,y))
	if(itsCurrentObject.MouseMove(e,x,y)) return;
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
  //	MoveSegment
  //--------------------------------------------------------
  public void MoveSegment(ErmesConnSegment theSegment, MouseEvent evt, int theX, int theY)
  {
    itsSelectedSegment = theSegment;
    editStatus = MOVINGSEGMENT;
    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    currentMouseX = theX;
    currentMouseY = theY;
  }
  
  public void MoveSegment(ErmesConnSegment theSegment, int theX, int theY){
    itsSelectedSegment = theSegment;
    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    currentMouseX = theX;
    currentMouseY = theY;
  }
  
  public void MoveSegment(int theX, int theY){
    editStatus = MOVINGSEGMENT;
    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    currentMouseX = theX;
    currentMouseY = theY;
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
    if(itsSelectedList.size()<MAX_NUM_SELECTED) FillVectorObjectRectangles();
  }

  void FillVectorObjectRectangles(){
    ErmesObject aObject;
    Rectangle aRect;
    for (Enumeration e = itsSelectedList.elements() ; e.hasMoreElements() ;) {
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
      itsSelectedList.addElement(aObject);
      aObject.Select();
    }
    CheckCurrentFont();
    for (Enumeration e = itsConnections.elements() ; e.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e.nextElement();
      itsSelectedConnections.addElement(aConnection);
      aConnection.Select();
    }
    itsSketchWindow.UpdateRoutingMenuWithSelection();
    repaint();
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
      theRequester.ChangeState(true, theRequester.connected);
    }
    else if (itsDirection == FromOutToIn) {
      //deselection of old outlet
      if(itsConnectingLetList.size()!=0){
	for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	  aOutlet = (ErmesObjOutlet)e.nextElement();
	  aOutlet.ChangeState(false,aOutlet.connected);
	}
	ResetConnect();
      }
      else itsConnectingObj.ConnectionAbort(itsConnectingLet);
      
      if(theRequester!=itsConnectingLet){
	//selection of new outlet
	itsConnectingObj = theObject;
	itsConnectingLet = theRequester;
	itsDirection = FromOutToIn;
	theRequester.ChangeState(true, theRequester.connected);
      }
      //else nothing to do
    }
    else {// FromInToOut
      if(itsConnectingLetList.size()!=0) MultiConnect(theRequester);
      else{
	itsConnectingLet.ChangeState(false, true);
	theRequester.ChangeState(false, true);
	itsHelper.TraceConnection((ErmesObjOutlet)theRequester, (ErmesObjInlet)itsConnectingLet);
	ResetConnect();
      }
    }
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
	theInOutlet.ChangeState(true, theInOutlet.connected);
      }
    }
    else{
      if(itsDirection == FromInToOut){
	if(theInOutlet.IsInlet()){
	  if(itsConnectingLetList.contains(theInOutlet)){
	    itsConnectingLetList.removeElement(theInOutlet);
	    theInOutlet.ChangeState(false, theInOutlet.connected);
	  }
	  else{
	    itsConnectingLetList.addElement(theInOutlet);
	    theInOutlet.ChangeState(true, theInOutlet.connected);
	  }
	}
	else {
	  for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	    aInlet = (ErmesObjInlet)e.nextElement();
	    aInlet.ChangeState(false, true);
	    itsHelper.TraceConnection((ErmesObjOutlet)theInOutlet, aInlet);
	  }
	  ResetConnect();
	}
      }
      else{
	if(!theInOutlet.IsInlet()){
	  if(itsConnectingLetList.contains(theInOutlet)){
	    itsConnectingLetList.removeElement(theInOutlet);
	    theInOutlet.ChangeState(false, theInOutlet.connected);
	  }
	  else{
	    itsConnectingLetList.addElement(theInOutlet);
	    theInOutlet.ChangeState(true, theInOutlet.connected);
	  }
	}
	else {
	  for (Enumeration e = itsConnectingLetList.elements(); e.hasMoreElements();) {
	    aOutlet = (ErmesObjOutlet)e.nextElement();
	    aOutlet.ChangeState(false, true);
	    itsHelper.TraceConnection(aOutlet, (ErmesObjInlet)theInOutlet);
	  }
	  ResetConnect();
	}
      }
    }
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
  //	RemoveConnRgn
  //	remove the connection's segments region from itsH/VSegmRgn
  //--------------------------------------------------------
  public void RemoveConnRgn(ErmesConnection theConnection){
    Rectangle aRect = new Rectangle();
    ErmesConnSegment aSegment;
    for (Enumeration e = theConnection.GetHSegmentList().elements(); e.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e.nextElement();
      aRect = aSegment.Bounds();
      itsHSegmRgn.Remove(aRect);
    }
    for (Enumeration e1 = theConnection.GetVSegmentList().elements(); e1.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e1.nextElement();
      aRect = aSegment.Bounds();
      itsVSegmRgn.Remove(aRect);
    }
  }
  
  //--------------------------------------------------------
  //	RemoveElementRgn
  //	remove the element region from itsElementRgn
  //--------------------------------------------------------
  public void RemoveElementRgn(ErmesObject theObject){
    Rectangle aRect = theObject.Bounds();
    aRect.grow(3,6);
    itsElementRgn.Remove(aRect);
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
  //--------------------------------------------------------
  //	SaveTo
  //	general saving method. 
  //--------------------------------------------------------
  boolean SaveTo(OutputStream stream) throws IOException {
    ErmesObject aObject = null;
    int index = 0;
    String put = new String();
    byte ric[] = new byte[256];
    byte end[] = {']', '\r', '\n'};
    //byte end
    
    //save the objects
    for (Enumeration e = itsElements.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      put = "set obj(" + index++ + ") [";
      put.getBytes(0, put.length(), ric, 0);
      stream.write(ric, 0, put.length());	//
      aObject.SaveTo(stream);	
      stream.write(end, 0, end.length);	
    }
    
    //save the connections
    ErmesConnection aConnection = null;
    for (Enumeration e1 = itsConnections.elements(); e1.hasMoreElements();) {
      aConnection = (ErmesConnection) e1.nextElement();
      put = 	"connect $obj(" + 
	itsElements.indexOf(aConnection.itsFromObject) +
	") " +
	aConnection.itsOutlet.itsOutletNum +
	" $obj(" +
	itsElements.indexOf(aConnection.itsToObject)+ 
	") "+ 
	aConnection.itsInlet.itsInletNum;
      put.getBytes(0, put.length(), ric, 0);
      stream.write(ric, 0, put.length());	//
      stream.write('\r');
      stream.write('\n');
    }
    
    return true;	//check this
  }
  
	
  //--------------------------------------------------------
  //	SaveConnectionRgn
  //	save the connection regions
  //--------------------------------------------------------
  void SaveConnectionRgn(ErmesConnection theConnection){
    Rectangle aRect = new Rectangle();
    ErmesConnSegment aSegment;
    for (Enumeration e = theConnection.GetHSegmentList().elements(); e.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e.nextElement();
      aRect = aSegment.Bounds();
      itsHSegmRgn.Add(aRect);
    }
    for (Enumeration e1 = theConnection.GetVSegmentList().elements(); e1.hasMoreElements();) {
      aSegment = (ErmesConnSegment)e1.nextElement();
      aRect = aSegment.Bounds();
      itsVSegmRgn.Add(aRect);
    }
  }
  
  public void SaveOneElementRgn(ErmesObject theObject){
    Rectangle aRect;
    aRect = theObject.Bounds();
    aRect.grow(3,6);
    itsElementRgn.Add(aRect);
  }
  
  public void SetAutorouting(boolean t) {
    //setting the routing mode could happen in two cases:
    //1). The demand is global (no connections selected)
    if(itsSelectedConnections.size()==0) {
      SetPatcherAutorouting(t);
    }
    else {
      //2. connections were selected, the demand is local to those connections
      SetSelectionAutorouting(t);
      repaint();
    }
  } 
  
  public void SetSelectionAutorouting(boolean t) {
    ErmesConnection aConnection;

    int selectionState = getSelectionRouting();

    if ((t?1:0) == selectionState) return; //nothing to do
    for (Enumeration e = itsSelectedConnections.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection)e.nextElement();
      if(aConnection.GetAutorouted() != t) aConnection.ChangeRoutingMode();
    }
  }

  //returns 1 if all the selected connections are autorouted,
  //0 if all the selected connections are not autorouted,
  //-1 otherwise 
  public int getSelectionRouting() {
    ErmesConnection aConnection;
    boolean currentValue;
    boolean allEquals = true;
    
    if (itsSelectedConnections.size() == 0) return -1;
    else if (itsSelectedConnections.size() == 1) return (((ErmesConnection)itsSelectedConnections.elementAt(0)).GetAutorouted())?1:0;
    //(else)
    currentValue = ((ErmesConnection)itsSelectedConnections.elementAt(0)).GetAutorouted();
    for (int i=0; i<itsSelectedConnections.size();i++) {
      allEquals = currentValue && ((ErmesConnection)itsSelectedConnections.elementAt(i)).GetAutorouted();
      if (!allEquals) return -1;
    }
    return (currentValue)?1:0;
  }

  public void SetPatcherAutorouting(boolean t){
    doAutorouting = t;
    ToSave();
  }
  
  public void SetResizeState(ErmesObject theResizingObject){
    editStatus = RESIZING_OBJECT;
    itsResizingObject = theResizingObject;
    if (currentResizeRect == null) currentResizeRect = new Rectangle();
    currentResizeRect.x = itsResizingObject.currentRect.x;
    currentResizeRect.y = itsResizingObject.currentRect.y;
    currentResizeRect.width = itsResizingObject.currentRect.width;
    currentResizeRect.height = itsResizingObject.currentRect.height;
    previousResizeRect.setBounds(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width, currentResizeRect.height);
  }
  
  public void SetRunMode(boolean theMode) {
    itsRunMode = theMode;
    setBackground(theMode?Color.white:sketchColor);
    if (theMode) itsHelper.DeselectAll();
    repaint(); 
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
      for(int i = 0; i<itsSelectedList.size();i++){
	aObject = (ErmesObject)itsSelectedList.elementAt(i);
	aPoint = itsHelper.SnapToGrid(aObject.itsX, aObject.itsY);
	aDeltaH = aPoint.x - aObject.itsX;
	aDeltaV = aPoint.y - aObject.itsY ;
	RemoveElementRgn(aObject);
	aObject.MoveBy(aDeltaH, aDeltaV);
	SaveOneElementRgn(aObject);
      }
      itsHelper.ReroutingAllConnections();//troppo...farlo solo per gli elem selezionati
      ToSave();
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
  /*provaSw public void SetToolBar(ErmesToolBar theToolBar) { questa riga sostituita a quella dopo*/
  public void SetToolBar(ErmesSwToolbar theToolBar) {
    itsToolBar = theToolBar;
  }
  
  //--------------------------------------------------------
  //	StartAdd
  //	message received from the ToolBar when an object is selected
  //--------------------------------------------------------
  public void StartAdd(int theObject) {
    itsHelper.DeselectAll();
    editStatus = START_ADD;
    itsAddObject = theObject;
  }
  
  public void startAdd(String theObject) {
    itsHelper.DeselectAll();
    editStatus = START_ADD;
    itsAddObjectName = theObject;
  }

  public void ToSave(){
    if(itsSketchWindow.isSubPatcher){
      if(itsSketchWindow.itsTopWindow!=null) 
	itsSketchWindow.itsTopWindow.itsSketchPad.ToSave();
    }
    else itsSketchWindow.ToSave();
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
    if (editStatus == AREA_SELECT) {
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
      int aNumSelected = itsSelectedList.size();
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
    else if (editStatus == MOVINGSEGMENT){
      if(itsHelper.IsMovable(itsSelectedSegment)){
	CopyTheOffScreen(g);//g.drawImage(offImage,0,0, this);
	Rectangle aRect = itsSelectedSegment.Bounds();
	if(itsHelper.IsHorizontal(aRect)) aRect.y+=(currentMouseY-itsStartMovingPt.y);
	else aRect.x+=(currentMouseX-itsStartMovingPt.x);
	g.setColor(Color.black);
	g.fillRect(aRect.x, aRect.y, aRect.width, aRect.height);
      }
      return;
    }
    else paint(g);
  }

  public void adjustmentValueChanged(AdjustmentEvent e){
    itsScrolled = true;
  }

  public void AlignSelectedObjects(String thePosition){
    ErmesObject aObject;
    int aValue;
    Vector aConnVector = new Vector();
    Vector aConnSetVector = new Vector();
    if(thePosition.equals("Top")){
      aValue = MinYSelected();
      for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	ConnectionsToRouting(aConnVector, aConnSetVector, aObject);
	RemoveElementRgn(aObject);
	aObject.MoveBy(0, aValue-aObject.GetY());
	SaveOneElementRgn(aObject);
      }
    }
    else if(thePosition.equals("Left")){
      aValue = MinXSelected();
      for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	ConnectionsToRouting(aConnVector, aConnSetVector, aObject);
	RemoveElementRgn(aObject);
	aObject.MoveBy(aValue-aObject.GetX(), 0);
	SaveOneElementRgn(aObject);
      }
    }
    else if(thePosition.equals("Bottom")){
      aValue = MaxYSelected();
      for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	ConnectionsToRouting(aConnVector, aConnSetVector, aObject);
	RemoveElementRgn(aObject);
	aObject.MoveBy(0, aValue-(aObject.GetY()+aObject.currentRect.height));
	SaveOneElementRgn(aObject);
      }
    }
    else if(thePosition.equals("Right")){
      aValue = MaxXSelected();
      for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
	aObject = (ErmesObject)e.nextElement();
	ConnectionsToRouting(aConnVector, aConnSetVector, aObject);
	RemoveElementRgn(aObject);
	aObject.MoveBy(aValue-(aObject.GetX()+aObject.currentRect.width), 0);
	SaveOneElementRgn(aObject);
      }
    }
    RerouteAlignedObjectConnections(aConnVector, aConnSetVector);
    DrawOffScreen(getGraphics());//repaint();
  }
  
  public int MinYSelected(){
    ErmesObject aObject;
    int aMinY = 10000;
    for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinY >= aObject.GetY()) aMinY = aObject.GetY();
    }
    return aMinY;
  }
  
  public int MinXSelected(){
    ErmesObject aObject;
    int aMinX = 10000;
    for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMinX >= aObject.GetX()) aMinX = aObject.GetX();
    }
    return aMinX;
  }

  public int MaxYSelected(){
    ErmesObject aObject;
    int aMaxY = -10000;
    for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxY<aObject.GetY()+aObject.currentRect.height) 
	aMaxY = aObject.GetY()+aObject.currentRect.height;
    }
    return aMaxY;
  }

   public int MaxXSelected(){
    ErmesObject aObject;
    int aMaxX = -10000;
    for(Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject)e.nextElement();
      if(aMaxX < aObject.GetX()+aObject.currentRect.width) 
	aMaxX = aObject.GetX()+aObject.currentRect.width;
    }
    return aMaxX;
  }

  private void RerouteAlignedObjectConnections(Vector theConnVector, Vector theConnSetVector){
    ErmesConnection aConnection;
    for(Enumeration e = theConnVector.elements(); e.hasMoreElements();) {
      aConnection = (ErmesConnection)e.nextElement();
      if(aConnection.itsAutorouted){
	if(!aConnection.GetErrorState()) {
	  RemoveConnRgn(aConnection);
	  aConnection.GetConnectionSet().RemoveRgn(aConnection);
	}
	aConnection.Delete();
	aConnection.PrepareToRouting();
	aConnection.AutoRouting();
	if(!aConnection.GetErrorState()){ 
	  SaveConnectionRgn(aConnection);
	  aConnection.GetConnectionSet().SaveRgn(aConnection);
	}
      }
      else aConnection.PrepareToRouting();
    }
    itsHelper.UpdateConnectionSet(theConnSetVector);
  }

  private void ConnectionsToRouting(Vector theConnVector, Vector theConnSetVector, ErmesObject aObject){
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
    for(Enumeration e = aObject.GetInletList().elements(); e.hasMoreElements();) {
      aInlet = (ErmesObjInlet)e.nextElement();
      for(Enumeration e1 = aInlet.GetConnections().elements(); e1.hasMoreElements();) {
	aConnection = (ErmesConnection)e1.nextElement();
	if(!theConnVector.contains(aConnection)) {
	  theConnVector.addElement(aConnection);
	  if(!theConnSetVector.contains(aConnection.GetConnectionSet())) 
	    theConnSetVector.addElement(aConnection.GetConnectionSet());
	}
      }
    }
    for(Enumeration e = aObject.GetOutletList().elements(); e.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet)e.nextElement();
      for(Enumeration e1 = aOutlet.GetConnections().elements(); e1.hasMoreElements();) {
	aConnection = (ErmesConnection)e1.nextElement();
	if(!theConnVector.contains(aConnection)) {
	  theConnVector.addElement(aConnection);
	  if(!theConnSetVector.contains(aConnection.GetConnectionSet())) 
	    theConnSetVector.addElement(aConnection.GetConnectionSet());
	}
      }
    }
  }  
}










