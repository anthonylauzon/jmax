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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.fts;

/**
 * Class wrapping the protocol values
 */

public class FtsAtom {
  public static final int VOID = 0;
  public static final int INT = 1;
  public static final int FLOAT = 2;
  public static final int STRING = 3;
  public static final int SYMBOL = 4;
  public static final int OBJECT = 5;
  
  public final boolean isVoid()
  {
    return type == VOID;
  }

  public final boolean isInt()
  {
    return type == INT;
  }

  public final boolean isFloat()
  {
    return type == FLOAT;
  }

  public final boolean isString()
  {
    return type == STRING;
  }

  public final boolean isSymbol()
  {
    return type == SYMBOL;
  }

  public final boolean isObject()
  {
    return type == OBJECT;
  }

  public final void setVoid()
  {
    type = VOID;
  }

  public final int getInt()
  {
    return intValue;
  }

  public final void setInt( int i)
  {
    type = INT;
    intValue = i;
  }

  public final float getFloat()
  {
    return floatValue;
  }

  public final void setFloat( float f)
  {
    type = FLOAT;
    floatValue = f;
  }

  public final String getString()
  {
    return stringValue;
  }

  public final void setString( String s)
  {
    type = STRING;
    stringValue = s;
  }

  public final FtsSymbol getSymbol()
  {
    return symbolValue;
  }

  public final void setSymbol( FtsSymbol s)
  {
    type = SYMBOL;
    symbolValue = s;
  }

  public final FtsObject getObject()
  {
    return objectValue;
  }

  public final void setObject( FtsObject o)
  {
    type = OBJECT;
    objectValue = o;
  }

  public final Object getValue()
  {
    switch( type) {
    case VOID:
      return null;
    case INT:
      return new Integer( intValue);
    case FLOAT:
      return new Float( floatValue);
    case STRING:
      return stringValue;
    case SYMBOL:
      return symbolValue;
    case OBJECT:
      return objectValue;
    }

    return null;
  }

  public int type;
  public int intValue;
  public float floatValue;
  public String stringValue;
  public FtsSymbol symbolValue;
  public FtsObject objectValue;
}
