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
 * A FtsAtom is a typed 'union', holding the values transmitted other the client protocol
 */

public class FtsAtom {
  /**
   * If the atom is a integer, holds the integer value
   */
  public int intValue;
  /**
   * If the atom is a double, holds the double value
   */
  public double doubleValue;
  /**
   * If the atom is a Symbol, holds the Symbol value
   */
  public FtsSymbol symbolValue;
  /**
   * If the atom is a String or a RawString, holds the String value
   */
  public String stringValue;
  /**
   * If the atom is a object, holds the object value
   */
  public FtsObject objectValue;

  /**
   * Tests if atom contains a 'void' value (i.e. no valid value)
   * 
   * @return true if atom type is void
   */
  public final boolean isVoid()
  {
    return type == Void.TYPE;
  }

  /**
   * Tests if atom contains an integer
   * 
   * @return true if atom type is integer
   */
  public final boolean isInt()
  {
    return type == Integer.TYPE;
  }

  /**
   * Tests if atom contains a double
   * 
   * @return true if atom type is double
   */
  public final boolean isDouble()
  {
    return type == Double.TYPE;
  }

  /**
   * Tests if atom contains a FtsSymbol
   * 
   * @return true if atom type is FtsSymbol
   */
  public final boolean isSymbol()
  {
    return type == FtsSymbol.class;
  }

  /**
   * Tests if atom contains a String
   * 
   * @return true if atom type is String
   */
  public final boolean isString()
  {
    return type == String.class;
  }

  /**
   * Tests if atom contains a raw String
   * 
   * @return true if atom type is RawString
   */
  public final boolean isRawString()
  {
    return type == RawString.class;
  }

  /**
   * Tests if atom contains a FtsObject or an instance of a derived class
   * 
   * @return true if atom type is FtsObject or a derived class of FtsObject
   */
  public final boolean isObject()
  {
    // return true if the class of the contained object is a derived class of FtsObject
    return FtsObject.class.isAssignableFrom( type); 
  }

  /**
   * Set the integer value
   * 
   * @param i the value
   */
  public final void setInt( int i)
  {
    type = Integer.TYPE;
    intValue = i;
  }

  /**
   * Set the double value
   * 
   * @param f the value
   */
  public final void setDouble( double f)
  {
    type = Double.TYPE;
    doubleValue = f;
  }

  /**
   * Set the FtsSymbol value
   * 
   * @param s the value
   */
  public final void setSymbol( FtsSymbol s)
  {
    type = FtsSymbol.class;
    symbolValue = s;
  }

  /**
   * Set the String value
   * 
   * @param s the value
   */
  public final void setString( String s)
  {
    type = String.class;
    stringValue = s;
  }

  /**
   * Set the raw String value
   * 
   * @param s the value
   */
  public final void setRawString( String s)
  {
    type = RawString.class;
    stringValue = s;
  }

  /**
   * Set the FtsObject value
   * 
   * @param o the value
   */
  public final void setObject( FtsObject o)
  {
    type = o.getClass();
    objectValue = o;
  }

  public final Object getValue()
  {
    if ( isVoid())
      return null;
    else if ( isInt())
      return new Integer( intValue);
    else if ( isDouble())
      return new Double( doubleValue);
    else if ( isString())
      return stringValue;
    else if ( isRawString())
      return stringValue;
    else if ( isSymbol())
      return symbolValue;
    else if ( isObject())
      return objectValue;

    return null;
  }

  public final void setValue( Object value)
  {
    if ( value instanceof Integer)
      setInt( ((Integer)value).intValue());
    else if ( value instanceof Double)
      setDouble( ((Double)value).doubleValue());
    else if ( value instanceof FtsSymbol)
      setSymbol((FtsSymbol)value);
    else if ( value instanceof String)
      setString( (String)value);
    else if ( value instanceof FtsObject)
      setObject( (FtsObject)value);
  }

  private Class type;
}
