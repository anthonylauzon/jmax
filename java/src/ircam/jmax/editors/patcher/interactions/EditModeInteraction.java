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
  }

  Interaction delegateSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
      	// Move
	return  Interactions.followInteraction;
      }
    else switch (squeack)
      {
      case (Squeack.DOWN | Squeack.BACKGROUND):
	// Drag Select
	return Interactions.dragSelectInteraction;

      case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
      case (Squeack.SHIFT | Squeack.DOWN | Squeack.BACKGROUND):
	// Drag Select
	return Interactions.dragToggleSelectInteraction;

      case (Squeack.DOWN | Squeack.OBJECT):
	// Drag Select
	return Interactions.moveInteraction;

      case (Squeack.DOWN | Squeack.TEXT):
	// Drag Select
	return Interactions.moveEditInteraction;

      case (Squeack.DOWN | Squeack.CONNECTION):
	// Drag Select
	return Interactions.connectionSelectInteraction;

      case (Squeack.SHIFT | Squeack.DOWN | Squeack.CONNECTION):
	// Drag Select
	return Interactions.connectionToggleSelectInteraction;

      case (Squeack.DOWN | Squeack.OUTLET):
	// Drag connect
	return Interactions.dragConnectInteraction;

      case (Squeack.CTRL | Squeack.DOWN | Squeack.OBJECT):
	// Ctrl drag for controllers
	return Interactions.editCtrlInteraction;

      case (Squeack.DOWN | Squeack.HRESIZE_HANDLE):
	// Horizontal Resize
	return Interactions.hResizeInteraction;

      case (Squeack.DOWN | Squeack.VRESIZE_HANDLE):
	// Vertical Resize
	return Interactions.vResizeInteraction;

      case (Squeack.ALT | Squeack.DOWN | Squeack.TEXT):
      case (Squeack.ALT | Squeack.DOWN | Squeack.OBJECT):
	// Help
	return Interactions.helpInteraction;

      case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.TEXT):
      case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.OBJECT):
	// Edit Content
	return Interactions.doubleClickEditInteraction;

      default :
	return null;
      }
  }
}




