
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.GlobalProbe;

/**
 * The base class of all the graphic objects on the sketch.
 * This class has a knowledge of its corrisponding fos object. It contains
 * methods for:
 * - painting, selecting, moving, dragging (services to the sketch)
 * - create fos objects, change the value when the FTS value
 *   has changed (services to FTS), redefine itself.
 * - sending values when the user interact with the object
 * - calling the project manager to open the associated editor 
 *   (example: subpatchers, table, etc.)
 */
public class ErmesObject implements FtsPropertyHandler, ErmesArea, ErmesDrawable {

  public int itsX, itsY;
  int itsInitX, itsInitY;
  public boolean itsSelected = false;
  boolean laidOut;
  public Rectangle currentRect;
  public ErmesSketchPad	itsSketchPad;
  FtsContainerObject 	itsFtsPatcher;
  public FtsObject	itsFtsObject = null;
  public Vector itsInletList = new Vector();
  public Vector itsOutletList = new Vector();	
  final static int PADS_DISTANCE = 12;
  static final int DRAG_DIMENSION = 4;
  boolean	absolutePainting = false;
  boolean updated = false; ///////////////fast & furious synchronization
  boolean itsDragging = false;
  public Font itsFont = null;
  FontMetrics itsFontMetrics = null;
  int itsJustification = ErmesSketchPad.CENTER_JUSTIFICATION;
  //#@!boolean itsResized = false;
  static Color itsUINormalColor = new Color(153, 204, 255);
  static Color itsUISelectedColor = new Color(51, 153, 204);
  static Color itsLangNormalColor = new Color(153, 204, 204);
  static Color itsLangSelectedColor = new Color(51, 153, 153);
  Rectangle itsArea = new Rectangle();
  boolean itsDirtyFlag = true;

  public ErmesObject() {
    super();
  }
	
  public Rectangle getArea() {
    itsArea.setBounds(currentRect.x-3, currentRect.y-3, currentRect.width+6, currentRect.height+6);
    return itsArea;
  }

  public void setDirty(boolean b) {
    itsDirtyFlag = b;
  }

  public boolean getDirty() {
    return itsDirtyFlag;
  }
  
  void SaveTo(OutputStream stream) {
    String strArgs;

    // Modified by MDC: use directly the getDescription,
    // that return the original string the user typed
    // for the object.

    strArgs = itsFtsObject.getDescription();

    String temp = "newobj " + getClass().getName() + " " + String.valueOf(itsX)
      + " " + String.valueOf(itsY) + " \"" + strArgs.trim() + "\"";
    
    byte array[] = new byte[temp.length()];
    temp.getBytes(0, temp.length(), array, 0);
    try {
      stream.write(array);
    } catch (java.io.IOException IOe) {
      System.out.println("error in saving");
      IOe.printStackTrace(); // temporary, MDC
    }
  }

  Dimension getPreferredSize() {return new Dimension(0,0);};
  Dimension getMinimumSize() {return new Dimension(0,0);};
  
  final ErmesSketchWindow GetSketchWindow() {

    return (ErmesSketchWindow)(itsSketchPad.itsSketchWindow);
  }
  //
  public void propertyChanged(FtsObject obj, String name, Object value) {
    //the ipothesys is that value is of the correct type...
    FtsValueChanged(value);
  }

  protected void FtsValueChanged(Object value) {
    // do nothing
  }
  
  protected synchronized void Paint_specific(Graphics g) {}  
  public boolean MouseDown_specific(MouseEvent e, int x, int y) {return true;};  
  public void ChangeFont(Font theFont) {
    itsFont = theFont;
    itsFontMetrics =itsSketchPad.getFontMetrics(theFont);
    ResizeToNewFont(itsFont);
  }
  
  public void ChangeJustification(int theJustification) {
    itsJustification = theJustification;
  }

  public int GetJustification(){
    return itsJustification;
  }

  void ResizeToNewFont(Font itsFont) {setDirty(false);}
  
