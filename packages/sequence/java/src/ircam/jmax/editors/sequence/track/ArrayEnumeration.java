//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

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
