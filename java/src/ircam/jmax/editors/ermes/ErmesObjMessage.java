package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

//
// The "message box" graphic object.
//
class ErmesObjMessage extends ErmesObjEditableObject implements FtsPropertyHandler {

  boolean itsFlashing = false;

  public ErmesObjMessage(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    itsFtsObject.watch( "value", this);

    itsText.setText( getArgs());
    itsText.setWidth( getWidth());
    itsText.setFontMetrics( itsFontMetrics);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  String getArgs()
  {
    // Get the correct String from the object's "value" property, that may change
    return (String)itsFtsObject.get( "value");
  }

  // ----------------------------------------
  // White area offset
  // ----------------------------------------
//   private static final int WHITE_X_OFFSET = 3;
//   private static final int WHITE_Y_OFFSET = 3;
  private static final int WHITE_X_OFFSET = 4;
  private static final int WHITE_Y_OFFSET = 2;

  protected final int getWhiteXOffset()
  {
    return WHITE_X_OFFSET;
  }

  protected final int getWhiteYOffset()
  {
    return WHITE_Y_OFFSET;
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
//   private static final int TEXT_X_OFFSET = 4;
//   private static final int TEXT_Y_OFFSET = 3;
  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 2;

  protected final int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  protected final int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }


  void redefine( String text)
  {
    ((FtsMessageObject)itsFtsObject).setMessage( text);

    itsText.setText( text);
  }

  // Set the text; it is a try; the message box object in the
  // application layer take care of converting the message text to the
  // "value" property; it is an hack, should be done more cleanly.

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    if (name == "value")
      {
	// BUG: must clear the vector before.
	//ParseText( getArgs());

	if ( !canResizeBy( 0, 0)) 
	  {
	    ResizeToText( 0,0);
	    itsSketchPad.repaint();
	  }
	else
	  {
	    Graphics g = itsSketchPad.getGraphics();
	    Paint( g);
	    g.dispose();
	  }
      }
    else
      super.propertyChanged( obj, name, value);
  }

  public boolean isUIController() 
  {
    return true;
  }

  void MouseUp_specific( MouseEvent e, int x, int y)
  {
    if (itsFlashing) 
      {
	itsFlashing = false;

	// Should use a repaint with its rectangle

	Graphics g = itsSketchPad.getGraphics();
	Paint_specific( g);
	g.dispose();
      }
  }
  
  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	if (itsFtsObject != null)
	  {
	    itsFtsObject.sendMessage( 0, "bang", null);
	    itsFlashing = true;

	    Graphics g = itsSketchPad.getGraphics();
	    Paint_specific(g);
	    g.dispose();
	  }
      }
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  public void Paint_specific(Graphics g) 
  {
    if (g == null) 
      return;

    if ( !itsSketchPad.itsRunMode) 
      {
	if ( itsSelected || itsFlashing) 
	  g.setColor( itsLangSelectedColor);
	else 
	  g.setColor(itsLangNormalColor);
      }
    else 
      g.setColor(itsUINormalColor);
 
    g.fillRect( getX() + 1, getY() + 1, getWidth() - 2, getHeight() - 2);
    
    
    if ( !itsSketchPad.itsRunMode) 
      {
	if( itsFlashing) 
	  g.setColor(itsLangSelectedColor);
	else
	  {
	    if( !itsSelected) 
	      g.setColor(Color.white);
	    else
	      g.setColor(itsLangNormalColor);
	  }
      }
    else 
      {
	if ( itsFlashing || itsSelected)
	  g.setColor( itsUISelectedColor);
	else 
	  g.setColor(Color.white);
      }
  
    g.fillRect( getX()+getWhiteXOffset(), getY()+1, getWidth()-(getWhiteXOffset()*2), getHeight());
    
    g.setColor( Color.black);
    g.drawRect( getX()+0, getY()+0, getWidth()-1, getHeight()-1);
    
    g.setColor( Color.black);

    if( !itsSketchPad.itsRunMode) 
      g.fillRect( getX()+getWidth()-DRAG_DIMENSION, getY()+getHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont( getFont());
    DrawParsedString( g);
  }
}
