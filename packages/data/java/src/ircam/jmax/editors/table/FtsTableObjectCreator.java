package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import java.util.*;

/**
 * A class that handles the scroll and zoom values for a group of listeners.
 */
public class FtsTableObjectCreator implements FtsObjectCreator
{
  public FtsObject createInstance(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
      return new FtsTableObject(fts, parent, variable, className, nArgs, args);
  }
}
