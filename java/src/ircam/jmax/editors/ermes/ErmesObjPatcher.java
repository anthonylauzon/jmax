package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

//
// The "patcher" graphic object. It knows the subpatchers it contains.
//

class ErmesObjPatcher extends ErmesObjEditableObject implements FtsPropertyHandler {

  // ----------------------------------------
  // Constructor
  // ----------------------------------------
  ErmesObjPatcher( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    itsFtsObject.watch( "ins", this);
    itsFtsObject.watch( "outs", this);    
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  String getArgs()
  {
    // Get the correct String from the object
    return itsFtsObject.getDescription().trim();
  }

  void redefine( String text) 
  {
    //the parent patcher could destroy connections...
    GetSketchWindow().itsPatcher.watch( "deletedConnection", GetSketchWindow());

    ( (FtsPatcherObject)itsFtsObject).redefinePatcher( text);
    itsText.setText( text);
  }
	
  public int MaxWidth( int uno, int due, int tre)
  {
    int MaxInt = uno;

    if( due>MaxInt) 
      MaxInt=due;
    if( tre>MaxInt) 
      MaxInt=tre;

    return MaxInt;
  }

  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  public void MouseDown_specific( MouseEvent evt,int x, int y) 
  {
    if ( evt.getClickCount() > 1)
      {
	itsSketchPad.waiting();
	Fts.editPropertyValue(itsFtsObject, "data",
			      new MaxDataEditorReadyListener() {
				public void editorReady(MaxDataEditor editor)
				  {itsSketchPad.stopWaiting();}
			      });
      }
    else
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void RestoreDimensions()
  {
    int aMaxWidth = MaxWidth( itsFontMetrics.stringWidth( "ZOB")+(itsFontMetrics.getHeight()+10)/2+5+5,
			      (itsInletList.size())*12,
			      (itsOutletList.size())*12);

    resizeBy( aMaxWidth - getWidth(), itsFontMetrics.getHeight() + 10 - getHeight());
  }

  // ----------------------------------------
  // White area offset
  // ----------------------------------------
  private static final int WHITE_X_OFFSET = 4;
  private static final int WHITE_Y_OFFSET = 4;

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
  private static final int TEXT_X_OFFSET = 5;
  private static final int TEXT_Y_OFFSET = 4;

  protected final int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  protected final int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  // ----------------------------------------
  // Inspector
  // ----------------------------------------
  public void inspect() 
  {
    ErmesPatcherInspector.inspect( (FtsContainerObject) itsFtsObject);
  }

  //--------------------------------------------------------
  // Paint stuff
  //--------------------------------------------------------
  public void Paint_specific( Graphics g) 
  {
    if( !itsSelected) 
      g.setColor( itsLangNormalColor);
    else 
      g.setColor( itsLangSelectedColor);

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);
    g.draw3DRect( x+3, y+3, w-7, h-7, false);
    
    g.setColor( Color.black);
    g.drawRect( x,y, w-1, h-1);
    if( !itsSketchPad.itsRunMode) 
      g.fillRect( x + w - DRAG_DIMENSION, y + h - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);

    g.setFont( getFont());
    DrawParsedString( g);
  }
}
