package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import java.util.*;

/**
 * A class that handles the scroll and zoom values for a group of listeners.
 */
public class FtsEventObjectCreator implements FtsObjectCreator
{ 
    static Object[] evtArgs = new Object[128];
    public FtsObject createInstance(Fts fts, FtsObject parent, String variableName, String className, int nArgs, FtsAtom args[])
    {
	double time = (double)args[0].getFloat();
	String valueType = args[1].getString();

	for(int i = 0; i< nArgs-2; i++)
	    {
		Object obj = args[2+i].getValue();
		if(obj instanceof Float) //obj = new Double((double)((Float)obj).floatValue());
		  obj = new Double(((Float)obj).doubleValue());
		evtArgs[i] = obj;	  
	    }

	EventValue evtValue = (EventValue)(ValueInfoTable.getValueInfo(valueType).newInstance());
	evtValue.setPropertyValues(nArgs-2, evtArgs);

	return new TrackEvent(fts, time, evtValue);
    }
}




