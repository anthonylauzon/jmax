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
    
  /*public FtsObject createInstance(Fts fts, FtsObject parent, String className, int nArgs, FtsAtom args[])
    {
    FtsSequenceObject sequence = (FtsSequenceObject)args[0].getObject();
    String trackName = args[1].getString();
    double time = (double)args[2].getFloat();
    String valueType = args[3].getString();
    
    for(int i = 0; i<nArgs-4; i++)
    evtArgs[i] = args[4+i].getValue();	

    EventValue evtValue = (EventValue)(ValueInfoTable.getValueInfo(valueType).newInstance());
    evtValue.setPropertyValues(nArgs-4, evtArgs);

    return new TrackEvent(fts, sequence, trackName, time, evtValue);
    }*/
  
  public FtsObject createInstance(Fts fts, FtsObject parent, String className, int nArgs, FtsAtom args[])
  {
    double time = (double)args[0].getFloat();
    String valueType = args[1].getString();
    
    for(int i = 0; i<nArgs-2; i++)
      evtArgs[i] = args[2+i].getValue();	

    EventValue evtValue = (EventValue)(ValueInfoTable.getValueInfo(valueType).newInstance());
    evtValue.setPropertyValues(nArgs-2, evtArgs);
    
    return new TrackEvent(fts, time, evtValue);
  }
}

