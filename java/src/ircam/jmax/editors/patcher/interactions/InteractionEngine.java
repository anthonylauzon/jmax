 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;


/** This class implement the interaction engine of the patcher editor.
  The interaction engine is a simplified version of the jMax toolkit
  interaction engine; we should move to the toolkit ASAP.

  The engine is composed by:
  
  <ul>
  <li> An input filter, essentially the lexical parser, that translate mouse
       events in higher level lexical events; lexical events are implemented as
       simple method call to the interaction; the input filter can change configuration,
       depending on the interaction; the current interaction can access the filter
       thru this class; see the InputFilter and Interaction class for more information about events.
  <li> Interactions; a set of class, one for each specific interaction (so that new
       interaction can be added by adding the class and branching it in the correct
       place in the interaction graph); the interactions are instantiated by need,
       and not reused, and have access to the Engine and thru this to the SketchPad/displayList
       to do semantic actions.
  </ul>

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

final public class InteractionEngine implements MouseMotionListener, MouseListener
{
  // The input filter can be set in different status;
  // they regard the different granularity input events
  // are analized.

  ErmesSketchPad sketch;
  DisplayList    displayList;

  public InteractionEngine(ErmesSketchPad sketch)
  {
    this.sketch = sketch;
    sketch.addMouseListener(this);

    displayList = sketch.getDisplayList();
    initAutoScroll();
  }
  
  public void dispose()
  {
    if (followingMoves)
      sketch.removeMouseMotionListener(this); 

    sketch.removeMouseListener(this);
  }

  // Control properties

  private boolean followingLocations = false;
  private boolean followingInOutletLocations = false;
  private boolean followingMoves     = false;
  private boolean followingInletLocations = false;
  private boolean followingOutletLocations = false;

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
    GraphicObject.setFollowingLocations(v); // Hack ? Should go thru displayList ?
  }
  final boolean isFollowingLocations()
  {
    return followingLocations;
  }
  final void setFollowingInOutletLocations(boolean v)
  {
    followingInOutletLocations = v;
    GraphicObject.setFollowingInOutletLocations(v);  // Hack ? Should go thru displayList ?
  }
  final boolean isFollowingInOutletLocations()
  {
    return followingInOutletLocations;
  }
  final void setFollowingInletLocations(boolean v)
  {
    followingInletLocations = v;
    GraphicObject.setFollowingInletLocations(v);  // Hack ? Should go thru displayList ?
  }
  final boolean isFollowingInletLocations()
  {
    return followingInletLocations;
  }
  final void setFollowingOutletLocations(boolean v)
  {
    followingOutletLocations = v;
    GraphicObject.setFollowingOutletLocations(v);  // Hack ? Should go thru displayList ?
  }
  final boolean isFollowingOutletLocations()
  {
    return followingOutletLocations;
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


  final private int getLocationBits(SensibilityArea area)
  {
    if (area == null)
      return Squeack.BACKGROUND;
    else
      return area.getSqueack();
  }


  final private void processEvent(int squeack, MouseEvent e)
  {
    ErmesSketchPad editor = (ErmesSketchPad) e.getSource();
    SensibilityArea area = null;

    editor.cleanAnnotations();

    updateMouseHistory(e);
    
    squeack |= getModifiersBits(e);

    if (followingLocations || followingInOutletLocations)
      {
	area = displayList.getSensibilityAreaAt(mouse.x, mouse.y);

	squeack |= getLocationBits(area);
      }

    autoScrollIfNeeded(editor, squeack, mouse, oldMouse);

    if (! scrollTimer.isRunning())
      sendSqueack(editor, squeack, area, mouse, oldMouse);

    if (area != null)
      area.dispose();
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
    scrollTimer = new Timer(8, scroller);
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
    ErmesSketchPad editor;
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

	  sendSqueack(editor, squeack, null, timerMouse, oldTimerMouse);

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

    void setEditor(ErmesSketchPad editor)
    {
      this.editor = editor;
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

  void autoScrollIfNeeded(ErmesSketchPad editor, int squeack, Point mouse, Point oldMouse)
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
	    scroller.setEditor(editor);
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

  // Handle the interaction stack

  final static int INTERACTION_STACK_DEPTH = 8;
  Interaction[]  interactionStack = new Interaction[INTERACTION_STACK_DEPTH];
  private int tos = -1; //point to the last used element of the stack

  final public void pushInteraction(Interaction interaction)
  {
    setFollowingMoves(false);
    setFollowingLocations(false);
    setFollowingInOutletLocations(false);
    setFollowingInletLocations(false);
    setFollowingOutletLocations(false);
    setAutoScrolling(false);

    interaction.configureInputFilter(this);
    interaction.reset();

    tos++;
    interactionStack[tos] = interaction;
  }

  // Substitute the top of the stack with a new interaction 
  
  final public void setInteraction(Interaction interaction)
  {
    setFollowingMoves(false);
    setFollowingLocations(false);
    setFollowingInOutletLocations(false);
    setFollowingInletLocations(false);
    setFollowingOutletLocations(false);
    setAutoScrolling(false);

    interaction.configureInputFilter(this);
    interaction.reset();

    interactionStack[tos] = interaction;
  }

  final public Interaction getCurrentInteraction()
  {
    if (tos < 0)
      return null;
    else
      return interactionStack[tos];
  }

  final public void popInteraction()
  {
    tos--;
    interactionStack[tos].configureInputFilter(this);
    interactionStack[tos].reset();
  }


  // Reset the stack and set a top level interaction

  final public void setTopInteraction(Interaction interaction)
  {
    tos = -1;
    pushInteraction(interaction);
  }

  // Send a squeack to an interaction, but giving the chance to push an interaction before

  final void sendSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    Interaction delegate;

    delegate = getCurrentInteraction().delegateSqueack(editor, squeack, area, mouse, oldMouse);

    if (delegate != null)
      pushInteraction(delegate);

    getCurrentInteraction().gotSqueack(editor, squeack, area, mouse, oldMouse);
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
    sketch.setKeyEventClient(null);
    sketch.stopTextEditing();
    sketch.requestFocus();

    if (e.isPopupTrigger())
      processEvent(Squeack.POP_UP, e);
    else if (e.getClickCount() > 1)
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
    // sketch.requestFocus();
  } 

  public void mouseExited( MouseEvent e)
  {
    // sketch.stopTextEditing();
  }
}










