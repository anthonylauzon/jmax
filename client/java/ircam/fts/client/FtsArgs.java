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
 * A buffer of FtsAtom, i.e. int, double, strings and FtsObject
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

  public final void addInt( int value)
  {
    ensureCapacity(1);
    array[current++].setInt( value);
  }

  public final void addInt( Integer value)
  {
    addInt( value.intValue());
  }

  public final void addDouble( double value)
  {
    ensureCapacity(1);
    array[current++].setDouble( value);
  }

  public final void addDouble( Double value)
  {
    addDouble( value.doubleValue());
  }

  /**
   * @deprecated replaced by addDouble( double value)
   */
  public final void addFloat( float value)
  {
    addDouble( (double)value);
  }

  /**
   * @deprecated replaced by addDouble( Double value)
   */
  public final void addFloat( Float value)
  {
    addDouble( (double)value.floatValue());
  }


  public final void addSymbol( FtsSymbol value)
  {
    ensureCapacity(1);
    array[current++].setSymbol( value);
  }

  public final void addString( String value)
  {
    ensureCapacity(1);
    array[current++].setString( value);
  }

  public final void addRawString( String value)
  {
    ensureCapacity(1);
    array[current++].setRawString( value);
  }

  public final void addObject( FtsObject value)
  {
    ensureCapacity(1);
    array[current++].setObject( value);
  }

  public final void add( Object value)
  {
    if ( value instanceof Integer)
      addInt( ((Integer)value).intValue());
    else if ( value instanceof Double)
      addDouble( ((Double)value).doubleValue());
    else if ( value instanceof Float)
      addDouble( (double)((Float)value).floatValue());
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

  public final boolean isDouble( int index)
  {
    return array[index].isDouble();
  }

  /**
   * @deprecated replaced by isDouble( int index)
   */
  public final boolean isFloat( int index)
  {
    return isDouble( index);
  }

  public final boolean isSymbol( int index)
  {
    return array[index].isSymbol();
  }

  public final boolean isString( int index)
  {
    return array[index].isString();
  }

  public final boolean isRawString( int index)
  {
    return array[index].isRawString();
  }

  public final boolean isObject( int index)
  {
    return array[index].isObject();
  }



  public final int getInt( int index)
  {
    return array[index].intValue;
  }

  public final double getDouble( int index)
  {
    return array[index].doubleValue;
  }

  /**
   * @deprecated replaced by getDouble( int index)
   */
  public final float getFloat( int index)
  {
    return (float)array[index].doubleValue;
  }

  public final FtsSymbol getSymbol( int index)
  {
    return array[index].symbolValue;
  }

  public final String getString( int index)
  {
    return array[index].stringValue;
  }

  public final String getRawString( int index)
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

  public String toString()
  {
    StringBuffer b = new StringBuffer();

    for ( int i = 0; i < current; i++)
      b.append( " [" + i + "] " + array[i] + "\n");

    return b.toString();
  }

  private FtsAtom[] array;
  private int size;
  private int current;
}

