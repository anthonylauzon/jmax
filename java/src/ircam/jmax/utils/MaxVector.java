package ircam.jmax.utils;

import java.util.*;

public class MaxVector
{
  protected Object objects[];
  protected int fillPointer;

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
	  newSize *= 2;

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
    for (int i = 0 ; i < fillPointer ; i++)
      if (elem.equals(objects[i]))
	return true;

    return false;
  }

  public final Object elementAt(int index)
  {
    return objects[index];
  }

  public final void setElementAt(Object obj, int index)
  {
    objects[index] = obj;
  }

  public final void removeElementAt(int index)
  {
    int j = fillPointer - index - 1;

    if (j > 0)
      {
	System.arraycopy(objects, index + 1, objects, index, j);
      }

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
    buf.append("[");

    for (int i = 0 ; i < fillPointer ; i++) {
      if (objects[i] != null)
	buf.append(objects[i].toString());
      else
	buf.append("(null)");

      if (i < max) {
	buf.append(", ");
      }
    }
    buf.append("]");
    return buf.toString();
  }
}


