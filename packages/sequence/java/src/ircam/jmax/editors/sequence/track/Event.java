
package ircam.jmax.editors.sequence.track;


import ircam.jmax.fts.*;
import ircam.jmax.editors.sequence.*;

/**
 * The interface of the objects that can be put in a sequence. They have a starting time   
 * and a value that can be of different types - ex. Ambitus notes, silences, fricatives, etc... 
 * The value field is an FtsRemoteData. 
 * The setProperty and getProperty methods are used to assign parameters without knowing 
 * the type of FtsRemoteData this event contains.
 * The same structure is reflected on the server side. */
public interface Event 
{
    /**
     * Get the initial time for this event */
    public abstract double getTime();

    /**
     * This is the method that must be called by the editors to
     * change the initial time of an event. It takes care of
     * keeping the data base consistency */
    public abstract void move(double time);
    /**
     * Set the time for this event */
    public abstract void setTime(double time);

    /**
     * Set the named property */
    public abstract void setProperty(String name, Object value);

    /**
     * Get the given property */
    public abstract Object getProperty(String name);

    /**
     * Returns the value of this event as an FtsRemoteData */
    public abstract EventValue getValue();

    /** Set the FtsData corresponding to this event */
    public abstract void setValue(EventValue value);

    final public static int SELECTED    = 0;
    final public static int DESELECTED  = 1;
    final public static int HIGHLIGHTED = 2;
}





