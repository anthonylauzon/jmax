package ircam.jmax.editors.patcher.interactions;

import ircam.jmax.editors.patcher.*;




/** This class implement the interaction engine of the patcher editor.
  The interaction engine is a simplified version of the jMax toolkit
  interaction engine; we should move to the toolkit ASAP.

  The engine is composed by:
  
  <ul>
  <li> An instance of this class, that do central housekeeping and control.
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
  */

final public class InteractionEngine
{
  ErmesSketchPad sketch;
  DisplayList    displayList;
  Interaction    interaction;
  InputFilter    filter;
  boolean        locked;

  // The two master interaction we hande in the patcher editor

  Interaction runMaster  = new IdleRunInteraction(this);
  Interaction editMaster = new IdleEditInteraction(this);
  Interaction addMaster  = new AddObjectInteraction(this);

  public InteractionEngine(ErmesSketchPad sketch)
  {
    this.sketch = sketch;
    this.displayList = sketch.getDisplayList();
    filter = new InputFilter(this);

    if (sketch.isLocked())
      setRunMode();
    else
      setEditMode();
	
    sketch.addMouseListener(filter);
  }

  /* Destructor: needed to deinstall the listener */

  public void dispose()
  {
    filter.dispose();
  }


  public void setAddMode()
  {
    setInteraction(addMaster);
  }

  public void setRunMode()
  {
    setInteraction(runMaster);
  }

  public void setEditMode()
  {
    setInteraction(editMaster);
  }

  final void setInteraction(Interaction interaction)
  {
    this.interaction = interaction;

    interaction.configureInputFilter(filter);
    interaction.reset();
  }
     
  final Interaction getInteraction()
  {
    return interaction;
  }

  final ErmesSketchPad getSketch()
  {
    return sketch;
  }

  final DisplayList getDisplayList()
  {
    return displayList;
  }


}