  public void Repaint()
  {
    Update(itsSketchPad.GetOffGraphics());
  }
  
  public void Update(Graphics g) {
    if(!itsSketchPad.itsGraphicsOn)return;
    g.setColor(itsSketchPad.getBackground());
    g.fillRect(itsX, itsY, currentRect.width, currentRect.height);
    Paint(g);
  }
  
  public void UpdateOnly(Graphics g) {
    if(!itsSketchPad.itsGraphicsOn)return;
    g.setColor(itsSketchPad.getBackground());
    g.fillRect(itsX, itsY, currentRect.width, currentRect.height);
  }


  /**
   * a method to paint the object AND its inlet/outlet
   * on and off screen
   */
  public void PaintComplete() {
    DoublePaint();
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    for (int i=0; i<itsInletList.size();i++) {
      aInlet = (ErmesObjInlet) itsInletList.elementAt(i);
      aInlet.DoublePaint();
    }
    for (int i=0; i<itsOutletList.size();i++) {
      aOutlet = (ErmesObjOutlet) itsOutletList.elementAt(i);
      aOutlet.DoublePaint();
    }
  }

  public  void Paint(Graphics g)
  {
    if(!itsSketchPad.itsGraphicsOn)return;
    if (itsSketchPad.isInGroup) {
      //emergency situation: ignore the Graphics and paint offScreen 
      Paint_specific(itsSketchPad.GetOffGraphics());
      itsSketchPad.drawPending = true;
    }
    else Paint_specific(g);
    /*old code
      if(!itsSketchPad.itsGraphicsOn)return;
      else Paint_specific(g);
      */
  }


  protected void DoublePaint() {
    //this double paint is usefull when an object change its state in run mode
    Graphics aGraphics = itsSketchPad.getGraphics();

    if (aGraphics != null) {	
      Paint(aGraphics);	
    }

    if (itsSketchPad.offScreenPresent && !itsSketchPad.itsRunMode)
      Paint(itsSketchPad.GetOffGraphics());
  }
	
