//	LA VERA SKETCH
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;
import java.io.*;

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
public class ErmesSketchPad extends Panel implements AdjustmentListener, MouseMotionListener, MouseListener/*, KeyListener */{
  Frame itsSketchWindow;
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

  int itsJustificationMode = CENTER_JUSTIFICATION;

  int inCount = 0;   //ref count of ErmesObjIn objects (used if this is a subpatcher)
  int outCount = 0;  //the same for ErmesObjOut objects
  ErmesObjInOutChoice itsInChoice = null;	//choice control associated with this sketch (sub-patch)
  ErmesObjInOutChoice itsOutChoice = null;
		
  int itsDirection = NoDirections;
  public boolean itsRunMode = false;
  boolean doSnapToGrid = false;
  public boolean doAutorouting = true;
  public boolean itsGraphicsOn = true;
  
  ErmesObjEditField itsEditField = null;
  ErmesObjTextArea itsTextArea = null;
  ErmesObject itsConnectingObj = null;  
  ErmesObjInOutlet itsConnectingLet = null;
  int currentMouseX, currentMouseY;	// used during the MOVING status
  
  ErmesRegion itsElementRgn;
  ErmesRegion itsHSegmRgn;
  ErmesRegion itsVSegmRgn;
  Vector itsConnections;
  public Vector itsSelectedList;
  Vector itsTempSelected;
  public Vector itsElements;
  Vector itsSelectedConnections;
  Vector itsConnectionSetList;
  Vector itsUpdateList;
  Vector itsPatcherElements;
  Rectangle currentRect = null;
  Point	currentPoint = null;
  Rectangle currentResizeRect = null; 

  public boolean offScreenValid = true;
  //Graphics offGraphicsLines = null;
  // Dimension offDimensionLines;
  //Image offImageLines;

  //Graphics offGraphicsElements = null;
  //Dimension offDimensionElements;
  //Image offImageElements;
  
  ErmesObjMessThread itsMessThread = null;

  boolean itsFirstClick = true;
  //STATIC OFFSCREEN!!!

  static ErmesSketchPad lastSketchWithOffScreen = null;
  boolean offScreenPresent = false;
  static Graphics offGraphics = null;
  static Dimension offDimension;
  static Image offImage;
		
		
  ErmesToolBar itsToolBar;
  ErmesConnSegment itsSelectedSegment;
  Point itsStartMovingPt;
  Rectangle itsStartInclusionRect;
  ErmesObjInOutlet itsCurrentInOutlet = null;
  ErmesObject itsCurrentObject = null;
  ErmesObject itsStartDragObject = null;
  ErmesObject itsResizingObject = null;
  ErmesConnection itsCurrentConnection = null;

  Vector itsConnectingLetList;

  int editStatus = DOING_NOTHING;

  static String objectNames[] ={ "ircam.jmax.editors.ermes.ErmesObjExternal",
				 "ircam.jmax.editors.ermes.ErmesObjMessage",
				 "ircam.jmax.editors.ermes.ErmesObjBang",
				 "ircam.jmax.editors.ermes.ErmesObjToggle",
				 "ircam.jmax.editors.ermes.ErmesObjInt",
				 "ircam.jmax.editors.ermes.ErmesObjFloat",
				 "ircam.jmax.editors.ermes.ErmesObjComment",
				 "ircam.jmax.editors.ermes.ErmesObjSlider",
				 "ircam.jmax.editors.ermes.ErmesObjIn",
				 "ircam.jmax.editors.ermes.ErmesObjOut",
				 "ircam.jmax.editors.ermes.ErmesObjPatcher"};

  Hashtable nameTable; // substitute name lists, initialized in the constructor.
  int itsAddObject;
  Rectangle resizeRect = new Rectangle();
  public ErmesSketchHelper itsHelper;


