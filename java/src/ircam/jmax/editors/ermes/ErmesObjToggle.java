
package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

/**
 * The "toggle" graphic object.
 */
class ErmesObjToggle extends ErmesObject implements FtsPropertyHandler{
  boolean itsToggled = false;
  static Dimension preferredSize = new Dimension(20,20);
  static Dimension minimumSize = new Dimension(15, 15);
  static Color itsCrossColor = new Color(0, 0, 128);

  public ErmesObjToggle(){
    super();
  }
  
  
  public void makeFtsObject()
  {
    try
      {
	itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "toggle");
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
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      itsToggled = !itsToggled;
      itsFtsObject.put("value", (itsToggled?1:0));

      DoublePaint(); //!@#
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
    return true;
  }

  public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
    super.Init(theSketchPad, x, y, theString);
    itsFtsObject.watch("value", this);
    return true;
  }
  
  public boolean Init(ErmesSketchPad theSketchPad, FtsObject theFtsObject) {
    super.Init(theSketchPad, theFtsObject);
    itsFtsObject.watch("value", this);
    return true;
  }

  public void propertyChanged(FtsObject obj, String name, Object value) {
    // toggle the toggle
    boolean temp = (((Integer)value).intValue() == 1);
    if (itsToggled != temp) {
      itsToggled = temp;	
      Paint_specific(itsSketchPad.getGraphics());
    }
  }

  public boolean isUIController() {
   return false; 
  }

  public void Paint_specific(Graphics g) {
    if (g == null) return;
    if(!itsSelected) g.setColor(itsUINormalColor);
    else g.setColor(itsUISelectedColor);
    g.fillRect(getItsX()+1,getItsY()+1, getItsWidth()-2,  getItsHeight()-2);
    g.fill3DRect(getItsX()+2,getItsY()+2, getItsWidth()-4,  getItsHeight()-4, true);
    g.setColor(Color.black);
    g.drawRect(getItsX()+0,getItsY()+ 0, getItsWidth()-1, getItsHeight()-1);
    if (itsToggled) {
      g.setColor(itsCrossColor);
      g.drawLine(getItsX()+4, getItsY()+4, getItsX()+getItsWidth()-6, getItsY()+ getItsHeight()-6);
      g.drawLine(getItsX()+getItsWidth()-6, getItsY()+4,getItsX()+ 4,getItsY()+ getItsHeight()-6);
    }
    g.setColor(Color.black);
     if(!itsSketchPad.itsRunMode) 
       g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }

  //--------------------------------------------------------
  // minimum & preferred sizes
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return minimumSize; 
  }

  public Dimension getPreferredSize() {
    return preferredSize;
  }
}





