package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.utils.*;

/**
 * The "bang" graphic object.
 */
class ErmesObjBang extends ErmesObject implements FtsPropertyHandler {

  boolean itsFlashing = false;
  Color   itsFlashColor = Color.yellow;
  static Dimension preferredSize = new Dimension(20,20);
  static Dimension minimumSize = new Dimension(15, 15);

  public ErmesObjBang(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }
  
  public void MouseDown_specific(MouseEvent evt,int x, int y) {
    if (itsSketchPad.itsRunMode || evt.isControlDown()) {
      
      itsFtsObject.sendMessage(0, "bang", null);
    }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  
  public void Init()
  {
    super.Init();
    itsFtsObject.watch("value", this);
  }

  static Color bangColors[] = {Color.yellow,
			       Color.blue,
			       Color.cyan,
			       Color.green, 
			       Color.magenta,
			       Color.orange,
			       Color.pink,
			       Color.red,
			       Color.white,
			       Color.black};

                                
  public void propertyChanged(FtsObject obj, String name, Object value)
  {
    int flash = ((Integer) value).intValue();
    
    if (flash <=  0) 
      itsFlashing = false;
    else if (flash >= bangColors.length)
      {
	itsFlashing = true;
	itsFlashColor = Color.yellow;
      }
    else
      {
	itsFlashing = true;
	itsFlashColor = bangColors[flash - 1];
      }

    Paint_specific(itsSketchPad.getGraphics());
  }
	

  public boolean isUIController() {
    return true;
  }

	
  public void Paint_specific( Graphics g) 
  {
    if( !itsFlashing)
      {
	if ( !itsSelected) 
	  g.setColor( itsUINormalColor);
	else 
	  g.setColor( itsUISelectedColor);
      }
    else
      {
	g.setColor( Color.yellow);
      }

    g.fillRect( getItsX() + 1, getItsY() + 1, getItsWidth() - 2,  getItsHeight() - 2);
    g.fill3DRect( getItsX() + 2, getItsY() + 2, getItsWidth() - 4,  getItsHeight() - 4, true);
    g.setColor(Color.black);
    g.drawRect( getItsX(), getItsY(), getItsWidth() - 1, getItsHeight() - 1);
    g.drawOval( getItsX() + 5, getItsY() + 5, getItsWidth() - 10, getItsHeight() - 10);

    if( !itsSketchPad.itsRunMode) 
      g.fillRect( getItsX() + getItsWidth() -DRAG_DIMENSION, getItsY() + getItsHeight() - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }

	
  
  //--------------------------------------------------------
  // minimum and preferred sizes
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    return minimumSize; 
  }
  
  public Dimension getPreferredSize() {
    return preferredSize;
  }
  
}
















