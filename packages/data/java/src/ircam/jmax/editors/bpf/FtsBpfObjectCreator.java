package ircam.jmax.editors.bpf;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import java.util.*;

/**
 * A class that handles the scroll and zoom values for a group of listeners.
 */
public class FtsBpfObjectCreator implements FtsObjectCreator
{
  public FtsObject createInstance(Fts fts, FtsObject parent, String variable, String className, int nArgs, FtsAtom args[])
  {
    return new FtsBpfObject(fts, parent, variable, className);
  }
}
