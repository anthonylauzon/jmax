package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "out" graphic object used in subpatchers.
 */
class ErmesObjOut extends ErmesObject {

  static Dimension preferredSize = new Dimension(20, 20);
  static Dimension minimumSize = new Dimension(15,15); 
  int itsId;

  public ErmesObjOut(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }
	
  //--------------------------------------------------------
  // Init
  //--------------------------------------------------------
  public void Init()
  {
    itsId = ((FtsOutletObject) itsFtsObject).getPosition();

    super.Init();
  }


  public void redefineFtsObject()
  {
    ((FtsOutletObject)itsFtsObject).setPosition(itsId);
  }
  
  public void MouseDown_specific(MouseEvent evt, int x, int y) {
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) {
      itsSketchPad.ClickOnObject(this, evt, x, y);
    }
    else  {	//we want to choose among the different Outlet number
      itsSketchPad.itsOutPop.SetNewOwner(this); //make the Choice pointing to this
      itsSketchPad.itsOutPop.show(itsSketchPad, getItsX(), getItsY());
    }
  }

  void ChangeOutletNo(int numberChoosen) {
    if (itsId != numberChoosen) {
      itsId = numberChoosen;
      redefineFtsObject();
    }
    itsSketchPad.repaint();
  }

  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsLangNormalColor);
    else g.setColor(itsLangSelectedColor);
    g.fillRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2);
    g.fill3DRect(getItsX()+2,getItsY()+2, getItsWidth()-4,  getItsHeight()-4, true);
    g.setColor(Color.black);
    //the box
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    //the triangle
    g.drawLine(getItsX()+1,getItsY()+ getItsHeight()-1, getItsX()+getItsWidth()/2, getItsY()+getItsHeight()/2);
    g.drawLine(getItsX()+getItsWidth()/2,getItsY()+ getItsHeight()/2  , getItsX()+getItsWidth()-1,getItsY()+getItsHeight()-1);	
    //the dragBox  
    //g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    g.setFont(getFont());
    g.drawString(""+(itsId+1), getItsX()+2, getItsY()+getItsHeight() +itsFontMetrics.getHeight()-1);
  }
	
  void ResizeToNewFont(Font theFont) {
    itsSketchPad.repaint();
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return minimumSize;
  }

  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    if(aWidth<getMinimumSize().width) aWidth = getMinimumSize().width;
    if(aHeight<getMinimumSize().height) aHeight = getMinimumSize().height;
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }
}


