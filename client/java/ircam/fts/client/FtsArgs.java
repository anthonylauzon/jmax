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

package ircam.fts.client;

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

    for ( int i = current; i < size; i++)
      newArray[i] = new FtsAtom();

    array = newArray;
  }

  public final void addBoolean( boolean value)
  {
    addInt( value ? 1 : 0);
  }

  public final void addBoolean( Boolean value)
  {
    addInt( value.booleanValue() ? 1 : 0);
  }

  public final void addInt( int i)
  {
    ensureCapacity(1);
    array[current++].setInt( i);
  }

  public final void addInt( Integer value)
  {
    addInt( value.intValue());
  }

  public final void addFloat( float f)
  {
    ensureCapacity(1);
    array[current++].setFloat( f);
  }

  public final void addFloat( Float value)
  {
    addFloat( value.floatValue());
  }

  public final void addSymbol( FtsSymbol s)
  {
    ensureCapacity(1);
    array[current++].setSymbol( s);
  }

  public final void addString( String s)
  {
    ensureCapacity(1);
    array[current++].setString( s);
  }

  public final void addObject( FtsObject o)
  {
    ensureCapacity(1);
    array[current++].setObject( o);
  }

  public final void add( Object value)
  {
    if ( value instanceof Integer)
      addInt( ((Integer)value).intValue());
    else if ( value instanceof Float)
      addFloat( ((Float)value).floatValue());
    else if ( value instanceof FtsSymbol)
      addSymbol((FtsSymbol)value);
    else if ( value instanceof String)
      addString( (String)value);
    else if ( value instanceof FtsObject)
      addObject( (FtsObject)value);
  }



  public final boolean isInt( int index)
  {
    return array[index].isInt();
  }

  public final boolean isFloat( int index)
  {
    return array[index].isFloat();
  }

  public final boolean isSymbol( int index)
  {
    return array[index].isSymbol();
  }

  public final boolean isString( int index)
  {
    return array[index].isString();
  }

  public final boolean isObject( int index)
  {
    return array[index].isObject();
  }



  public final int getInt( int index)
  {
    return array[index].intValue;
  }

  public final float getFloat( int index)
  {
    return array[index].floatValue;
  }

  public final FtsSymbol getSymbol( int index)
  {
    return array[index].symbolValue;
  }

  public final String getString( int index)
  {
    return array[index].stringValue;
  }

  public final FtsObject getObject( int index)
  {
    return array[index].objectValue;
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

