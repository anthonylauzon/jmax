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

/**
 * The union contained in a FtsValue
 */

namespace ircam {
namespace fts {
namespace client {

  // Forward declarations
  class FtsObject;

  /**
   * A FtsAtom is a typed 'union', holding the values transmitted other the client protocol
   */

  class FTSCLIENT_API FtsAtom {
    friend class FtsArgs;
    friend std::ostream &operator<<( std::ostream &os, const FtsAtom &a);
  public:
    /**
     * Tests if atom contains a 'void' value (i.e. no valid value)
     * 
     * @return true if atom type is void
     */
    int isVoid() const
    {
      return _type == VOID;
    }

    /**
     * Tests if atom contains an integer
     * 
     * @return true if atom type is integer
     */
    int isInt() const
    {
      return _type == INT;
    }

    /**
     * Tests if atom contains a double
     * 
     * @return true if atom type is double
     */
    int isDouble() const
    {
      return _type == DOUBLE;
    }

    /**
     * Tests if atom contains a "symbol"
     * 
     * @return true if atom type is "symbol"
     */
    int isSymbol() const
    {
      return _type == SYMBOL;
    }

    /**
     * Tests if atom contains a String
     * 
     * @return true if atom type is String
     */
    int isString() const
    {
      return _type == STRING;
    }

    /**
     * Tests if atom contains a raw String
     * 
     * @return true if atom type is RawString
     */
    int isRawString() const
    {
      return _type == RAW_STRING;
    }

    /**
     * Tests if atom contains a FtsObject or an instance of a derived class
     * 
     * @return true if atom type is FtsObject or a derived class of FtsObject
     */
    int isObject() const
    {
      return _type == OBJECT; 
    }

    /**
     * Set the atom as void
     * 
     */
    void setVoid()
    {
      _type = VOID;
    }

    /**
     * Set the integer value
     * 
     * @param i the value
     */
    void setInt( int i)
    {
      _type = INT;
      value.intValue = i;
    }

    /**
     * Set the double value
     * 
     * @param f the value
     */
    void setDouble( double f)
    {
      _type = DOUBLE;
      value.doubleValue = f;
    }

    /**
     * Set the "symbol" value
     * 
     * @param s the value
     */
    void setSymbol( const char *s)
    {
      _type = SYMBOL;
      value.symbolValue = s;
    }

    /**
     * Set the String value
     * 
     * @param s the value
     */
    void setString( const char *s)
    {
      _type = STRING;
      value.stringValue = s;
    }

    /**
     * Set the raw String value
     * 
     * @param s the value
     */
    void setRawString( const char *s)
    {
      _type = RAW_STRING;
      value.stringValue = s;
    }

    /**
     * Set the FtsObject value
     * 
     * @param o the value
     */
    void setObject( FtsObject *o)
    {
      _type = OBJECT;
      value.objectValue = o;
    }

  private:

    union {
      int intValue;
      double doubleValue;
      const char *symbolValue;
      const char *stringValue;
      FtsObject *objectValue;
    } value;

    static const int VOID        = 1;
    static const int INT         = 2;
    static const int DOUBLE      = 3;
    static const int SYMBOL      = 4;
    static const int STRING      = 5;
    static const int RAW_STRING  = 6;
    static const int OBJECT      = 7;

    int _type;
  };

};
};
};

std::ostream &operator<<( std::ostream &os, const ircam::fts::client::FtsAtom &a);

