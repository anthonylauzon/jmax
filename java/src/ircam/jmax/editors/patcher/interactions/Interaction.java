package ircam.jmax.editors.patcher.interactions;

import java.awt.*;


import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** A convenience abstract super class for all the interactions.
  Provide empty definition for all the lexical event methods,
  and a convenience variable to store the InteractionEngine.
  */


abstract class Interaction
{
  // This method configure the input filter
  // for this interaction; is called by the interaction engine.
  // by default, deinstall the move and the location handling

  void configureInputFilter(InteractionEngine filter)
  {
  }

  // This method is also called by the interaction engine
  // to reset the state of the interaction; by default,
  // it do nothing

  void reset()
  {
  }

  // Called for every squeack

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
  }

  Interaction delegateSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    return null;
  }
}
