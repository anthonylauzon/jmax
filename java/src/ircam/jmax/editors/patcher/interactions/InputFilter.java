package ircam.jmax.editors.patcher.interactions;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/**
  The input filter; translate input mouse events in lexical events
  for the Interaction engine; it also provide a couple of input tracking
  services, like keeping the position of the last mouse event, and the
  previous mouse event position; they are passed to the event.
  The lexical analysis made is not modal (state less).

  There are events, modifiers and locations;
  the event is a combination of bit patterns, defined in the Squeack
  class; the squeack is passed to the interaction thru a single method call
  together with the destination object, if any, and with the current and previous
  mouse point.

  Then, there are two properties that control filtering: followingMoves
  and followingLocations; if followingMoves is false, no move or drag event are
  generated; if followingLocation is false, no squeack with locations is
  generated; the two flags should be installed by the interactions
  when set in the interaction engine.

  */

final class InputFilter implements MouseMotionListener, MouseListener
{
  // The input filter can be set in different status;
  // they regard the different granularity input events
  // are analized.

  InteractionEngine engine;
  ErmesSketchPad sketch;
  DisplayList    displayList;

  InputFilter(InteractionEngine engine)
  {
    this.engine = engine;

    sketch = engine.getSketch();
    displayList = sketch.getDisplayList();
  }
  
  void dispose()
  {
    if (followingMoves)
      sketch.removeMouseMotionListener(this); 

    sketch.removeMouseListener(this);
  }

  // Control properties

  private boolean followingLocations = false;
  private boolean followingMoves     = false;

  final void setFollowingMoves(boolean v)
  {
    if (followingMoves)
      {
	if (! v)
	  sketch.removeMouseMotionListener(this); 
      }
    else
      {
	if (v)
	  sketch.addMouseMotionListener(this); 
      }

    followingMoves = v;
  }

  final boolean isFollowingMoves()
  {
    return followingMoves;
  }

  final void setFollowingLocations(boolean v)
  {
    followingLocations = v;
  }

  final boolean isFollowingLocations()
  {
    return followingLocations;
  }

  // Utilities

  private Point mouse = new Point();
  private Point oldMouse = new Point();

  final private void updateMouseHistory(MouseEvent e)
  {
    oldMouse.setLocation(mouse);
    mouse.setLocation(e.getX(), e.getY());
  }

  final private int getModifiersBits(MouseEvent e)
  {
    int ret = 0;

    if (e.isShiftDown())
      ret |= Squeack.SHIFT;

    if (e.isControlDown())
      ret |= Squeack.CTRL;

    if (e.isAltDown())
      ret |= Squeack.ALT;

    return ret;
  }


  final private int getLocationBits(DisplayObject object)
  {
    if (object == null)
      return Squeack.BACKGROUND;
    else if (object instanceof ErmesObject)
      return Squeack.OBJECT;
    else if (object instanceof ErmesConnection)
      return Squeack.CONNECTION;
    else if (object instanceof HResizeSensibilityArea)
      return Squeack.HRESIZE_HANDLE;
    else if (object instanceof VResizeSensibilityArea)
      return Squeack.VRESIZE_HANDLE;
    else if (object instanceof OutletSensibilityArea)
      return Squeack.OUTLET;
    else if (object instanceof InletSensibilityArea)
      return Squeack.INLET;
    else
      return Squeack.UNKNOWN;
  }

  final private void processEvent(int squeack, MouseEvent e)
  {
    DisplayObject object = null;

    updateMouseHistory(e);
    
    squeack |= getModifiersBits(e);

    if (followingLocations)
      {
	object = displayList.getDisplayObjectAt(mouse.x, mouse.y);

	squeack |= getLocationBits(object);
      }

    engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
  }

  // Methods following the mouse

  public void mouseMoved( MouseEvent e)
  {
    processEvent(Squeack.MOVE, e);
  }

  public void mouseDragged( MouseEvent e)
  {
    processEvent(Squeack.DRAG, e);
  }

  public void mousePressed( MouseEvent e)
  {
    if (e.getClickCount() > 1)
      processEvent(Squeack.DOUBLE_CLICK, e);
    else
      processEvent(Squeack.DOWN, e);
  }

  public void mouseReleased( MouseEvent e)
  {
    processEvent(Squeack.UP, e);
  }

  // The following methods are not used by the interaction engine.

  public void mouseClicked( MouseEvent e)
  {
  }

  public void mouseEntered( MouseEvent e)
  {
  } 

  public void mouseExited( MouseEvent e)
  {
  }
}





