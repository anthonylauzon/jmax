package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;

//
// The "patcher" graphic object.
//

class ErmesObjPatcher extends ErmesObjEditableObject
{
  // ----------------------------------------
  // Constructor
  // ----------------------------------------

  ErmesObjPatcher( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super( theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    // get the correct String from the object
    return itsFtsObject.getDescription().trim();
  }

  public void redefine( String text) 
  {
    ( (FtsPatcherObject)itsFtsObject).redefinePatcher( text);
    
    // adjust the size
    updateDimensions();
  }
	

  public void editContent()
  {
    itsSketchPad.waiting();
    Fts.editPropertyValue(itsFtsObject,
			  new MaxDataEditorReadyListener() {
      public void editorReady(MaxDataEditor editor)
	{itsSketchPad.stopWaiting();}
    });
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  private static final int TEXT_X_OFFSET = 4;
  private static final int TEXT_Y_OFFSET = 3;
  
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
    return 7;
  }

  public int getTextHeightOffset()
  {
    return 7;
  }

  public Color getTextBackground()
  {
    if (isSelected()) 
      return Settings.sharedInstance().getSelectedColor();
    else 
      return Settings.sharedInstance().getObjColor();
  }

  // ----------------------------------------
  // Inspector
  // ----------------------------------------

  public void inspect() 
  {
  }

  //--------------------------------------------------------
  // Paint stuff
  //--------------------------------------------------------

  public void paint( Graphics g) 
  {
    if (isSelected()) 
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else 
      g.setColor( Settings.sharedInstance().getObjColor());

    g.fill3DRect( getX() + 1, getY() + 1, getWidth() - 2, getHeight() - 2, true);
    g.draw3DRect( getX() + 2, getY() + 2, getWidth() - 5, getHeight() - 5, false);
    
    DrawParsedString( g);

    super.paint( g);
  }
}