  boolean itsScrolled = false;

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
    finally {
      if (!aObject.Init(this, x, y, args)) {
	//something went wrong...
	return null;
      };
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

  void ChangeFont(Font theFont) {
    try {
      FontMetrics aFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics(theFont);
      setFont(theFont);   
    }
    catch (Exception e) {
      ErrorDialog aErr = new ErrorDialog(itsSketchWindow, "This font/fontsize does not exist on this platform");
      aErr.setLocation(100, 100);
      aErr.show();  
      return;
    }

    sketchFont = theFont;
    ErmesObject aObject;
    for (Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      RemoveElementRgn(aObject);
      aObject.ChangeFont(theFont);
      SaveOneElementRgn(aObject);
    }
    ToSave();
    repaint();
  }

  public void ChangeJustification(String theJustification){
    if(theJustification.equals("Center")) itsJustificationMode = CENTER_JUSTIFICATION;
    else if(theJustification.equals("Left")) itsJustificationMode = LEFT_JUSTIFICATION;
    else if(theJustification.equals("Right")) itsJustificationMode = RIGHT_JUSTIFICATION;

    ErmesObject aObject;
    for (Enumeration e = itsSelectedList.elements(); e.hasMoreElements();) {
      aObject = (ErmesObject) e.nextElement();
      if((aObject instanceof ErmesObjEditableObject)||(aObject instanceof ErmesObjComment))
	aObject.ChangeJustification(itsJustificationMode);
    }
    ToSave();
    repaint();

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
	
	editStatus = START_SELECT;
	ToSave();
	repaint();
      }
      else{
	itsSelectedConnections.addElement(itsCurrentConnection); 
	itsCurrentConnection.Select();
	editStatus = START_SELECT;///////
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
	}
      }
      else{//se c'e' lo shift premuto
	if(evt.getClickCount()>1){
	  itsCurrentConnection.ReroutingConn();
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
    if((g!= null)&&(offScreenPresent))
      g.drawImage(offImage,0,0, this);	
  }

  //--------------------------------------------------------
  //	DoNothing
  //	message received from the ToolBar when an object is DEselected
  //--------------------------------------------------------
  public void DoNothing() {
    itsToolBar.Deselect();
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
    
    g.drawImage(offImage,0,0, this);
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
  
  public Frame GetSketchWindow(){
    return itsSketchWindow;
  }
  

  public ErmesObjMessThread GetMessThread(){
    return itsMessThread;
  }

  //--------------------------------------------------------
  //	InitFromDocument
  //  create a sketchpad with data coming from a file
  // N.B. the Document argument is maybe unneeded, 
  // since it can be read in itsSketchWindow
  //--------------------------------------------------------
  public void InitFromDocument(ErmesPatcherDoc theDocument){
	
    FtsObject aFtsPatcher = theDocument.GetFtsPatcher();
    // chiama tanti AddObject...
    Vector objectVector = aFtsPatcher.getContainedObjects();	//usefull?
    
    FtsGraphicDescription fg;
    FtsObject	fo;
    FtsConnection fc;
    ErmesObject aObject;
    for (Enumeration e = objectVector.elements(); e.hasMoreElements();) {
      fo = (FtsObject)e.nextElement();
      fg = (FtsGraphicDescription) fo.getGraphicDescription();
      // Note that the representation is now found from the fts className,
      // made unique; the new file format will allow for specifing
      // additional information, like a non default graphic representation
      // the code will need a small change here

      String objectName = itsHelper.SearchFtsName(fo.getClassName());
      aObject = itsHelper.AddObject(fg, objectName, fo);
      //resizes the object to the dimensions 
      //les deux dimensions ne correspondent pas directement aux dimensions effectives
      //dans le format .pat??????????????????????????????????????
      //aObject.Resize1(fg.width, fg.height);//?????
      if (objectName == "ircam.jmax.editors.ermes.ErmesObjPatcher") itsPatcherElements.addElement(aObject);
      if (aObject != null) fo.setRepresentation(aObject);
    }
		
    // chiama tanti AddConnection...
    Vector connectionVector = aFtsPatcher.getContainedConnections();	//usefull?
    ErmesObject fromObj, toObj;
    ErmesConnection aConnection = null;
    
    for (Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) {
      fc = (FtsConnection)e2.nextElement();

      // MDC: this test has been added to allow loading patches with errors
      // in connections, so the debug can be done :->

      if (fc.checkConsistency()){
	fromObj = (ErmesObject) fc.getFrom().getRepresentation();
	toObj = (ErmesObject) fc.getTo().getRepresentation();
	aConnection = itsHelper.AddConnection(fromObj, toObj, fc.getFromOutlet(), fc.getToInlet(), fc);
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
  public ErmesSketchPad(Frame theSketchWindow) {    
    super();
    itsHelper = new ErmesSketchHelper(this);
    setLayout(null);
    preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);
    itsSketchWindow = theSketchWindow;
    itsElementRgn = new ErmesRegion();
    itsHSegmRgn = new ErmesRegion();
    itsVSegmRgn = new ErmesRegion();
    itsConnections = new Vector();
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
    itsMessThread = new ErmesObjMessThread(this, "aFlash");
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
    addKeyListener((ErmesSketchWindow)itsSketchWindow);

    // Initialization of the "fts class"  to "graphic object" table

    nameTable = new Hashtable(16, (float) 0.5);
    nameTable.put("message", "ircam.jmax.editors.ermes.ErmesObjMessage");
    nameTable.put("bang", "ircam.jmax.editors.ermes.ErmesObjBang");
    nameTable.put("toggle", "ircam.jmax.editors.ermes.ErmesObjToggle");
    nameTable.put("intbox", "ircam.jmax.editors.ermes.ErmesObjInt");
    nameTable.put("floatbox", "ircam.jmax.editors.ermes.ErmesObjFloat");
    nameTable.put("comment", "ircam.jmax.editors.ermes.ErmesObjComment");
    nameTable.put("slider", "ircam.jmax.editors.ermes.ErmesObjSlider");
    nameTable.put("inlet", "ircam.jmax.editors.ermes.ErmesObjIn");
    nameTable.put("outlet", "ircam.jmax.editors.ermes.ErmesObjOut");
    // At the moment, if we put patchers in the green box, we cannot edit them ??
    nameTable.put("patcher", "ircam.jmax.editors.ermes.ErmesObjExternal");
    // nameTable.put("patcher", "ircam.jmax.editors.ermes.ErmesObjPatcher");
  }
	
  static public void RequestOffScreen(ErmesSketchPad theSketchPad) {
    if (lastSketchWithOffScreen!=null)
      lastSketchWithOffScreen.offScreenPresent = false;
    theSketchPad.offScreenPresent = true;
    lastSketchWithOffScreen = theSketchPad;
    //no check for now: change the OffScreen property
  }
  
  //--------------------------------------------------------
  //	minimumSize
  //--------------------------------------------------------
    public Dimension getMinimumSize() {
        return new Dimension(30, 20);
    }
	
  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}

  public void mousePressed(MouseEvent e){
    itsSketchWindow.requestFocus();//???
    //create the object whose name is objectNames[addObject] in coord x, y
    int x = e.getX();
    int y = e.getY();
    int i;
    Rectangle aRect = null;
    ErmesObject aObject = null;
    ErmesObjOutlet aOutlet;
    ErmesConnection aConnection;
		
    
    if(itsFirstClick){
      DrawOffScreen(getGraphics());
      itsFirstClick = false;
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
    }
    if(editStatus == EDITING_COMMENT){
      itsTextArea.LostFocus();
    }
    ///if we are in a InOutLet
    if(itsHelper.IsInInOutLet(x,y)){
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
      itsCurrentObject.MouseDown(e,x,y);
      return;
    }
    
    ///if we are in a Connection
    if(itsHelper.IsInConnection(x,y)) {
      itsCurrentConnection.MouseDown(e,x,y);
      return;
    }
    
    if (!itsToolBar.locked) itsToolBar.Deselect();
    
    if(editStatus == START_ADD){
      if(doSnapToGrid){
	Point aPoint = itsHelper.SnapToGrid(x, y);
	x = aPoint.x;
	y = aPoint.y;
      }
      boolean isTopPatcher = (!((ErmesSketchWindow)itsSketchWindow).isSubPatcher);
      if (isTopPatcher && (objectNames[itsAddObject].equals("ircam.jmax.editors.ermes.ErmesObjIn") || objectNames[itsAddObject].equals("ircam.jmax.editors.ermes.ErmesObjOut"))) {
	//forbidden to add such objects in a top level patch
	ErrorDialog aErr = new ErrorDialog(itsSketchWindow, "Can't instantiate inlets/outlets in a Top level patcher");
	aErr.setLocation(100, 100);
	aErr.show();
	editStatus = DOING_NOTHING;
	return;
      }

      try
	{
	  //there was an error "aObject may not have been initialized"
	  aObject = (ErmesObject) Class.forName(objectNames[itsAddObject]).newInstance();
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
      if(objectNames[itsAddObject] == "ircam.jmax.editors.ermes.ErmesObjPatcher")
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
    else{
      if (!e.isShiftDown()) itsHelper.DeselectAll();
      editStatus = AREA_SELECT;
      currentRect = new Rectangle(x, y, 0, 0);
      currentPoint = new Point(x,y);
    }
  }
       
  public void mouseReleased(MouseEvent e){

    int x = e.getX();
    int y = e.getY();
    
    if(itsScrolled) itsScrolled=false;

    if (itsRunMode) {
      if (itsStartDragObject != null) itsStartDragObject.MouseUp(e, x, y);
      itsStartDragObject = null;
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
	if (offScreenPresent) {
	  CopyTheOffScreen(getGraphics());
	}
	else {//should never happen, but..
	  DrawOffScreen(getGraphics());
	}
      }
      else if (!e.isShiftDown()) itsHelper.DeselectAll();
	       
      currentRect = null;
      currentPoint = null;
      if (itsSelectedList.isEmpty()) editStatus = DOING_NOTHING;
      else editStatus = START_SELECT;
      return;
    }
    else if (editStatus == MOVING) {
      int aDeltaH, aDeltaV;
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
      }
      repaint();
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


  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  
  public boolean DynamicScrolling(int theX, int theY){
    Adjustable aHAdjustable =((ErmesSketchWindow)itsSketchWindow).itsScrollerView.getHAdjustable();
    Adjustable aVAdjustable =((ErmesSketchWindow)itsSketchWindow).itsScrollerView.getVAdjustable();
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
      if((java.lang.Math.abs(x-currentResizeRect.x)<5)||
	 (java.lang.Math.abs(y-currentResizeRect.y)<5)) return;
      currentResizeRect.setSize(x-currentResizeRect.x, y-currentResizeRect.y);
      update(getGraphics());
      return;
    } 
    else if (editStatus == MOVING){
      repaint();
      if((itsStartInclusionRect.x+(x-itsStartMovingPt.x)>0)&&
	 (itsStartInclusionRect.y+(y-itsStartMovingPt.y)>0)){
	currentMouseX = x;
	currentMouseY = y;
      }
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
    itsSketchWindow.setCursor(Cursor.getDefaultCursor());
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
    editStatus = MOVING;
    currentMouseX = theX;
    currentMouseY = theY;
    itsStartMovingPt.x = theX;
    itsStartMovingPt.y = theY;
    itsStartInclusionRect = itsHelper.StartMoveInclusionRect();
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
    if (itsInChoice != null) return; //it's OK, we did it already
    
    int temp = ((ErmesSketchWindow)itsSketchWindow).itsDocument.itsPatcher.getNumberOfInlets();
    itsInChoice = new ErmesObjInOutChoice();
    for (int i=0; i<temp; i++) {
      itsInChoice.addItem(Integer.toString(i+1));
    }
    itsInChoice.setLocation(-200, -200);
    itsInChoice.setVisible(false);
    add(itsInChoice);
  }

  void PrepareOutChoice() {
    if (itsOutChoice != null) return; //it's OK, we did it already
    
    int temp = ((ErmesSketchWindow)itsSketchWindow).itsDocument.itsPatcher.getNumberOfOutlets();
    itsOutChoice = new ErmesObjInOutChoice();
    for (int i=0; i<temp; i++) {
      itsOutChoice.addItem(Integer.toString(i+1));
    }
    itsOutChoice.setLocation(-200, -200);
    itsOutChoice.setVisible(false);
    add(itsOutChoice);
  }

  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
    public void paint(Graphics g) {
      if(itsScrolled){
	if (offScreenPresent) {
	  CopyTheOffScreen(getGraphics());
	}
	else {
	  DrawOffScreen(getGraphics());
	}
      }
      else
	if(itsGraphicsOn) DrawOffScreen(g);
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
  
  //??	void SaveSegmRgn(ErmesConnSegment theSegment){}
  public void SetAutorouting(){
    doAutorouting = !doAutorouting;
  }
  
  public void SetResizeState(ErmesObject theResizingObject){
    editStatus = RESIZING_OBJECT;
    itsResizingObject = theResizingObject;
    if (currentResizeRect == null) currentResizeRect = new Rectangle();
    currentResizeRect.x = itsResizingObject.currentRect.x;
    currentResizeRect.y = itsResizingObject.currentRect.y;
    currentResizeRect.width = itsResizingObject.currentRect.width;
    currentResizeRect.height = itsResizingObject.currentRect.height;
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
  public void SetToolBar(ErmesToolBar theToolBar) {
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
  

  public void ToSave(){
    ErmesSketchWindow aSketchWindow = (ErmesSketchWindow)itsSketchWindow;
    if(aSketchWindow.isSubPatcher){
      if(aSketchWindow.itsTopWindow!=null) 
	aSketchWindow.itsTopWindow.itsSketchPad.ToSave();
    }
    else aSketchWindow.GetDocument().ToSave();
  }


  public void update(Graphics g) {
    if(!itsGraphicsOn)return;
    ErmesSketchPad.debug_count_update += 1;
    if (debug_count_update % ErmesSketchPad.DEBUG_COUNT == 0) {
      /* System.out.println("uscito in update"); */
    }
     
    if (editStatus == AREA_SELECT) {
      g.drawImage(offImage,0,0, this);
      g.setColor(Color.black);
      g.drawRect(currentRect.x, currentRect.y, currentRect.width-1, currentRect.height-1);
    }
    else if (editStatus == RESIZING_OBJECT) {
      int x, y, w, h;
      g.drawImage(offImage,0,0, this);
      if (currentResizeRect.x + currentResizeRect.width >itsResizingObject.currentRect.x + 20&&
	  currentResizeRect.y + currentResizeRect.height>itsResizingObject.currentRect.y) {
	g.setColor(Color.black);
	g.drawRect(currentResizeRect.x, currentResizeRect.y, currentResizeRect.width-1, currentResizeRect.height-1);
      }
    }
    else if (editStatus == MOVING){
      g.drawImage(offImage,0,0, this);
      Rectangle aInclusionRect = itsHelper.StartMoveInclusionRect();
      Rectangle aRect /*= new Rectangle()*/;
      g.setColor(Color.black);
      aRect = itsCurrentObject.Bounds();
      aRect.x+=(currentMouseX-itsStartMovingPt.x);
      aRect.y+=(currentMouseY-itsStartMovingPt.y);
      aInclusionRect.x+=(currentMouseX-itsStartMovingPt.x);
      aInclusionRect.y+=(currentMouseY-itsStartMovingPt.y);
      g.drawRect(aRect.x, aRect.y, aRect.width-1, aRect.height-1);
      g.drawRect(aInclusionRect.x, aInclusionRect.y, aInclusionRect.width-1, aInclusionRect.height-1);
      return;
    }
    else if (editStatus == MOVINGSEGMENT){
      if(itsHelper.IsMovable(itsSelectedSegment)){
	g.drawImage(offImage,0,0, this);
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
}










