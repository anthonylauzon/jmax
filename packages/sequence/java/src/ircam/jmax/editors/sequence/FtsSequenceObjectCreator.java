package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import java.util.*;

/**
 * A class that handles the scroll and zoom values for a group of listeners.
 */
public class FtsSequenceObjectCreator implements FtsObjectCreator
{
  public FtsObject createInstance(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
    return new FtsSequenceObject(fts, parent, variable, className);
  }
}
