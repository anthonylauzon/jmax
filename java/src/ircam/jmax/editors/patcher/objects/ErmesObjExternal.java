package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.editors.patcher.*;

//
// The generic "extern" object in ermes. (example: adc1~)
//
public class ErmesObjExternal extends ErmesObjEditableObject implements FtsObjectErrorListener
{
  private int isError = -1; // cache of the error property, to speed up paint

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  ErmesObjExternal( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  public String getArgs()
  {
    // get the correct String from the object
    return itsFtsObject.getDescription();
  }

  public void errorChanged(boolean value) 
  {
    // Handle the "error" property, the only one we're listening at.

    if (value)
      isError = 1;
    else
      isError = 0;

    redraw();
  }

  /* Inspector */

  public void inspect() 
  {
  }

  public void redefine( String text) 
  {
    try 
      {
	itsFtsObject = Fts.redefineFtsObject( itsFtsObject, text);
	isError = -1;
      } 
    catch (FtsException e) 
      {
	System.out.println("Error in redefining object, action cancelled");
      }

    super.redefine(text);
  }


  public void editContent()
  {
    itsSketchPad.waiting();
    Fts.editPropertyValue( itsFtsObject, new MaxDataEditorReadyListener() {
      public void editorReady(MaxDataEditor editor)
	{itsSketchPad.stopWaiting();}
    });
  }



  // ----------------------------------------
  // Text area offset
  // ----------------------------------------

  private static final int TEXT_X_OFFSET = 4;
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
    return 5;
  }

  public Color getTextBackground()
  {
    if (isSelected())
      return Settings.sharedInstance().getSelectedColor();
    else
      return Color.white;
  }

  // ----------------------------------------
  // Paint stuff
  // ----------------------------------------

  public void paint(Graphics g) 
  {
    if (isError == -1)
      {
	if (itsFtsObject.isError())
	  isError = 1;
	else
	  isError = 0;
      }

    if (isError == 0) 
      {
	if (isSelected())
	  g.setColor( Settings.sharedInstance().getSelectedColor());
	else
	  g.setColor( Settings.sharedInstance().getObjColor());
      } 
    else
      g.setColor( Color.red);

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);

    drawContent( g);

    super.paint( g);
  }
}



