
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

/**
 * The "toggle" graphic object.
 */
class ErmesObjToggle extends ErmesObject {
  boolean itsToggled = false;
  static Dimension preferredSize = new Dimension(20,20);
  static Color itsCrossColor = new Color(0, 0, 128);

  public ErmesObjToggle(){
    super();
  }
  
  
  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);	//set itsX, itsY
    return true;
  }
  
  // starting of the graphic/FTS mix
  
  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "toggle");
      }
    catch (FtsException e)
      {
	// Enzo !!! Aiuto :-> (MDC)
      }
  }

  public void redefineFtsObject()
  {
    // vtoggle do not redefine themselves
  }

  public boolean MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode) {
      itsToggled = !itsToggled;
      itsFtsObject.put("value", (itsToggled?1:0));
      //((FtsInteger) itsFtsActive).setValue(itsToggled?1:0);	//ENZOOOOO
      DoublePaint();
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }

  protected void FtsValueChanged(Object value) {
    // toggle the toggle
    boolean temp = (((Integer)value).intValue() == 1);
    if (itsToggled != temp) {
      itsToggled = temp;	
      DoublePaint();
    }
  }

  public boolean NeedPropertyHandler(){
    return true;
  }

  public void Paint_specific(Graphics g) {
    if(!itsSelected) g.setColor(itsUINormalColor/*Color.lightGray*/);
    else g.setColor(itsUISelectedColor/*Color.gray*/);
    g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
    g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
    g.setColor(Color.black);
    g.drawRect(itsX+0,itsY+ 0, currentRect.width-1, currentRect.height-1);
    if (itsToggled) {
      g.setColor(itsCrossColor);
      g.drawLine(itsX+4, itsY+4, itsX+currentRect.width-6, itsY+ currentRect.height-6);
      g.drawLine(itsX+currentRect.width-6, itsY+4,itsX+ 4,itsY+ currentRect.height-6);
    }
    g.setColor(Color.black);
     if(!itsSketchPad.itsRunMode) 
       g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }

  public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
    if((currentRect.width+theDeltaX < getMinimumSize().width)||
       (currentRect.height+theDeltaY < getMinimumSize().height))
      return false;
    else return true;
  }
  
  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return new Dimension(15,15); //(depending on the layout manager).
  }

  //If we don't specify this, the canvas might not show up at all
  //(depending on the layout manager).
  public Dimension getPreferredSize() {
    return preferredSize;
  }
}
