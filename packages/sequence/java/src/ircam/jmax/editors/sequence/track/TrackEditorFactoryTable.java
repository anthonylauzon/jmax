
package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;

/**
 * A class used to register new TrackEditorFactories */

public class TrackEditorFactoryTable {

    public static void setFactoryFor(ValueInfo info, TrackEditorFactory factory)
    {
	factories.put(info, factory);
    }


    public static TrackEditor newEditor(Track track, Geometry geometry)
    {
	TrackEditorFactory tef[] = new TrackEditorFactory[track.getTrackDataModel().getNumTypes()];

	int i = 0;
	for (Enumeration e=track.getTrackDataModel().getTypes(); e.hasMoreElements();)
	    {
		tef[i++] = (TrackEditorFactory) factories.get( e.nextElement());
	    }

	bubbleSort(tef);

	return tef[0].newEditor(track, geometry);
    }

    public static Enumeration getTypes()
    {
	return factories.keys();
    }

    static void bubbleSort(TrackEditorFactory tef[])
    {
	boolean flag = true;
	TrackEditorFactory temp;

	while(flag) {
	    flag = false;
	    for (int i = 0; i<tef.length-1; i++)
		if (tef[i].getWeight() < tef[i+1].getWeight())
		    {
			temp = tef[i];
			tef[i] = tef[i+1];
			tef[i+1] = temp;
			flag = true;
		    }
	} 
    }

    //---
    private static Hashtable factories = new Hashtable();
}
