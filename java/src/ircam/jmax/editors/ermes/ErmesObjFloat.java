package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

//
// The graphic "float box" object.
//

class ErmesObjFloat extends ErmesObject implements FtsPropertyHandler {
  private float itsFloat = (float) 0.;

  private final int DEFAULT_VISIBLE_DIGIT = 3;
  private float itsStartingValue;

  // values relative to mouse dragging motion
  private float acceleration;
  private float velocity;
  private float previousVelocity;
  private int previousY;

  ErmesObjFloat( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);

    itsFtsObject.watch( "value", this);

    itsFloat = ((Float)itsFtsObject.get( "value")).floatValue();

    int h = itsFontMetrics.getHeight() + 4;
    if ( getHeight() < h) 
      setHeight( h);

    int w = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("...") + 17;
    if (getWidth() < w) 
      setWidth( w);

    recomputeInOutletsPositions();
  }

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    itsFloat = ((Float) value).floatValue();

    Graphics g = itsSketchPad.getGraphics();
    Paint_specific(g);
    g.dispose();
  }

  void FromDialogValueChanged(float theFloat) 
  {
    itsFloat = theFloat;

    itsFtsObject.put("value", theFloat);
    Repaint();
  }

  void ResizeToNewFont(Font theFont) 
  {
    ResizeToText(0,0);
  }

  void ResizeToText(int theDeltaX, int theDeltaY) 
  {
    int aWidth = getWidth()+theDeltaX;
    int aHeight = getHeight()+theDeltaY;

    if (( aWidth < aHeight/2 + 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..."))
	&& (aHeight < itsFontMetrics.getHeight()+4)) 
      {
	aWidth = getMinimumSize().width;
	aHeight = getMinimumSize().height;
      } else 
	{
	  if ( aWidth < aHeight/2 + 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..."))
	    aWidth = aHeight/2 + 17 +	itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("...");
	  if ( aHeight < itsFontMetrics.getHeight() + 4)
	    aHeight = itsFontMetrics.getHeight() + 4;
        }
    resizeBy( aWidth - getWidth(), aHeight - getHeight());
  }

  boolean canResizeBy(int theDeltaX, int theDeltaY) 
  {
    if ( ( getWidth() + theDeltaX < getHeight()/2 + 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("..."))
	 || ( getHeight() + theDeltaY < itsFontMetrics.getHeight() + 4))
      return false;
    else
      return true;
  }


  void RestoreDimensions() 
  {
    int tempWidth = 17 + itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT + itsFontMetrics.stringWidth("...");
    int tempHeight = itsFontMetrics.getHeight()+4;
    resizeBy(tempWidth - getWidth(), tempHeight - getHeight());
    // itsSketchPad.repaint(); // @@@ BARBOGIO
  }

  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;
    previousY = y;

    if (itsSketchPad.itsRunMode || evt.isControlDown()) 
      {
	itsStartingValue = itsFloat;

	itsFtsObject.put( "value", itsFloat);
      } 
    else
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }

  public void inspect() 
  {
    new ErmesObjFloatDialog(itsSketchPad.GetSketchWindow(), String.valueOf(itsFloat), this);
  }

  void MouseUp( MouseEvent evt,int x, int y) 
  {
    velocity = 0;
    previousVelocity = 0;
    acceleration = 0;

    if (itsSketchPad.itsRunMode) 
      {
	itsFtsObject.ask("value");
	Fts.sync();
	Repaint();
	return;
      } 

    super.MouseUp(evt, x, y);
  }

  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  boolean MouseDrag_specific(MouseEvent evt,int x, int y) 
  {
    previousVelocity = velocity;
    velocity = (previousY-y);
    acceleration = Math.abs(velocity-previousVelocity);
    previousY=y;

    if (itsSketchPad.itsRunMode || evt.isControlDown()) 
      {

	float increment;
	if (velocity*previousVelocity > 0)
	  increment = (velocity/1000) + ((velocity>0)?acceleration:-acceleration)/10;
	else
	  increment = velocity/1000;

	if (evt.isShiftDown())
	  increment*=10;

	itsFloat += increment;

	itsFtsObject.put("value", new Float(itsFloat));
	Repaint();

	return true;
      } 
    else
      return false;
  }

  boolean isUIController() 
  {
    return true;
  }

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  protected void Paint_specific(Graphics g) 
  {
    //draw the white area
    if (g == null)
      return;

    int xWhitePoints[] = {
      getX() + 3, 
      getX() + getWidth() - 3,
      getX() + getWidth() - 3, 
      getX()+3,
      getX()+getHeight()/2+3
    };

    int yWhitePoints[] = {
      getY()+1,
      getY()+1,
      getY()+getHeight()-1,
      getY()+getHeight()-1,
      getY()+getHeight()/2
    };

    if (!itsSelected)
      g.setColor(Color.white);
    else
      g.setColor(itsUINormalColor);
    g.fillPolygon(xWhitePoints, yWhitePoints, 5);

    //fill the triangle
    if (!itsSelected)
      g.setColor(itsUINormalColor);
    else
      g.setColor( itsUISelectedColor);

    g.fill3DRect( getX() + getWidth() - 4, getY() + 1, 3, getHeight()-2, true);

    int xPoints[]= { getX()+1, getX()+getHeight()/2+1, getX()+1};
    int yPoints[]= { getY(), getY() + getHeight()/2, getY()+getHeight()-1};
    g.fillPolygon(xPoints, yPoints, 3);

    //draw the outline
    g.setColor(Color.black);
    g.drawRect( getX()+0, getY()+0, getWidth()-1, getHeight()-1);

    //draw the triangle
    if (!itsSelected)
      g.setColor(itsUISelectedColor);
    else
      g.setColor(Color.black);

    g.drawLine( getX()+1, getY(), getX()+ getHeight()/2+1,getY()+getHeight()/2);
    g.drawLine( getX()+getHeight()/2+1, getY()+getHeight()/2, getX()+1, getY()+getHeight()-1);

    //draw the value
    String aString;
    //String aString2 = "..";
    if (itsFloat != 0)
      aString = GetVisibleString(String.valueOf(itsFloat));
    else
      aString = "0.0";
    g.setFont(getFont());
    g.setColor(Color.black);

    g.drawString( aString, 
		  getX()+getHeight()/2+5, 
		  getY()+itsFontMetrics.getAscent() + (getHeight()-itsFontMetrics.getHeight())/2+1);

    if (!itsSketchPad.itsRunMode)
      g.fillRect( getX() + getWidth()-DRAG_DIMENSION,
		  getY()+getHeight()-DRAG_DIMENSION,
		  DRAG_DIMENSION, 
		  DRAG_DIMENSION);
  }

  String GetVisibleString(String theString) 
  {
    String aString = theString;
    String aString2 = "..";
    int aStringLength = theString.length();
    int aCurrentSpace = getWidth() - (getHeight()/2 + 8) - 3;
    int aStringWidth = itsFontMetrics.stringWidth( aString);

    if (aStringWidth < aCurrentSpace)
      return aString;

    while (( aCurrentSpace <= aStringWidth ) && (aString.length() > 0))
      {
	aString = aString.substring( 0, aString.length() - 1);
	aStringWidth = itsFontMetrics.stringWidth( aString);
      }

    if ( ( aStringWidth + itsFontMetrics.stringWidth("..") >= aCurrentSpace)
	 && ( aString.length() > 0) )
      aString = aString.substring( 0, aString.length() - 1);

    aString = aString + aString2;
    return aString;
  }



  // ----------------------------------------
  // old stuff
  // ----------------------------------------
  Dimension getMinimumSize() 
  {
    new Throwable( this.getClass().getName()).printStackTrace();
    return new Dimension(  400, 400);
  }
}
