package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** 
  The Master interaction for the edit mode.
  Mostly, each method install a slave interaction,
  preallocated in the constructor, and dispatch the event
  to it.
  */


class EditModeInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setFollowingLocations(true);
    filter.setFollowingInOutletLocations(true);
  }

  Interaction delegateSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
      	// Move

	return  Interactions.followInteraction;
      }
    else
      {
	editor.resetMessage();

	switch (squeack)
	  {
	  case (Squeack.POP_UP | Squeack.BACKGROUND):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.backgroundPopUpInteraction;

	  case (Squeack.DOWN | Squeack.BACKGROUND):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.dragSelectInteraction;

	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.TEXT):
	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.BACKGROUND):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.dragToggleSelectInteraction;

	  case (Squeack.DOWN | Squeack.OBJECT):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.moveInteraction;

	  case (Squeack.POP_UP | Squeack.OBJECT):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.popUpInteraction;

	  case (Squeack.DOWN | Squeack.TEXT):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.moveEditInteraction;

	  case (Squeack.DOWN | Squeack.CONNECTION):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.connectionSelectInteraction;

	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.CONNECTION):
	    // Drag Select
	    editor.resetHighlighted();
	    return Interactions.connectionToggleSelectInteraction;

	  case (Squeack.DOWN  | Squeack.OUTLET):
	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.OUTLET):
	    // Drag connect
	    return Interactions.dragConnectInteraction;

	  case (Squeack.DOWN  | Squeack.INLET):
	  case (Squeack.SHIFT | Squeack.DOWN | Squeack.INLET):
	    // Drag connect
	    return Interactions.dragReverseConnectInteraction;

	  case (Squeack.CTRL | Squeack.DOWN | Squeack.OBJECT):
	  case (Squeack.CTRL | Squeack.DOWN | Squeack.TEXT):
	    // Ctrl drag for controllers
	    editor.resetHighlighted();
	    return Interactions.editCtrlInteraction;

	  case (Squeack.DOWN | Squeack.HRESIZE_HANDLE):
	    // Horizontal Resize
	    editor.resetHighlighted();
	    return Interactions.hResizeInteraction;

	  case (Squeack.DOWN | Squeack.VRESIZE_HANDLE):
	    // Vertical Resize
	    editor.resetHighlighted();
	    return Interactions.vResizeInteraction;

	  case (Squeack.ALT | Squeack.DOWN | Squeack.TEXT):
	  case (Squeack.ALT | Squeack.DOWN | Squeack.OBJECT):
	    // Help
	    editor.resetHighlighted();
	    return Interactions.helpInteraction;

	  case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.TEXT):
	  case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.OBJECT):
	    // Edit Content
	    editor.resetHighlighted();
	    return Interactions.doubleClickEditInteraction;

	  default :
	    return null;
	  }
      }
  }
}




