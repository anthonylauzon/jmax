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


class IdleEditInteraction extends Interaction
{
  // Standard interaction in edit mode.
  // To add a new one, just

  Interaction dragSelectInteraction;
  Interaction dragToggleSelectInteraction;
  Interaction helpInteraction;
  Interaction editCtrlInteraction;
  Interaction hResizeInteraction;
  Interaction vResizeInteraction;
  Interaction moveEditInteraction;
  Interaction moveInteraction;
  Interaction dragConnectInteraction;
  Interaction followInteraction;
  Interaction connectionSelectInteraction;
  Interaction connectionToggleSelectInteraction;
  Interaction doubleClickEditInteraction;

  IdleEditInteraction(InteractionEngine engine)
  {
    super(engine);

    dragSelectInteraction = new DragSelectInteraction(engine, this);
    dragToggleSelectInteraction = new DragToggleSelectInteraction(engine, this);
    helpInteraction       = new HelpInteraction(engine, this);
    editCtrlInteraction   = new EditCtrlInteraction(engine, this);
    hResizeInteraction    = new HResizeInteraction(engine, this);
    vResizeInteraction    = new VResizeInteraction(engine, this);
    moveEditInteraction   = new MoveEditInteraction(engine, this);
    moveInteraction       = new MoveInteraction(engine, this);
    dragConnectInteraction = new DragConnectInteraction(engine, this);
    followInteraction       = new FollowInteraction(engine, this);
    connectionSelectInteraction = new ConnectionSelectInteraction(engine, this);
    connectionToggleSelectInteraction = new ConnectionToggleSelectInteraction(engine, this);
    doubleClickEditInteraction = new CtrlDoubleClickEditInteraction(engine, this);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingMoves(true);
    filter.setFollowingLocations(true);
  }

  void gotSqueack(int squeack, DisplayObject object, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
      	// Move
	engine.setInteraction(followInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
      }
    else switch (squeack)
      {
      case (Squeack.DOWN | Squeack.BACKGROUND):
	// Drag Select
	engine.setInteraction(dragSelectInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
      case (Squeack.SHIFT | Squeack.DOWN | Squeack.BACKGROUND):
	// Drag Select
	engine.setInteraction(dragToggleSelectInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.OBJECT):
	// Drag Select
	engine.setInteraction(moveInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.TEXT):
	// Drag Select
	engine.setInteraction(moveEditInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.CONNECTION):
	// Drag Select
	engine.setInteraction(connectionSelectInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.SHIFT | Squeack.DOWN | Squeack.CONNECTION):
	// Drag Select
	engine.setInteraction(connectionToggleSelectInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.OUTLET):
	engine.setInteraction(dragConnectInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.CTRL | Squeack.DOWN | Squeack.OBJECT):
	// Ctrl drag for controllers
	engine.setInteraction(editCtrlInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.HRESIZE_HANDLE):
	// Horizontal Resize 
	engine.setInteraction(hResizeInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.VRESIZE_HANDLE):
	// Vertical Resize 
	engine.setInteraction(vResizeInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.ALT | Squeack.DOWN | Squeack.OBJECT):
	// Help
	engine.setInteraction(helpInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.TEXT):
      case (Squeack.CTRL | Squeack.DOUBLE_CLICK | Squeack.OBJECT):
	// Edit Content 
	engine.setInteraction(doubleClickEditInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      }
  }
}