  // This method is called during the inits
  public void update(FtsObject theFtsObject) {
    int i;
    ErmesObjInlet	 aErmesObjInlet;
    ErmesObjOutlet	 aErmesObjOutlet;
    int aHDist;
    
    // retrieve the inlet, outlet informations
    if (theFtsObject == null) {
      //this case have been seen during the instantiation of an external
      //"patcher <name> <nin> <nout>"
      return;
    }
    itsFtsObject = theFtsObject;
    int n_inlts = theFtsObject.getNumberOfInlets();
    int n_outlts = theFtsObject.getNumberOfOutlets();
    int in_local_distance = PADS_DISTANCE;
    int out_local_distance = PADS_DISTANCE;
    
    int old_ninlts = itsInletList.size();//used in case of redefines
    int old_noutlts = itsOutletList.size();//used in case of redefines
    
    
    Rectangle aRect = currentRect;
    int maxPads = (n_outlts > n_inlts) ? n_outlts : n_inlts;



    itsFtsObject.setRepresentation(this);
    if (maxPads * PADS_DISTANCE > aRect.width) { //the pads are longer then the element
      
      Reshape(aRect.x, aRect.y, maxPads*PADS_DISTANCE, aRect.height);
    }
    if (n_inlts > 1) in_local_distance = (aRect.width-10)/(n_inlts-1) ;
    if (n_outlts > 1) out_local_distance = (aRect.width-10)/(n_outlts-1) ;
    
    if (n_inlts > old_ninlts) {	//we added inlets...
      for (i=0; i< n_inlts; i++) {
	if (i<old_ninlts) {
	  aErmesObjInlet = (ErmesObjInlet) itsInletList.elementAt(i);
	  aErmesObjInlet.MoveTo(itsX+2+(i)*in_local_distance, aErmesObjInlet.itsY);
	  itsSketchPad.markSketchAsDirty();
	}
	else {
	  if (this instanceof ircam.jmax.editors.ermes.ErmesObjOut) 
	    aErmesObjInlet = new ErmesObjOutInlet(i, this, itsX+2+(i)*in_local_distance, itsY);
	  else aErmesObjInlet = new ErmesObjInlet(i, this, itsX+2+(i)*in_local_distance, itsY);
	  itsInletList.addElement(aErmesObjInlet);
	  itsSketchPad.AddInlet(aErmesObjInlet);
	  itsSketchPad.addToDirtyInOutlets(aErmesObjInlet);
	}
      }
    }
    else if (n_inlts <= old_ninlts) { //we reduced the number of inlets...
      if(n_inlts>1) aHDist = (currentRect.width-10)/(n_inlts-1);
      else aHDist=0;
      for (i=0; i< old_ninlts; i++) {
	if (i<n_inlts){
	  aErmesObjInlet = (ErmesObjInlet) itsInletList.elementAt(i);
	  aErmesObjInlet.MoveTo(itsX+2+(i)*aHDist,aErmesObjInlet.itsY);
	  itsSketchPad.markSketchAsDirty();
	}
	else{
	  //erase the inlet, and the associated connections
	  aErmesObjInlet = (ErmesObjInlet)itsInletList.elementAt(itsInletList.size()-1);
	  itsInletList.removeElementAt(itsInletList.size()-1);
	  itsSketchPad.RemoveInlet(aErmesObjInlet);
	  itsSketchPad.markSketchAsDirty();
	}
      }
      //yahooitsSketchPad.paintDirtyList();
    }
    
/////////    
    if (n_outlts>old_noutlts) {	//we added outlets...
     if(n_outlts>1) aHDist = (currentRect.width-10)/(n_outlts-1);
     else aHDist=0;
     for (i=old_noutlts; i< n_outlts; i++) {
       if(this instanceof ircam.jmax.editors.ermes.ErmesObjIn)
	 aErmesObjOutlet = new ErmesObjInletOutlet(i,this,itsX,itsY);
       else aErmesObjOutlet = new ErmesObjOutlet(i,this,itsX,itsY);
       itsOutletList.addElement(aErmesObjOutlet);
       itsSketchPad.AddOutlet(aErmesObjOutlet);
       itsSketchPad.addToDirtyInOutlets(aErmesObjOutlet);
     }
     for (i=0; i<itsOutletList.size(); i++) {
       aErmesObjOutlet = (ErmesObjOutlet) itsOutletList.elementAt(i);
       aErmesObjOutlet.MoveTo(itsX+2+i*aHDist, aErmesObjOutlet.itsY);
       if (old_noutlts > 0 && old_noutlts != n_outlts) itsSketchPad.markSketchAsDirty();
     }
    }
    else if (n_outlts <= old_noutlts) { //we reduced the number of outlets
      int size = itsOutletList.size()-1;
      for (i=n_outlts;i<old_noutlts;i++){
	aErmesObjOutlet = (ErmesObjOutlet)itsOutletList.elementAt(size);
	itsOutletList.removeElementAt(size);
	itsSketchPad.RemoveOutlet(aErmesObjOutlet);
	size--;
	itsSketchPad.markSketchAsDirty();
      }
      if(n_outlts>1) aHDist = (currentRect.width-10)/(n_outlts-1);
      else aHDist = 0;
      for (i=0;i<n_outlts;i++){
	aErmesObjOutlet = (ErmesObjOutlet) itsOutletList.elementAt(i);
	aErmesObjOutlet.MoveTo(itsX+2+i*aHDist, aErmesObjOutlet.itsY);
	itsSketchPad.markSketchAsDirty();
      }
    }
   
    //prepare to be waked up when values change
    if(NeedPropertyHandler()){
      itsFtsObject.watch("value", this);
    }
    //if (offGraphics!=null) PaintComplete();
  }
  
  public boolean NeedPropertyHandler(){
    return false;
  }

