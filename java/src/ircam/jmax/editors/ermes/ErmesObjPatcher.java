package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

import ircam.jmax.utils.*;

/**
 * The "patcher" graphic object. It knows the subpatchers it contains.
 */

public class ErmesObjPatcher extends ErmesObjEditableObject implements FtsPropertyHandler
{
  public ErmesSketchWindow itsSubWindow = null;
  Dimension preferredSize = new Dimension(80,24);
 
  final static int PATCHER_HEIGHT_DIFF = 5;

  //--------------------------------------------------------
  // Constructor
  //--------------------------------------------------------
  public ErmesObjPatcher(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }
	
  protected int getWhiteOffset() {
    return 10;
  }


  public void Init()
  {
    itsArgs = itsFtsObject.getDescription().trim();

    super.Init();
    resizeBy(0, itsFontMetrics.getHeight()+2*PATCHER_HEIGHT_DIFF-getItsHeight());
    ParseText(itsArgs);
    
    if ((! itsArgs.equals("")) &&(! canResizeBy( 0, 0)))
      RestoreDimensions();

    itsFtsObject.watch("ins", this);
    itsFtsObject.watch("outs", this);    
  }

  public boolean inspectorAlreadyOpen() {
    return (ErmesPatcherInspector.isOpen() && ErmesPatcherInspector.getInspectedObject() == itsFtsObject);
  }

  public void openInspector() {
    ErmesPatcherInspector.inspect((FtsContainerObject) itsFtsObject);
  }

  public void redefineFtsObject()
  {
    //the parent patcher could destroy connections...
    GetSketchWindow().itsPatcher.watch("deletedConnection",GetSketchWindow());

    //the children could destroy connections AND objects: NO MORE<
    // changed: FTS do not destroy the patcher content, and never will
    // again

    ((FtsPatcherObject)itsFtsObject).redefinePatcher(itsArgs);
    if (itsSubWindow != null)  {
      itsSubWindow.itsSketchPad.RedefineInChoice();
      itsSubWindow.itsSketchPad.RedefineOutChoice();
    }
  }
  
	
  public int MaxWidth(int uno, int due, int tre){
    int MaxInt = uno;
    if(due>MaxInt) MaxInt=due;
    if(tre>MaxInt) MaxInt=tre;
    return MaxInt;
  }

  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
	  

  public void MouseDown_specific(MouseEvent evt,int x, int y) {
    if (evt.getClickCount()>1)
      {
	try
	  {
	    MaxDataEditor editor;
	    MaxData data;

	    itsSketchPad.waiting();

	    data = ((FtsObjectWithData) itsFtsObject).getData();

	    editor = Mda.edit(data);

	    // Add ready listener

	    editor.addEditorReadyListener(new MaxEditorReadyListener() {
	      public void editorReady(MaxDataEditor editor) { itsSketchPad.stopWaiting();}
	    });
	  }
	catch (MaxDocumentException e)
	  {
	    // Really a system error here
	    System.err.println(e);
	  }
      }
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }
	
  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fill3DRect(getItsX()+1, getItsY()+1, getItsWidth()-2, getItsHeight()-2, true);
    g.draw3DRect(getItsX()+3, getItsY()+3, getItsWidth()-6, getItsHeight()-6, false);
    
    int xPoints[] = {getItsX()+7,getItsX()+7,getItsX()+13};
    int yPoints[] = {getItsY()+6,getItsY()+18,getItsY()+12};
    g.fillPolygon(xPoints, yPoints, 3);
    g.setColor(Color.black);
    g.drawRect(getItsX()+0,getItsY()+ 0, getItsWidth()-1, getItsHeight()-1);
    g.drawRect(getItsX()+4, getItsY()+4, getItsWidth()-8, getItsHeight()-8);
    
    g.setFont(getFont());
    DrawParsedString(g);

    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  void ResizeToNewFont(Font theFont) {
    ResizeToText(0,0);
  }
	
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    if(aHeight<itsFontMetrics.getHeight() + 10) 
      aHeight = itsFontMetrics.getHeight() + 10;
    if(aWidth<itsFontMetrics.stringWidth(itsMaxString) + aHeight/2+5+5) 
      aWidth = itsFontMetrics.stringWidth(itsMaxString) + aHeight/2+5+5;
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
  
  public boolean canResizeBy(int theDeltaX, int theDeltaY){
    if((getItsWidth()+theDeltaX < itsFontMetrics.stringWidth(itsMaxString)+getItsHeight()/2+5+5)||(getItsHeight()+theDeltaY<itsFontMetrics.getHeight() + 10))
      return false;
    else return true;
  }

    public void RestoreDimensions(){

    int aMaxWidth = MaxWidth(itsFontMetrics.stringWidth(itsMaxString)+(itsFontMetrics.getHeight()+10)/2+5+5,(itsInletList.size())*12, (itsOutletList.size())*12);
    resizeBy(aMaxWidth-getItsWidth(), itsFontMetrics.getHeight() + 10 - getItsHeight());
    // itsSketchPad.repaint(); // @@@@ BARBOGIO
  }

}












