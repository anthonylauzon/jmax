/*
 * FTS client library
 * Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
 * The union contained in a FtsValue
 */

#include <string.h>

// Forward declarations
class FtsObject;

union FTSCLIENT_API FtsValueUnion {
  friend class FtsValue;
  friend class FtsArgs;

private:
  int _i;
  float _f;
  char *_s;
  FtsObject *_o;
};

/**
 * The value contained in a message.
 */
class FTSCLIENT_API FtsValue {
  friend class FtsArgs;

public:
  FtsValue() : _type( FtsValue::EMPTY) {}

private:
  static const int EMPTY;
  static const int INT;
  static const int FLOAT;
  static const int STRING;
  static const int OBJECT;

  void set( int i) { 
    _type = FtsValue::INT; 
    _value._i = i; 
  }

  void set( float f) { 
    _type = FtsValue::FLOAT; 
    _value._f = f; 
  }

  void set( const char *s) { 
    _type = FtsValue::STRING; 
    _value._s = strcpy( new char[strlen(s)+1], s); 
  }

  void set( FtsObject *o) { 
    _type = FtsValue::OBJECT; 
    _value._o = o; 
  }

  void unset() { 
    if (_type == FtsValue::STRING) 
      delete [] _value._s; 
  }

  int _type;
  FtsValueUnion _value;
};

/**
 * The arguments of a message.<BR>
 * Used for object creation, message sending and as argument of callbacks.
 */
class FTSCLIENT_API FtsArgs {

public:
  // Empty constructor
  FtsArgs() {}

  // Copy constructor
  FtsArgs( const FtsArgs &args);

  ~FtsArgs() { 
    clear(); 
  }

  /**
   * add an argument of specified type at end of arguments list
   */
  void add( int i) { _buffer->set( i); _buffer++; }
  void add( float f) { _buffer->set( f); _buffer++; }
  void add( const char *s) { _buffer->set( s); _buffer++; }
  void add( FtsObject *o) { _buffer->set( o); _buffer++; }

  /**
   * tests argument type
   */
  int isInt( int index) { return _buffer[index]._type == FtsValue::INT; }
  int isFloat( int index) { return _buffer[index]._type == FtsValue::FLOAT; }
  int isString( int index) { return _buffer[index]._type == FtsValue::STRING; }
  int isObject( int index) { return _buffer[index]._type == FtsValue::OBJECT; }

  /**
   * gets an argument by index
   */
  int getInt( int index) { return _buffer[index]._value._i; }
  float getFloat( int index) { return _buffer[index]._value._f; }
  const char *getString( int index) { return _buffer[index]._value._s; }
  FtsObject *getObject( int index) { return _buffer[index]._value._o; }

  /** 
   * resets the arguments counter
   */
  void clear();

  /**
   * returns the length of arguments list
   */
  int length() { return _buffer.length(); }

  /**
   * prints content
   */
  ostream &print( ostream &os);

private:
  FtsBuffer<FtsValue> _buffer;
};

ostream &operator<<( ostream &os, FtsArgs &args);

