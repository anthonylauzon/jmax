package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;

//
// The "comment" graphic object
//

public class ErmesObjComment extends ErmesObjEditableObject
{
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

  public String getArgs()
  {
    // get the correct String from the object

    return ftsObject.getComment();
  }

  public void redefine( String text) 
  {
    ((FtsCommentObject)ftsObject).setComment( text);

    super.redefine(text);
  }

  protected void computeRenderer()
  {
    String args = getArgs();
    Icon icon = null;

    // Change the renderer if needed

    if ((args != null) && (args.length() > 0)  && args.charAt(0) == '%')
      icon = Icons.get(args);

    if (icon != null)
      renderer = new IconRenderer(this, icon);
    else if (! (renderer instanceof TextRenderer))
      renderer = new TextRenderer(this);
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  private static final int TEXT_X_OFFSET = 2;
  private static final int TEXT_Y_OFFSET = 2;

  public int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  public int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }

  public int getTextWidthOffset()
  {
    return 6;
  }

  public int getTextHeightOffset()
  {
    return 4;
  }

  public Color getTextBackground()
  {
    if (itsSketchPad.isLocked())
      return Color.white;
    else
      {
	if (isSelected())
	  return Color.gray;
	else
	  return itsSketchPad.getBackground();
      }
  }

  public void paint(Graphics g) 
  {
    if ( !itsSketchPad.isLocked())
      {
	if (isSelected())
	  g.setColor(Color.gray);
	else
	  g.setColor(itsSketchPad.getBackground());

	g.fill3DRect( getX(), getY(), getWidth(), getHeight(), true);
      }

    //text
   drawContent( g);
  }

  //--------------------------------------------------------
  // minimumSize()
  //--------------------------------------------------------
  public Dimension getMinimumSize() 
  {
    return null;
  }
}
