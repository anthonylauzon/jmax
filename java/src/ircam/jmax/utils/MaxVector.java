//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 
package ircam.jmax.utils;

import java.util.*;

public class MaxVector
{
  protected Object objects[];
  protected int fillPointer; // point to the next not used element 

  public MaxVector()
  {
    super();
    this.objects = new Object[8];
  }

  public final void ensureCapacity(int requestedSize)
  {    
    if (requestedSize >= objects.length)
      {
	int newSize = objects.length;

	while (newSize < requestedSize)
	  newSize = ( 3 * newSize) / 2;

	Object oldObjects[] = objects;

	objects = new Object[newSize];
	System.arraycopy(oldObjects, 0, objects, 0, fillPointer);
      }
  }


  public final void setSize(int newSize)
  {
    if (newSize > objects.length) 
      ensureCapacity(newSize);
    else
      {
	for (int i = newSize ; i < fillPointer ; i++) 
	  objects[i] = null;
      }

    fillPointer = newSize;
  }

  public final int size() {
    return fillPointer;
  }

  public final boolean isEmpty() {
    return fillPointer == 0;
  }

  final class MaxVectorEnumerator implements Enumeration {
    int count;

    MaxVectorEnumerator()
    {
      count = 0;
    }

    public boolean hasMoreElements()
    {
      return count < fillPointer;
    }

    public Object nextElement()
    {
      if (count < fillPointer) 
	return objects[count++];

      throw new NoSuchElementException("VectorEnumerator");
    }
  }

  // Safe access

  public final  Enumeration elements() {
    return new MaxVectorEnumerator();
  }

  // Unsafe but fast access

  public final Object[] getObjectArray()
  {
    return objects;
  }
    
  public final boolean contains(Object elem)
  {
    if (elem == null) //look for a null element:
      {
	for (int i = 0 ; i < fillPointer ; i++)
	  if (objects[i] == null)
	    return true;
      }
    else 
      {
	for (int i = 0 ; i < fillPointer ; i++)
	  if (elem.equals(objects[i]))
	    return true;
      }
    return false;
  }

  public final Object elementAt(int index)
  {
    if (index >= fillPointer)
      {
	System.err.println("MaxVector: index " + index + " out of bounds ");
	Thread.dumpStack();
      }

    return objects[index];
  }

  public final void setElementAt(Object obj, int index)
  {
    objects[index] = obj;
  }

  public final void removeElementAt(int index)
  {
    int l = fillPointer - index - 1;

    if (l > 0)
      System.arraycopy(objects, index + 1, objects, index, l);

    fillPointer--;
    objects[fillPointer] = null; /* to let gc do its work */
  }

  public final void addElement(Object obj)
  {
    ensureCapacity(fillPointer + 1);
    objects[fillPointer++] = obj;
  }

  public final void insertElementAt(Object obj, int index)
  {
    if (index < fillPointer + 1)
      {
	ensureCapacity(fillPointer + 1);
	System.arraycopy(objects, index, objects, index + 1, fillPointer - index);
	objects[index] = obj;
	fillPointer++;
      }
  }


  public final boolean removeElement(Object obj)
  {
    if (obj == null) return false;
    for (int i = 0 ; i < fillPointer ; i++)
      if (obj.equals(objects[i]))
	{
	  removeElementAt(i);
	  return true;
	}

    return false;
  }

  public final void removeAllElements()
  {
    for (int i = 0; i < fillPointer; i++) 
      objects[i] = null;

    fillPointer = 0;
  }

  public final String toString()
  {
    int max = size() - 1;
    StringBuffer buf = new StringBuffer();
    buf.append("MaxVector[" + size() + "]{");

    for (int i = 0 ; i < fillPointer ; i++)
      {
	if (objects[i] != null)
	  buf.append(objects[i].toString());
	else
	  buf.append("(null)");

	if (i < max) 
	  buf.append(", ");
      }

    buf.append("}");

    return buf.toString();
  }
}


