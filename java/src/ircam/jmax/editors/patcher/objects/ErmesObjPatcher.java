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
    
    // (em) set the text and adjust the size
    setText( getArgs());
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
  // White area offset
  // ----------------------------------------
  private static final int WHITE_X_OFFSET = 4;
  private static final int WHITE_Y_OFFSET = 4;

  public final int getWhiteXOffset()
  {
    return WHITE_X_OFFSET;
  }

  public final int getWhiteYOffset()
  {
    return WHITE_Y_OFFSET;
  }

  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
  private static final int TEXT_X_OFFSET = 4;
  private static final int TEXT_Y_OFFSET = 2;

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
  }

  //--------------------------------------------------------
  // Paint stuff
  //--------------------------------------------------------
  public void Paint( Graphics g) 
  {
    if( !isSelected()) 
      g.setColor( Settings.sharedInstance().getObjColor());
    else 
      g.setColor( Settings.sharedInstance().getSelectedColor());

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x+1, y+1, w-2, h-2, true);
    g.draw3DRect( x+2, y+2, w-5, h-5, false);
    
    g.setColor( Color.black);
    g.setFont( getFont());
    DrawParsedString( g);

    super.Paint( g);
  }
}
