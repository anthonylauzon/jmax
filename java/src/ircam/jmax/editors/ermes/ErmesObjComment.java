package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The "comment" graphic object
//
class ErmesObjComment extends ErmesObjEditableObject {

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjComment( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);
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
    ((FtsCommentObject)itsFtsObject).setComment( text);

    // (em) set the text and adjust the size
    setText( text);
  }

  // ----------------------------------------
  // White area offset
  // ----------------------------------------
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
  private static final int TEXT_X_OFFSET = 2;
  private static final int TEXT_Y_OFFSET = 2;

  protected final int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  protected final int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public void Paint_specific(Graphics g) 
  {
    if ( !itsSketchPad.isLocked())
      {
	if (itsSelected)
	  g.setColor(Color.gray);
	else
	  g.setColor(itsSketchPad.getBackground());

	g.fill3DRect( getX(), getY(), getWidth(), getHeight(), true);
      }

    //text
    g.setColor( Color.black);
    g.setFont( getFont());
    DrawParsedString( g);
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() 
  {
    return null;
  }
}
