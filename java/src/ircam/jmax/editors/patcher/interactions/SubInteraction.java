package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;

/**
  A convenience abstract super class for all the sub interactions.
  A sub interaction is an interaction started from a master interaction,
  that finally will reinstall it.
  Actually, this is like an interaction in the toolkit, while
  a master interaction integrate in some way the tool and the
  main interaction.

  The master interaction store the interaction, the engine
  store the master interactions.
  */


abstract class SubInteraction extends Interaction
{
  Interaction master;

  SubInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine);
    this.master = master;
  }

  protected void end()
  {
    engine.setInteraction(master);
  }
}



