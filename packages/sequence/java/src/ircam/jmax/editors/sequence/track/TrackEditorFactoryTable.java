
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;
import ircam.jmax.toolkit.*;
/**
 * A class used to register new TrackEditorFactories */

public class TrackEditorFactoryTable {

    public static void setFactoryFor(ValueInfo info, TrackEditorFactory factory)
    {
	if(!infos.contains(info))
	    {
		factories.put(info.getName(), factory);
		infos.addElement(info);
	    }
    }

    public static TrackEditor newEditor(Track track, Geometry geometry)
    {
	TrackEditorFactory tef = (TrackEditorFactory) factories.get(track.getTrackDataModel().getType().getName());
	return tef.newEditor(track, geometry);
    }

    public static Enumeration getTypes()
    {
	return infos.elements();
    }
    //---
    private static Hashtable factories = new Hashtable();
    private static Vector infos = new Vector();
}





