package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import java.util.*;

/**
 * A class that create a FtsTrackObject.
 */
public class FtsTrackObjectCreator implements FtsObjectCreator
{ 
  static Object[] evtArgs = new Object[128];
    
    public FtsObject createInstance(Fts fts, FtsObject parent, String className, int nArgs, FtsAtom args[])
    {
	String name = args[0].getString();
	String valueType = args[1].getString();
	
	return new FtsTrackObject(fts, name, ValueInfoTable.getValueInfo(valueType));
    }
}




