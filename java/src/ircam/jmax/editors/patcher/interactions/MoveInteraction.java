package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling Moves cursor changes; 
  Note that each interaction should set its initial cursor.
  */


class MoveInteraction extends SubInteraction
{
  MoveInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }

  void configureInputFilter(InputFilter filter)
  {
    // Note that following locations is false because
    // the first event is not handled by this filter,
    // but by the master interaction filter.

    filter.setFollowingMoves(true); // need the move
    filter.setFollowingLocations(false);
  }


  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.MOVE | Squeack.BACKGROUND))
      engine.getSketch().setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.OBJECT))
      engine.getSketch().setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.CONNECTION))
      engine.getSketch().setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.HRESIZE_HANDLE))
      engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
    if (squeack == (Squeack.MOVE | Squeack.VRESIZE_HANDLE))
      engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
    if (squeack == (Squeack.MOVE | Squeack.INLET))
      engine.getSketch().setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.OUTLET))
      engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
    
    end();
  }
}




