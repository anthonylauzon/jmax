package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

//
// The generic "extern" object in ermes. (example: adc1~)
//
class ErmesObjExternal extends ErmesObjEditableObject implements FtsPropertyHandler {

  private int isError = -1; // cache of the error property, to speed up paint

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjExternal( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    itsFtsObject.watch("error", this);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  String getArgs()
  {
    // Get the correct String from the object
    return itsFtsObject.getDescription().trim();
  }

  public void propertyChanged( FtsObject obj, String name, Object value) 
  {
    // Handle the "error" property, the only one we're listening at.
    // Call super for the others

    if (name == "error") 
      {
	System.err.println("Got value for error " + value + " in error " + obj);
	if ((value != null) && (value instanceof Integer)) 
	  {
	    isError = ((Integer)value).intValue();
	    DoublePaint();
	    itsSketchPad.repaint();// ??? 
	  }
      } 
    else
	super.propertyChanged( obj, name, value);
  }

  /* Inspector */

  public void inspect() 
  {
    if ( itsFtsObject instanceof FtsContainerObject)
      ErmesPatcherInspector.inspect( (FtsContainerObject) itsFtsObject);
  }

  void redefine( String text) 
  {
    try 
      {
	itsFtsObject = Fts.redefineFtsObject( itsFtsObject, text);

	itsFtsObject.watch("ins", this);
	itsFtsObject.watch("outs", this);
	itsFtsObject.watch("error", this);
	isError = -1;
      } 
    catch (FtsException e) 
      {
	System.out.println("Error in redefining object, action cancelled");
      }

    itsText.setText( text);
  }

  public void MouseDown_specific( MouseEvent evt,int x, int y) 
  {
    if ( evt.getClickCount() > 1 ) 
      {
	itsSketchPad.waiting();
	Fts.editPropertyValue( itsFtsObject, "data", new MaxDataEditorReadyListener() {
	  public void editorReady(MaxDataEditor editor)
	    {itsSketchPad.stopWaiting();}
	});
      }
    else if ( !itsSketchPad.itsRunMode) 
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void startEditing() 
  {
    isError = 0; // to get the edited object green
    super.startEditing();
  }

  // ----------------------------------------
  // White area offset
  // ----------------------------------------
  // Settings for 20 pixels height
//   private static final int WHITE_X_OFFSET = 3;
//   private static final int WHITE_Y_OFFSET = 3;
  // Settings for 18 pixels height
  private static final int WHITE_X_OFFSET = 2;
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
  // Settings for 20 pixels height
//   private static final int TEXT_X_OFFSET = 4;
//   private static final int TEXT_Y_OFFSET = 2;
  // Settings for 18 pixels height
  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 1;

  protected final int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  protected final int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  // ----------------------------------------
  // Paint stuff
  // ----------------------------------------
  public void Paint_specific(Graphics g) 
  {
    if (isError == -1)
      isError = ((Integer)itsFtsObject.get( "error")).intValue();

    if (isError == 0) 
      {
	if (! itsSelected)
	  g.setColor( itsLangNormalColor);
	else
	  g.setColor( itsLangSelectedColor);
      } 
    else
      g.setColor(Color.red);

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    // For 20 pixels height
    //g.fill3DRect( x+1, y+1, w-2, h-2, true);
    // For 18 pixels height
    g.draw3DRect( x+1, y+1, w-3, h-3, true);

    if (! itsSelected)
      g.setColor(Color.white);
    else
      g.setColor(itsLangNormalColor);

    int whiteXOffset = getWhiteXOffset();
    int whiteYOffset = getWhiteYOffset();
    g.fillRect( x + whiteXOffset, y + whiteYOffset, w - 2*whiteXOffset, h - 2*whiteYOffset);

    g.setColor(Color.black);
    g.drawRect( x, y, w-1, h-1);
    if (!itsSketchPad.itsRunMode)
      g.fillRect( x + w - DRAG_DIMENSION, y + h - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);

    g.setFont( getFont());
    DrawParsedString( g);
  }
}
