package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.interactions.*;

//
// The "message box" graphic object.
//

class Message extends Editable implements FtsMessageListener {

  boolean itsFlashing = false;

  public Message(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    // get the correct String from the object's "Message" property, that may change

    return ((FtsMessageObject)ftsObject).getMessage();
  }


  // ----------------------------------------
  // Text area offset
  // ----------------------------------------
//   private static final int TEXT_X_OFFSET = 4;
//   private static final int TEXT_Y_OFFSET = 3;
  private static final int TEXT_X_OFFSET = 3;
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

  public void redefine( String text) 
  {
    ((FtsMessageObject)ftsObject).setMessage( text);

    super.redefine(text);
  }

  // Set the text when FTS change the message content

  public void messageChanged(String message)
  {
    updateRedraw();
  }


  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack))
      {
	ftsObject.sendMessage( 0, "bang", null);
	itsFlashing = true;
	redraw();
      }
    else if (Squeack.isUp(squeack))
      {
	if (itsFlashing) 
	  {
	    itsFlashing = false;

	    redraw();
	  }
      }
  }

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  public Color getTextBackground()
  {
    if (! itsSketchPad.isLocked()) 
      {
	if( itsFlashing) 
	  return Settings.sharedInstance().getSelectedColor();
	else
	  {
	    if (isSelected()) 
	      return Settings.sharedInstance().getSelectedColor();
	    else
	      return Color.white;
	  }
      }
    else 
      {
	if ( itsFlashing || isSelected())
	  return Settings.sharedInstance().getSelectedColor();
	else 
	  return Color.white;
      }
  }

  public void paint(Graphics g) 
  {
    g.setColor(getTextBackground());
    g.fillRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2);
    
    drawContent( g);

    super.paint( g);
  }
}
