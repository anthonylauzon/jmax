package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

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
public class ErmesObject implements FtsPropertyHandler {

  public int itsX, itsY;
  int itsInitX, itsInitY;
  public boolean itsSelected = false;
  boolean laidOut;
  public Rectangle currentRect;
  public ErmesSketchPad	itsSketchPad;
  FtsObject 	itsFtsPatcher;
  FtsGraphicDescription itsFtsGraphicDescription;
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
  boolean itsResized = false;
  static Color itsUINormalColor = new Color(153, 204, 255);
  static Color itsUISelectedColor = new Color(51, 153, 204);
  static Color itsLangNormalColor = new Color(153, 204, 204);
  static Color itsLangSelectedColor = new Color(51, 153, 153);
  
  public ErmesObject() {
    super();
  }
	
  void SaveTo(OutputStream stream) {
    String strArgs;
    Enumeration e = itsFtsObject.getArguments().elements();

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

  /*abstract */Dimension getPreferredSize() {return new Dimension(0,0);};
  
  //
  public void propertyChanged(String name, Object value) {
    //the ipothesys is that value is of the correct type...
    FtsValueChanged(value);
  }

  protected void FtsValueChanged(Object value) {
    int a = 1;
    
    a = a/(a*a);
  }
  
  /*abstract*/ public void Paint_specific(Graphics g) {};  
  /*abstract*/ public boolean MouseDown_specific(MouseEvent e, int x, int y) {return true;};  
  public void ChangeFont(Font theFont) {
    itsFont = theFont;
    itsFontMetrics =itsSketchPad.getFontMetrics(theFont);
    ResizeToNewFont(itsFont);
  }
  
  public void ChangeJustification(int theJustification) {
    itsJustification = theJustification;
  }


  void ResizeToNewFont(Font itsFont) {}
  
  public void Repaint()
  {
    Update(itsSketchPad.GetOffGraphics());
  }
  
  public void Update(Graphics g) {
    if(!itsSketchPad.itsGraphicsOn)return;
    g.setColor(itsSketchPad.getBackground());
    g.fillRect(itsX, itsY, getPreferredSize().width, getPreferredSize().height);
    Paint(g);
  }
  
  public  void Paint(Graphics g)
  {
    if(!itsSketchPad.itsGraphicsOn)return;
    Paint_specific(g);
  }


  protected void DoublePaint() {
    //this double paint is usefull when an object schange its state in run mode
    Graphics aGraphics = itsSketchPad.getGraphics();
    if (aGraphics != null) {	//for some reason, the Sketch may not be visible (yet?)
      Paint(aGraphics);	
    }

    if (itsSketchPad.offScreenPresent) Paint(itsSketchPad.GetOffGraphics());
  }
	
  // This method is called during the inits
  public void update(FtsObject theFtsObject) {
    int i;
    ErmesObjInlet	 aErmesObjInlet;
    ErmesObjOutlet	 aErmesObjOutlet;
    int aHDist;
    
    // retrieve the inlet, outlet informations
    int n_inlts = theFtsObject.getNumberOfInlets();
    int n_outlts = theFtsObject.getNumberOfOutlets();
    int in_local_distance = PADS_DISTANCE;
    int out_local_distance = PADS_DISTANCE;
    
    int old_ninlts = itsInletList.size();//used in case of redefines
    int old_noutlts = itsOutletList.size();//used in case of redefines
    
    
    Rectangle aRect = currentRect;
    int maxPads = (n_outlts > n_inlts) ? n_outlts : n_inlts;
    if (maxPads * PADS_DISTANCE > aRect.width) { //the pads are longer then the element
      Reshape(aRect.x, aRect.y, maxPads*PADS_DISTANCE, aRect.height);

    }
    if (n_inlts > 1) in_local_distance = (aRect.width-10)/(n_inlts-1) ;
    if (n_outlts > 1) out_local_distance = (aRect.width-10)/(n_outlts-1) ;
    
    Graphics g = itsSketchPad.getGraphics();//essay
    Graphics offGraphics = itsSketchPad.GetOffGraphics();
    if (n_inlts > old_ninlts) {	//we added inlets...
      for (i=0; i< n_inlts; i++) {
	if (i<old_ninlts) {
	  aErmesObjInlet = (ErmesObjInlet) itsInletList.elementAt(i);
	  aErmesObjInlet.MoveTo(itsX+2+(i)*in_local_distance, aErmesObjInlet.itsY);
	}
	else {
	  if (this instanceof ircam.jmax.editors.ermes.ErmesObjOut) 
	    aErmesObjInlet = new ErmesObjOutInlet(i, this, itsX+2+(i)*in_local_distance, itsY);
	  else aErmesObjInlet = new ErmesObjInlet(i, this, itsX+2+(i)*in_local_distance, itsY);
	  itsInletList.addElement(aErmesObjInlet);
	}
	if (offGraphics!= null) {
	  aErmesObjInlet.Repaint();
	  itsSketchPad.CopyTheOffScreen(g);
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
	  if (offGraphics!= null) {
	    aErmesObjInlet.Repaint();
	    itsSketchPad.CopyTheOffScreen(g);
	  }
	}
	else{
	  //erase the inlet, and the associated connections
	  itsInletList.removeElementAt(i);
	  //we should remove the connections. How?
	}
      }
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
     }
     for (i=0; i<itsOutletList.size(); i++) {
       aErmesObjOutlet = (ErmesObjOutlet) itsOutletList.elementAt(i);
       aErmesObjOutlet.MoveTo(itsX+2+i*aHDist, aErmesObjOutlet.itsY);
       if(offGraphics!= null){
	 aErmesObjOutlet.Repaint();
	 itsSketchPad.CopyTheOffScreen(g);
       }
     }
    }
    else if (n_outlts <= old_noutlts) { //we reduced the number of outlets
      int size = itsOutletList.size()-1;
      for (i=n_outlts;i<old_noutlts;i++){
	itsOutletList.removeElementAt(size);
	size--;
      }
      if(n_outlts>1) aHDist = (currentRect.width-10)/(n_outlts-1);
      else aHDist = 0;
      for (i=0;i<n_outlts;i++){
	aErmesObjOutlet = (ErmesObjOutlet) itsOutletList.elementAt(i);
	aErmesObjOutlet.MoveTo(itsX+2+i*aHDist, aErmesObjOutlet.itsY);
      }
      if(offGraphics!= null) itsSketchPad.repaint();//???????
    }
    //prepare to be waked up when values change
    itsFtsObject.installPropertyHandler("value", this);
    itsFtsObject.getProperty("value");
  }
  
  
// This init method is only called in "from skratch" initializations
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    itsSelected = false;
    itsSketchPad = theSketchPad;
    itsFont = itsSketchPad.sketchFont;
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    laidOut = false;
    itsX = x;
    itsY = y;
		