  //to be redefinded if needed.
  void putOtherProperties(FtsObject theFObject){}
  

// This init method is only called in "from skratch" initializations
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSelected = false;
    itsSketchPad = theSketchPad;
    itsFont = itsSketchPad.sketchFont;
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    laidOut = false;
    itsX = x;
    itsY = y;
		
    if (currentRect == null) makeCurrentRect(x, y);

    Reshape(itsX, itsY, getPreferredSize().width, getPreferredSize().height);
    
    itsFtsPatcher = itsSketchPad.GetSketchWindow().itsPatcher;
    makeFtsObject();
    if (itsFtsObject == null) return false;
    else update(itsFtsObject);
    itsSketchPad.addToDirtyObjects(this);
    return true;
  }

  public void makeFtsObject() {  }
  public void redefineFtsObject() {  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    int i;
    int width = 0, height = 0;
    itsSelected = false;
    itsSketchPad = theSketchPad;
    String aFont = (String)theFtsObject.get("font");
    Integer  aSize = (Integer)theFtsObject.get("fs");
    int aIntSize;
    
    if((aFont == null)&&(aSize == null)) itsFont = itsSketchPad.sketchFont;
    else{
      if(aFont == null) aFont = itsSketchPad.sketchFont.getName();
      if(aSize == null) aIntSize = itsSketchPad.sketchFont.getSize();
      else aIntSize = aSize.intValue();
      itsFont = new Font(aFont,itsSketchPad.sketchFont.getStyle(), aIntSize);
    }
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    
    if (currentRect == null) makeCurrentRect(theFtsObject);
    
    laidOut = false; //??
    
    itsFtsObject = theFtsObject;
    update(itsFtsObject);
    itsFtsPatcher = GetSketchWindow().itsPatcher;
    return true;
  }
  
  protected void makeCurrentRect(int x, int y) {
    Dimension d = getPreferredSize();
    currentRect = new Rectangle(x, y, d.width, d.height);
  }

  protected void makeCurrentRect(FtsObject theFtsObject) {
    int width=0;
    int  height=0;

    itsX = ((Integer)theFtsObject.get("x")).intValue();
    itsY = ((Integer)theFtsObject.get("y")).intValue();
    {
      Integer widthInt = (Integer) theFtsObject.get("w");
      if (widthInt != null)
	width  = widthInt.intValue();
    }
    
    {
      Integer heightInt = (Integer)theFtsObject.get("h");
      if (heightInt != null)
	height = heightInt.intValue();
    }
    
    if(width<10){
      width  = getMinimumSize().width;
    }
    if(height<10){
      height  = getMinimumSize().height;
    }
    currentRect = new Rectangle(itsX, itsY, width, height);
  }

  public boolean Select(boolean paintNow)
  {
    if (!itsSelected) {
      itsSelected = true;
      if (paintNow) DoublePaint();
      else itsSketchPad.addToDirtyObjects(this);
      return true;
    }
    return false;
  }

  public boolean Deselect(boolean PaintNow)
  {
    if (itsSelected) {
      itsSelected = false;
      if (PaintNow) DoublePaint();
      else itsSketchPad.addToDirtyObjects(this);
      return true;
    }
    return false;
  }
  
  public FtsObject GetFtsObject(){
    return itsFtsObject;
  }

  public Font GetFont(){
    return itsFont;
  }

  public Vector GetOutletList(){
    return itsOutletList;
  }
	
  public Vector GetInletList(){
    return itsInletList;
  }
  
  public ErmesSketchPad GetSketchPad(){
    return itsSketchPad;
  }
	
  public int GetX(){
    return itsX;
  }

  public int GetY(){
    return itsY;
  }

  public boolean ConnectionRequested(ErmesObjInOutlet theRequester)
  {
    // HERE the checking: is the type of connection requested allowed?
    if (!theRequester.IsInlet())	//if is an outlet...
      return (itsSketchPad.OutletConnect(this, theRequester));
    else return (itsSketchPad.InletConnect(this, theRequester)); // then, is it's an inlet
  }

  
  public boolean ConnectionAbort(ErmesObjInOutlet theRequester, boolean paintNow)
  {
    // HERE the checking: is the type of connection abort allowed?
    // (for now always allowed)
    theRequester.ChangeState(false, theRequester.connected, paintNow);
    itsSketchPad.ResetConnect();
    return true;	//for now, everything is allowed
  }
  
  public boolean IsResizedObject(int theWidth){
    return false;
  }
  
  public boolean MouseMove(MouseEvent e,int x,int y){
    if (itsSketchPad.itsRunMode || e.isControlDown()) return false;
    if(IsInDragBox(x,y)){
      GetSketchWindow().setCursor(Cursor.getPredefinedCursor(Cursor.SE_RESIZE_CURSOR));
      return true;
    }
    else return false;
  }
  
  public boolean MouseDrag(MouseEvent e,int x,int y){
    return MouseDrag_specific(e, x, y);
  }
		
  public boolean IsInDragBox(int x,int y){
    return (x >(currentRect.x+currentRect.width - DRAG_DIMENSION ) && 
	    x <(currentRect.x+currentRect.width) &&
	    y > (currentRect.y+currentRect.height-DRAG_DIMENSION) &&
	    y < (currentRect.y+currentRect.height) );    
  }

  public boolean MouseDown(MouseEvent e,int x, int y) {
    if (!itsSketchPad.itsRunMode && !e.isControlDown()){
      if((e.getClickCount()>1)&&(e.isShiftDown())) {
	RestoreDimensions(true);
	return true;
      }
      else 
	if (itsSelected) itsSketchPad.clickHappenedOnAnAlreadySelected =true;
	else itsSketchPad.clickHappenedOnAnAlreadySelected =false;
	if(IsInDragBox(x,y)) {
	  SetInitDrag(x,y);
	  return true;
	}
	else return MouseDown_specific(e, x, y);
    }
    else return MouseDown_specific(e, x, y);
  }
	
  public boolean MouseUp_specific(MouseEvent e, int x, int y){return false;}
  public boolean MouseDrag_specific(MouseEvent e, int x, int y) {return false;}
  
  public boolean MouseUp(MouseEvent e,int x,int y) {
    if (itsSketchPad.itsRunMode || e.isControlDown()) return MouseUp_specific(e, x, y);	       
    if(itsDragging) {
      if(itsSketchPad.itsResizeMode == itsSketchPad.BOTH_RESIZING){
	int aWidth, aHeight;
	boolean wrongWidth = java.lang.Math.abs(x-currentRect.x)<getMinimumSize().width ||
	  (x<currentRect.x);
	boolean wrongHeight = (java.lang.Math.abs(y-currentRect.y)<getMinimumSize().height) ||
	  (y<currentRect.y);

	if (wrongWidth && wrongHeight) RestoreDimensions(true);
	else if(wrongWidth) {

	  Resize(getMinimumSize().width - currentRect.width, y-itsInitY);
	  itsSketchPad.repaint();

	}
	else if( wrongHeight) {
	  Resize(x-itsInitX, getMinimumSize().height-currentRect.height);
	  itsSketchPad.repaint();
	}
	else {
	  Resize(x-itsInitX+1, y-itsInitY+1);
	}
	
      }
      else if(itsSketchPad.itsResizeMode == itsSketchPad.HORIZONTAL_RESIZING){
	if(IsResizeTextCompat(x-itsInitX, 0)) Resize(x-itsInitX, 0);
	else ResizeToText(x-itsInitX, 0);
      }
      else if(itsSketchPad.itsResizeMode == itsSketchPad.VERTICAL_RESIZING){
	if(IsResizeTextCompat(0, y-itsInitY)) Resize(0, y-itsInitY);
	else ResizeToText(0, y-itsInitY);
      }
      itsDragging = false;
      //#@!itsResized = true;
    }
    return false;
  }
  
  public void RestoreDimensions(boolean paintNow){
    //possible optimization: don't repaint if nothing changes
    Resize(getMinimumSize().width - currentRect.width, getMinimumSize().height-currentRect.height);
     if (paintNow) itsSketchPad.repaint();
     else itsSketchPad.addToDirtyObjects(this);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = currentRect.width+theDeltaX;
    int aHeight = currentRect.height+theDeltaY;
    if(aWidth<getMinimumSize().width) aWidth = getMinimumSize().width;
    if(aHeight<getMinimumSize().height) aHeight = getMinimumSize().height;
    Resize(aWidth-currentRect.width, aHeight-currentRect.height);
    setDirty(false);
  };
  
  public boolean IsResizeTextCompat(int x, int y){
    return true;
  }
	
  public void SetInitDrag(int theX, int theY){
    itsInitX = currentRect.x+currentRect.width;//favanga n.s.
    itsInitY = currentRect.y+currentRect.height;
    itsDragging = true;
    itsSketchPad.SetResizeState(this);
  }
  

  public void MoveBy(int theDeltaH, int theDeltaV) {
    int j;
    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    
    if (theDeltaH == 0 && theDeltaV == 0) return;
    itsX+=theDeltaH; itsY+=theDeltaV;
    currentRect.x = itsX;
    currentRect.y = itsY;	
    
    for (Enumeration e1=itsInletList.elements(); e1.hasMoreElements();) {
      aInlet = (ErmesObjInlet) e1.nextElement();
      aInlet.MoveBy(theDeltaH, theDeltaV);
    }
    for (Enumeration e2=itsOutletList.elements(); e2.hasMoreElements();) {
      aOutlet = (ErmesObjOutlet) e2.nextElement();
      aOutlet.MoveBy(theDeltaH, theDeltaV);
    }
  }

  public void Resize(int theDeltaH, int theDeltaV)
  {
    if (theDeltaH ==0 && theDeltaV ==0) return;
    if (-theDeltaH > currentRect.width || -theDeltaV > currentRect.height) return;
    currentRect.width += theDeltaH;
    currentRect.height += theDeltaV;
    

    ErmesObjInlet aInlet;
    ErmesObjOutlet aOutlet;
    int aInletDistance;
    if(itsInletList.size()>1){
      aInletDistance = (currentRect.width-10)/(itsInletList.size()-1);
    
      for(int i=1;i<itsInletList.size(); i++){
	aInlet = (ErmesObjInlet) itsInletList.elementAt(i);
	aInlet.MoveTo(itsX+2+i*aInletDistance, aInlet.itsY);
      }
    }	
    int aHDistance;
    //qui deve ricalcolare lo scarto tra le varie outlet
    if(itsOutletList.size() >1) aHDistance = (currentRect.width-10)/(itsOutletList.size()-1);
    else aHDistance = 0; 
    for(int j=0;j<itsOutletList.size(); j++){
      aOutlet = (ErmesObjOutlet) itsOutletList.elementAt(j);
      aOutlet.MoveTo(itsX+2+j*aHDistance, aOutlet.itsY+theDeltaV);
    }
  }
  
  public void Resize1(int w, int h) {
    if (currentRect.width == w && currentRect.height == h) return;
    currentRect.width = w;
    currentRect.height = h;
  }
  
  public void Reshape(int x, int y, int width, int height) {
    itsX = x;
    itsY = y;
    currentRect.x = itsX;
    currentRect.y = itsY;
    currentRect.width = width;
    currentRect.height = height;
  }
  
  public Rectangle Bounds() {
    Rectangle aRect = new Rectangle (currentRect.x, currentRect.y, currentRect.width, currentRect.height);
    return aRect;
  }
	
  public Rectangle GetDragBoxRect(){
    return new Rectangle(currentRect.x+currentRect.width-DRAG_DIMENSION,
			 currentRect.y+currentRect.height-DRAG_DIMENSION,
			 DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  public Dimension Size() {
    return (new Dimension(currentRect.width, currentRect.height));
  }

  void RunModeSetted(){
  }

}











