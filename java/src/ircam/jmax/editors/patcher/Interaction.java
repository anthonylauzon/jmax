package ircam.jmax.editors.patcher;

import java.awt.*; 
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;

import javax.swing.*; 

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;

/** 
  Obsolete stuff; still here only for the keylistener, and key client
  and should go way very very soon ...
  */

class Interaction  implements KeyListener
{
  ErmesSketchPad sketch;
  DisplayList    displayList;

  KeyEventClient keyEventClient = null;

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Constructors                                             //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  Interaction(ErmesSketchPad sketch, DisplayList displayList)
  {
    this.sketch      = sketch;
    this.displayList = displayList;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //                                                                            //
  //                   Keyboard Handling                                        //
  //                                                                            //
  ////////////////////////////////////////////////////////////////////////////////

  // KeyClient Handling

  public void setKeyEventClient( KeyEventClient keyEventClient)
  {
    if ( this.keyEventClient != null && this.keyEventClient != keyEventClient)
      this.keyEventClient.keyInputLost();

    this.keyEventClient = keyEventClient;

    if (this.keyEventClient != null)
      this.keyEventClient.keyInputGained();
  }

  // Modified to use inheritance and call the ErmesEditor method
  // for all the standard key bindings

  public void keyPressed( KeyEvent e)
  {
    int aInt = e.getKeyCode();

    sketch.cleanAnnotations(); // MDC

    if ( keyEventClient != null)
      {
	keyEventClient.keyPressed( e);
      }
    else if ( (aInt == KeyEvent.VK_DELETE) ||
	      (aInt== KeyEvent.VK_BACK_SPACE) )
      {
	/* This code should move to an Action !!
	   It currently cannot because the keyEventClient
	   want to pre-empt DELETE ... (should do something;
	   also, the keyEventClient should disappear, and just be
	   a keyListener sullo sketch
	   */
	   
	ErmesSelection selection = ErmesSelection.patcherSelection;

	if (selection.ownedBy(sketch))
	  if (! sketch.getEditField().HasFocus())
	    selection.deleteAll();
      }
  }

  public void keyTyped(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyTyped( e);
  }

  public void keyReleased(KeyEvent e)
  {
    if ( keyEventClient != null)
      keyEventClient.keyReleased( e);
  }
}







