package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling Moves cursor changes; 
  Note that each interaction should set its initial cursor.
  */


class FollowInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.MOVE | Squeack.BACKGROUND))
      editor.setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.OBJECT))
      editor.setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.CONNECTION))
      editor.setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.HRESIZE_HANDLE))
      editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
    if (squeack == (Squeack.MOVE | Squeack.VRESIZE_HANDLE))
      editor.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
    if (squeack == (Squeack.MOVE | Squeack.INLET))
      editor.setCursor(Cursor.getDefaultCursor());
    if (squeack == (Squeack.MOVE | Squeack.OUTLET))
      editor.setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
    if (squeack == (Squeack.MOVE | Squeack.TEXT))
      editor.setCursor(Cursor.getDefaultCursor());
    
    editor.endInteraction();
  }
}