    Dimension d = getPreferredSize();
    currentRect = new Rectangle(x, y, d.width, d.height);
    Reshape(itsX, itsY, d.width, d.height);
    
    itsFtsPatcher = ((ErmesSketchWindow) (itsSketchPad.itsSketchWindow)).itsDocument.itsPatcher;
    makeFtsObject();
    if (itsFtsObject == null) return false;
    else update(itsFtsObject);
    return true;
  }

  public void makeFtsObject() {  }
  public void redefineFtsObject() {  }
  
  public boolean Init(ErmesSketchPad theSketchPad,FtsGraphicDescription theFtsDescription, FtsObject theFtsObject) {
    int i;
    int width, height;
    itsSelected = false;
    itsSketchPad = theSketchPad;
    itsFtsGraphicDescription = theFtsDescription;
    itsFont = itsSketchPad.sketchFont;
    itsFontMetrics = itsSketchPad.getFontMetrics(itsFont);
    laidOut = false;
    itsX = itsFtsGraphicDescription.x;
    itsY =itsFtsGraphicDescription.y;
    if((itsFtsGraphicDescription.width<10)||(itsFtsGraphicDescription.height<10)){
       width  = getPreferredSize().width;
       height  = getPreferredSize().height;
    }
    else{
      width = itsFtsGraphicDescription.width;
      height = itsFtsGraphicDescription.height;
    }
    currentRect = new Rectangle(itsFtsGraphicDescription.x, itsFtsGraphicDescription.y, width, height);
    //Reshape(itsX, itsY, width, height);
    if((itsFtsGraphicDescription.width != getPreferredSize().width)||
       (itsFtsGraphicDescription.height != getPreferredSize().height))
      itsResized = true;
    itsFtsObject = theFtsObject;
    update(itsFtsObject);
    itsFtsPatcher = ((ErmesSketchWindow) (itsSketchPad.itsSketchWindow)).itsDocument.itsPatcher;
    return true;
  }
  
  public boolean Select()
  {
    itsSelected = true;
    return true;
  }

  public boolean Deselect()
  {
    itsSelected = false;
    return true;
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
	
  public Frame GetSketchWindow() {
    return itsSketchPad.GetSketchWindow();
  }

  public /*abstract*/ boolean ConnectionRequested(ErmesObjInOutlet theRequester) {return true;};
  public /*abstract*/ boolean ConnectionAbort(ErmesObjInOutlet theRequester) {return true;};
  
  
  public boolean MouseMove(MouseEvent e,int x,int y){
    if (itsSketchPad.itsRunMode) return false;
    if(IsInDragBox(x,y)){
      GetSketchWindow().setCursor(Cursor.getPredefinedCursor(Cursor.SE_RESIZE_CURSOR));
      return true;
    }
    else return false;
  }
  
  public boolean MouseDrag(MouseEvent e,int x,int y){
    if (itsSketchPad.itsRunMode) return true;
    else return false;	
  }
		
  public boolean IsInDragBox(int x,int y){
    //return GetDragBoxRect().inside(x,y);
    return (x >(currentRect.x+currentRect.width - DRAG_DIMENSION ) && 
	    x <(currentRect.x+currentRect.width) &&
	    y > (currentRect.y+currentRect.height-DRAG_DIMENSION) &&
	    y < (currentRect.y+currentRect.height) );
    
    
  }

  public boolean MouseDown(MouseEvent e,int x, int y) {
    if (!itsSketchPad.itsRunMode){
      if((itsResized)&&(e.getClickCount()>1)&&(e.isShiftDown())) {
	RestoreDimensions();
	return true;
      }
      else 
	if(IsInDragBox(x,y)) {
	  SetInitDrag(x,y);
	  return true;
	}
	else return MouseDown_specific(e, x, y);
    }
    else return MouseDown_specific(e, x, y);
  }
	
  public boolean MouseUp(MouseEvent e,int x,int y) {
    if (itsSketchPad.itsRunMode) return false;		
    if(itsDragging) {
      itsSketchPad.RemoveElementRgn(this);
      if(IsResizeTextCompat(x-itsInitX, y-itsInitY)) Resize(x-itsInitX, y-itsInitY);
      else ResizeToText(x-itsInitX, y-itsInitY);
      itsSketchPad.SaveOneElementRgn(this);
      itsDragging = false;
      itsResized = true;
    }
    return false;
  }
  
  public void RestoreDimensions(){
    itsResized = false;
    itsSketchPad.RemoveElementRgn(this);
    Resize(getPreferredSize().width - currentRect.width, getPreferredSize().height-currentRect.height);
    itsSketchPad.SaveOneElementRgn(this);
    itsSketchPad.repaint();
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){};
  
  public boolean IsResizeTextCompat(int x, int y){
    return true;
  }
	
  public void SetInitDrag(int theX, int theY){
    itsInitX = theX;
    itsInitY = theY;
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
	ReroutingConnections(aInlet);
      }
    }	
    int aHDistance;
    //qui deve ricalcolare lo scarto tra le varie outlet
    if(itsOutletList.size() >1) aHDistance = (currentRect.width-10)/(itsOutletList.size()-1);
    else aHDistance = 0; 
    for(int j=0;j<itsOutletList.size(); j++){
      aOutlet = (ErmesObjOutlet) itsOutletList.elementAt(j);
      aOutlet.MoveTo(itsX+2+j*aHDistance, aOutlet.itsY+theDeltaV);
      ReroutingConnections(aOutlet);
    }
  }
  
  public void ReroutingConnections(ErmesObjInOutlet theInOutlet){
    ErmesConnection aConnection;
    for (Enumeration e2 = theInOutlet.GetConnections().elements() ; e2.hasMoreElements() ;) {
      aConnection = (ErmesConnection) e2.nextElement();
      if(aConnection.itsAutorouted){//only for the autorouted lines
	if(!aConnection.GetErrorState()) {
	  itsSketchPad.RemoveConnRgn(aConnection);
	  aConnection.GetConnectionSet().RemoveRgn(aConnection);
	}
	
	aConnection.Delete();
	aConnection.PrepareToRouting();
	aConnection.AutoRouting();
				//bug 24 aConnection.Paint(itsSketchPad.getGraphics());
	
	if(!aConnection.GetErrorState()){ 
	  itsSketchPad.SaveConnectionRgn(aConnection);
	  aConnection.GetConnectionSet().SaveRgn(aConnection);
	  aConnection.GetConnectionSet().UpdateCircles();
	}
      }
      else aConnection.PrepareToRouting();
    }
  }
  
  public void Resize1(int w, int h) {
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











