
package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.editors.sequence.*;
/**
 * The interface of the objects that can be values of events in a sequence.
 * The needed functionalities are generic property handling, without assumptions
 * on the actual properties that the implementations of this interface can handle.
 * For example, a LogicValue can handle expressions, AmbitusValue can handle ambitus,
 * and so on. The method getPropertyNames() returns an enumeration of the names of the 
 * properties known by this EventValue 
 * @see ircam.jmax.editors.sequence.track.Event
 * @see ircam.jmax.editors.sequence.track.TrackEvent 
 */
public interface EventValue
{
   /**
     * Set the named property */
    public abstract void setProperty(String name, Object value);
    /**
     * Get the given property */
    public abstract Object getProperty(String name);

    /**
     * Get the given property */
    public abstract void edit(int x, int y, int modifiers, Event evt, SequenceGraphicContext gc);

    /**
     * Get the popup for this EventValue */
    public abstract JPopupMenu getPopupMenu();

    /** 
     * Returns the ValueInfo object for this EventValue */
    public abstract ValueInfo getValueInfo();

    /**
     * Returns the ObjectRenderer for this object */
    public abstract ObjectRenderer getRenderer();

    /**
     * Returns an Enumeration of all the property names known by this
     * EventValue */
    public abstract Enumeration getPropertyNames();
    
    public abstract String[] getLocalPropertyNames();

    public abstract int getPropertyType(int index);

    /**
     * Returns the number of properties known by this
     * EventValue */
    public abstract int getPropertyCount();

    public abstract int getLocalPropertyCount();

    /**
     * Returns all the property values known by this
     * EventValue like a FtsAtom array */
    public abstract Object[] getPropertyValues();
    
    public abstract Object[] getLocalPropertyValues();

    public abstract boolean samePropertyValues(Object args[]);
   
    /**
     * Returns all the property values known by this
     * EventValue like a FtsAtom array */
    public abstract void setPropertyValues(int nArgs, Object args[]);

    public abstract void setLocalPropertyValues(int nArgs, Object args[]);
    
    /**
     * getProperty on an unknown property should return this value */
    public static Object UNKNOWN_PROPERTY = new Integer(Integer.MAX_VALUE);
 
    /**
     * The defaut value of a property, when it is not known.
     * This is a situation in which we have no choices: the value
     * here is suited to be represented in Renderer that handle (at least) midi-like
     * values */
    public Object DEFAULT_PROPERTY = new Integer(64);

    public static int UNKNOWN_TYPE = -1;
    public static int INTEGER_TYPE =  0;
    public static int FLOAT_TYPE   =  1;
    public static int STRING_TYPE  =  2;
    public static int BOOLEAN_TYPE =  3;
    public static int DOUBLE_TYPE  =  4;
}










