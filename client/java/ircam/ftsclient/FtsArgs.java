//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
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
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.ftsclient;

/**
 * A buffer of FtsAtom, i.e. int, float, strings and FtsObject
 *
 * This class does not use standard Collection implementations in order
 * to avoid allocations during message reception.
 *
 */

public class FtsArgs {
  private static final int INITIAL_SIZE = 8;

  public FtsArgs()
  {
    size = INITIAL_SIZE;
    array = new FtsAtom[size];
    for ( int i = 0; i < size; i++)
      array[i] = new FtsAtom();
    current = 0;
  }

  private void ensureCapacity( int wanted)
  {
    if ( current + wanted < size)
      return;

    while (size <= current + wanted)
      size *= 2;

    FtsAtom[] newArray = new FtsAtom[size];

    System.arraycopy( array, 0, newArray, 0, current);

    array = newArray;
  }

  public void add( int i)
  {
    ensureCapacity(1);
    array[current++].set( i);
  }

  public void add( float f)
  {
    ensureCapacity(1);
    array[current++].set( f);
  }

  public void add( FtsSymbol s)
  {
    ensureCapacity(1);
    array[current++].set( s);
  }

  public void add( String s)
  {
    ensureCapacity(1);
    array[current++].set( s);
  }

  public void add( FtsObject o)
  {
    ensureCapacity(1);
    array[current++].set( o);
  }

    public void add(Object value)
    {
	if(value instanceof Integer)
	    add(((Integer)value).intValue());
	else
	    if(value instanceof Float)
		add(((Float)value).floatValue());
	    else 
		if(value instanceof String)
		    add((String)value);
		else
		    if(value instanceof FtsObject)
			add((FtsObject)value);
		    else
			if(value instanceof FtsSymbol)
			    add((FtsSymbol)value);
    }

  public void clear()
  {
    current = 0;
  }

  public FtsAtom[] getAtoms()
  {
    return array;
  }

  public int getLength()
  {
    return current;
  }

  private FtsAtom[] array;
  private int size;
  private int current;
}

