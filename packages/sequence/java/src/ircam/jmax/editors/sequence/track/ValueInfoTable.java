
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;

/**
 * A class used to register ValueInfo and acces it by name*/

public class ValueInfoTable {

    public static void registerInfo(ValueInfo info)
    {
	infos.put(info.getName(), info);
    }

    public static ValueInfo getValueInfo(String type)
    {
	return (ValueInfo)(infos.get(type));
    }

    public static Enumeration getTypeNames()
    {
	return infos.keys();
    }

    //---
    private static Hashtable infos = new Hashtable();
}
