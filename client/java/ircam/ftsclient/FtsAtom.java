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
 * A FtsAtom is a typed 'union', holding the values transmitted other the client protocol
 */

public class FtsAtom {
  /**
   * If the atom is a integer, holds the integer value
   */
  public int intValue;
  /**
   * If the atom is a float, holds the float value
   */
  public float floatValue;
  /**
   * If the atom is a Symbol, holds the Symbol value
   */
  public FtsSymbol symbolValue;
  /**
   * If the atom is a String, holds the String value
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
   * Tests if atom contains a float
   * 
   * @return true if atom type is float
   */
  public final boolean isFloat()
  {
    return type == Float.TYPE;
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
   * Tests if atom contains a FtsObject
   * 
   * @return true if atom type is FtsObject
   */
  public final boolean isObject()
  {
    return type == FtsObject.class;
  }

  /**
   * Set the integer value
   * 
   * @param i the value
   */
  public final void set( int i)
  {
    type = Integer.TYPE;
    intValue = i;
  }

  /**
   * Set the float value
   * 
   * @param f the value
   */
  public final void set( float f)
  {
    type = Float.TYPE;
    floatValue = f;
  }

  /**
   * Set the FtsSymbol value
   * 
   * @param s the value
   */
  public final void set( FtsSymbol s)
  {
    type = FtsSymbol.class;
    symbolValue = s;
  }

  /**
   * Set the String value
   * 
   * @param s the value
   */
  public final void set( String s)
  {
    type = String.class;
    stringValue = s;
  }

  /**
   * Set the FtsObject value
   * 
   * @param o the value
   */
  public final void set( FtsObject o)
  {
    type = FtsObject.class;
    objectValue = o;
  }

  public final Object getValue()
  {
      if(isVoid())
	  return null;
      else if(isInt())
	  return new Integer( intValue);
      else if(isFloat())
	  return new Float( floatValue);
      else if(isString())
	  return stringValue;
      else if(isSymbol())
	  return symbolValue;
      else if(isObject())
	  return objectValue;

      return null;
  }

  private Class type;
}
