package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.editors.patcher.*;

//
// The generic "extern" object in ermes. (example: adc1~)
//
public class Standard extends Editable implements FtsObjectErrorListener
{
  boolean ignoreError = false;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  Standard( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------
  public String getArgs()
  {
    // get the correct String from the object
    return ftsObject.getDescription();
  }

  public void errorChanged(boolean value) 
  {
    ignoreError = false;
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
	ignoreError = false;
	ftsObject = ftsObject.getFts().redefineFtsObject( ftsObject, text);

	if (ftsObject.isError())
	  {
	    itsSketchPad.showMessage(ftsObject.getErrorDescription());
	  }
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
    ftsObject.getFts().editPropertyValue( ftsObject, new MaxDataEditorReadyListener() {
      public void editorReady(MaxDataEditor editor)
	{itsSketchPad.stopWaiting();}
    });
  }

  public boolean hasContent()
  {
    return true;
  }

  public void setIgnoreError(boolean v)
  {
    ignoreError = v;
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
    if ((! ignoreError) && ftsObject.isError())
      g.setColor( Color.red);
    else
      {
	if (isSelected())
	  g.setColor( Settings.sharedInstance().getSelectedColor());
	else
	  g.setColor( Settings.sharedInstance().getObjColor());
      } 

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);

    drawContent( g);

    super.paint( g);
  }
}



