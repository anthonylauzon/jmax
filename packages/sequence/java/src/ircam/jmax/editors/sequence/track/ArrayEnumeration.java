
package ircam.jmax.editors.sequence.track;

import java.util.*;

/**
 * An utility class to return the elements of an array of Objects as an Enumeration */
public class ArrayEnumeration implements Enumeration{
    
    public ArrayEnumeration( Object elements[])
    {
	this.elements = elements;
	index = 0;
    }

    public boolean hasMoreElements()
    {
	return (index < elements.length);
    }

    public Object nextElement()
    {
	if (!hasMoreElements()) return null; //do not have other elements!
	
	index++;
	return elements[index-1];
    }

    //---
    Object elements[];
    int index;
}
