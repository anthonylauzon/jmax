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
 * A buffer of FtsAtom, i.e. int, double, strings and FtsObject
 *
 */
namespace ircam {
namespace fts {
namespace client {

  class FTSCLIENT_API FtsArgs {
  public:
    FtsArgs();
    ~FtsArgs();

    void addInt( int value)
    {
      ensureCapacity(1);
      _array[_current++].setInt( value);
    }

    void addDouble( double value)
    {
      ensureCapacity(1);
      _array[_current++].setDouble( value);
    }

    void addSymbol( const char *value)
    {
      ensureCapacity(1);
      _array[_current++].setSymbol( value);
    }

    void addString( const char *value)
    {
      ensureCapacity(1);
      _array[_current++].setString( value);
    }

    void addRawString( const char *value)
    {
      ensureCapacity(1);
      _array[_current++].setRawString( value);
    }

    void addObject( FtsObject *value)
    {
      ensureCapacity(1);
      _array[_current++].setObject( value);
    }

    int isInt( int index) const
    {
      return _array[index].isInt();
    }

    int isDouble( int index) const
    {
      return _array[index].isDouble();
    }

    int isSymbol( int index) const
    {
      return _array[index].isSymbol();
    }

    int isString( int index) const
    {
      return _array[index].isString();
    }

    int isRawString( int index) const
    {
      return _array[index].isRawString();
    }

    int isObject( int index) const
    {
      return _array[index].isObject();
    }


    int getInt( int index) const
    {
      return _array[index].value.intValue;
    }

    double getDouble( int index) const
    {
      return _array[index].value.doubleValue;
    }

    const char *getSymbol( int index) const
    {
      return _array[index].value.stringValue;
    }

    const char *getString( int index) const
    {
      return _array[index].value.stringValue;
    }

    const char *getRawString( int index) const
    {
      return _array[index].value.stringValue;
    }

    FtsObject *getObject( int index) const
    {
      return _array[index].value.objectValue;
    }

    void clear()
    {
      _current = 0;
    }

    FtsAtom *getAtoms() const
    {
      return _array;
    }

    int getLength() const
    {
      return _current;
    }

  private:
    void ensureCapacity( int wanted);

    FtsAtom *_array;
    int _size;
    int _current;
  };

};
};
};

std::ostream &operator<<( std::ostream &os, const ircam::fts::client::FtsArgs &a);
