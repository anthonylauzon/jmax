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

class ErmesObjMessage extends ErmesObjEditableObject implements FtsMessageListener {

  boolean itsFlashing = false;

  public ErmesObjMessage(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  public String getArgs()
  {
    // get the correct String from the object's "Message" property, that may change

    return ((FtsMessageObject)itsFtsObject).getMessage();
  }

  // ----------------------------------------
  // White area offset
  // ----------------------------------------
//   private static final int WHITE_X_OFFSET = 3;
//   private static final int WHITE_Y_OFFSET = 3;
  private static final int WHITE_X_OFFSET = 4;
  private static final int WHITE_Y_OFFSET = 2;

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
//   private static final int TEXT_X_OFFSET = 4;
//   private static final int TEXT_Y_OFFSET = 3;
  private static final int TEXT_X_OFFSET = 3;
  private static final int TEXT_Y_OFFSET = 2;

  protected final int getTextXOffset()
  {
    return TEXT_X_OFFSET;
  }

  protected final int getTextYOffset()
  {
    return TEXT_Y_OFFSET;
  }


  public void redefine( String text) 
  {
    ((FtsMessageObject)itsFtsObject).setMessage( text);

    // (em) set the text and adjust the size
    setText( text);
  }

  // Set the text when FTS change the message content

  public void messageChanged(String message)
  {
    // (fd) To be redone
    // Should be a nice repaint ??
    // (em) set the text and adjust the size

    setText( message);

    Graphics g = itsSketchPad.getGraphics();
    Paint(g);
    g.dispose();
  }


  public void gotSqueack(int squeack, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case Squeack.DOWN:
	itsFtsObject.sendMessage( 0, "bang", null);
	itsFlashing = true;
	redraw();
	break;
      case Squeack.UP:
	if (itsFlashing) 
	  {
	    itsFlashing = false;

	    redraw();
	  }
	break;
      }
  }

  public void Paint(Graphics g) 
  {
    if ( !itsSketchPad.isLocked()) 
      {
	if( itsFlashing) 
	  g.setColor( Settings.sharedInstance().getSelectedColor());
	else
	  {
	    if( !isSelected()) 
	      g.setColor( Color.white);
	    //g.setColor( Settings.sharedInstance().getUIColor());
	    else
	      g.setColor( Settings.sharedInstance().getObjColor());
	  }
      }
    else 
      {
	if ( itsFlashing || isSelected())
	  g.setColor( Settings.sharedInstance().getSelectedColor());
	else 
	  //g.setColor( Settings.sharedInstance().getUIColor());
	  g.setColor( Color.white);
      }
  
    g.fillRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2);
    //g.fill3DRect( getX()+1, getY()+1, getWidth()-2, getHeight()-2, true);
    
    g.setColor( Color.black);
    g.setFont( getFont());
    DrawParsedString( g);

    super.Paint( g);
  }
}
