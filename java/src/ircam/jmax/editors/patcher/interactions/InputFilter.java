package ircam.jmax.editors.patcher.interactions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

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
    initAutoScroll();
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

    autoScrollIfNeeded(squeack, mouse, oldMouse);

    if (! scrollTimer.isRunning())
      engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
  }

  // Scroll handling
  // The input filter handle automatic smooth scrolling
  // the autoresize is handled by the sketch and by the
  // semantic action directly.
    
  Timer scrollTimer;
  ScrollDragAction scroller;
  boolean autoScroll = false;
  final private static int scrollMargin = 5;

  private void initAutoScroll()
  {
    scroller    = new ScrollDragAction();
    scrollTimer = new Timer(10, scroller);
    scrollTimer.setCoalesce(true);
    scrollTimer.setRepeats(true);
  }

  void setAutoScrolling(boolean v)
  {
    autoScroll = v;
  }

  boolean isAutoScrolling()
  {
    return autoScroll;
  }

  class ScrollDragAction implements ActionListener
  {
    int squeack;
    Point timerMouse = new Point();
    Point oldTimerMouse = new Point();
    Point direction  = new Point();

    public void actionPerformed(ActionEvent evt)
    {
      // Three case: inside the margin, going outside
      // outside the window, inside the margin
      // The third is not handled because this method
      // is never called in this case.

      if (! sketch.pointIsVisible(timerMouse, scrollMargin))
	{
	  // The point is in the margin (visible in the window,
	  // not visible if we take out the margin

	  engine.getInteraction().gotSqueack(squeack, null, timerMouse, oldTimerMouse);

	  sketch.whereItIs(timerMouse, direction, scrollMargin);

	  sketch.scrollBy(direction.x * 3, direction.y * 3);

	  addPoint(timerMouse);
	  timerMouse.x = timerMouse.x + direction.x * 3;
	  timerMouse.y = timerMouse.y + direction.y * 3;
	}
    }


    void setSqueack(int squeack)
    {
      this.squeack = squeack;
    }

    void addPoint(Point mouse)
    {
      oldTimerMouse.setLocation(timerMouse);
      timerMouse.setLocation(mouse);
    }

    void addPoint(int x, int y)
    {
      oldTimerMouse.setLocation(timerMouse);
      timerMouse.setLocation(x, y);
    }
  }

  void autoScrollIfNeeded(int squeack, Point mouse, Point oldMouse)
  {
    // Handle the auto scrolling and autoresizing

    if (isAutoScrolling() && Squeack.isDrag(squeack) &&
	(! sketch.pointIsVisible(mouse, scrollMargin)))
      {
	if (scrollTimer.isRunning())
	  {
	    // Ignore
	  }
	else
	  {
	    scroller.setSqueack(squeack);
	    scroller.addPoint(oldMouse);
	    scroller.addPoint(mouse);
	    scrollTimer.start();
	  }
      }
    else 
      {
	if (scrollTimer.isRunning())
	  {
	    scrollTimer.stop();
	  }
      }
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






