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
    
    public FtsObject createInstance(Fts fts, FtsObject parent, String variableName, String className, int nArgs, FtsAtom args[])
    {
	String name;
	String valueType = args[0].getString();
	if(nArgs>1)
	    name = args[1].getString();
	else
	    name = "untitled";

	return new FtsTrackObject(fts, name, ValueInfoTable.getValueInfo(valueType));
    }
}




