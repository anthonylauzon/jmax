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

/* 
 * Utilility template classes used by the FTS client library
 */

#include <string.h>

/**
 * A template buffer
 */

#define FTSBUFFER_INITIAL_SIZE  8

template <class T> class FTSCLIENT_API FtsBuffer {
public:
  FtsBuffer() { 
    _size = FTSBUFFER_INITIAL_SIZE; 
    _buffer = new T[_size]; 
    _current = 0; 
  }

  ~FtsBuffer() { 
    delete [] _buffer; 
  }

  FtsBuffer &operator++() { 
    ensureCapacity(); 
    _current++; 
    return *this; 
  }

  FtsBuffer &operator++( int unused) { 
    ensureCapacity(); 
    _current++; 
    return *this; 
  }

  T& operator[]( int index) { 
    return _buffer[index]; 
  }

  T &operator*() { 
    return _buffer[_current]; 
  }

  T* operator->() { 
    return _buffer + _current; 
  }

  operator const T*() { 
    return _buffer; 
  }

  void clear() { 
    _current = 0; 
  }

  int length() { 
    return _current; 
  }

private:
  void ensureCapacity( int wanted = 1);

  int _size;
  int _current;
  T *_buffer;
};

template <class T> void FtsBuffer<T>::ensureCapacity( int wanted)
{
  if ( _current + wanted < _size)
    return;

  while (_size <= _current + wanted)
    _size *= 2;

  T *newBuffer = new T [_size];

  for ( int i = 0; i <= _current; i++)
    newBuffer[i] = _buffer[i];

  delete [] _buffer;
  _buffer = newBuffer;
}

/**
 * A template hash table
 */

extern unsigned int getNextPrime( unsigned int n);

#define FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY 101
#define FTSHASHTABLE_STANDARD_LOAD_FACTOR 0.75

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTable;

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTableCell {
friend class FtsHashTable< KeyT, ValT>;
private:
  FtsHashTableCell( KeyT k, ValT v) : _key(k), _value(v), _next(0) {}
  KeyT _key;
  ValT _value;
  FtsHashTableCell *_next;
};

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTable {
public:
  FtsHashTable( int initialCapacity = FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY);

  ~FtsHashTable() { 
    clear(); 
    delete [] _table; 
  }

  void clear();

  int put( KeyT key, ValT value);
  int get( KeyT key, ValT &value);
  int remove( KeyT key);

  void stats( ostream &os);

protected:

  unsigned int hash( const char *s);

  unsigned int hash( int k) { 
    return (unsigned int)k; 
  }

  unsigned int hash( void *p) { 
    return (unsigned int)p>>3; 
  }

  int equals( int k1, int k2) { 
    return k1 == k2; 
  }

  int equals( const char *s1, const char *s2) { 
    return strcmp(s1, s2) == 0; 
  }

  int equals( void *p1, void *p2) { 
    return p1 == p2; 
  }

  void rehash();

private:

  FtsHashTableCell< KeyT, ValT> **lookupCell( const KeyT &key);
  int getInitialCapacity( int initialCapacity);

  int _length;
  int _count;
  int _rehashCount;
  FtsHashTableCell< KeyT, ValT> **_table;
};

template <class KeyT, class ValT> 
FtsHashTable< KeyT, ValT>::FtsHashTable( int initialCapacity)
{
  _length = getNextPrime( initialCapacity);
  _count = 0;
  _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);
  _table = new FtsHashTableCell< KeyT, ValT> * [_length];

  for ( int i = 0; i < _length; i++)
    _table[i] = 0;
}

template <class KeyT, class ValT> 
void FtsHashTable< KeyT, ValT>::clear()
{
  for ( int i = 0; i < _length; i++)
    {
      FtsHashTableCell< KeyT, ValT> *p, *next;

      for ( p = _table[i]; p; p = next)
	{
	  next = p->_next;
	  delete p;
	}

      _table[i] = 0;
    }

  _count = 0;
}

template <class KeyT, class ValT> 
unsigned int FtsHashTable< KeyT, ValT>::hash( const char *s)
{
  unsigned int h = *s, i = 0;

  for ( s += 1; *s != '\0'; s++)
    h = (h<<1) + *s;

  return h;
}

template <class KeyT, class ValT> 
FtsHashTableCell<KeyT, ValT> **FtsHashTable< KeyT, ValT>::lookupCell( const KeyT &key)
{
  FtsHashTableCell< KeyT, ValT> **c;

  c = &_table[ hash( key) % _length];

  while (*c && !equals( (*c)->_key, key))
    c = &(*c)->_next;

  return c;
}

template <class KeyT, class ValT> 
int FtsHashTable< KeyT, ValT>::get( KeyT key, ValT &value)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      value = (*c)->_value;
      return 1;
    }

  return 0;
}


template <class KeyT, class ValT> 
void FtsHashTable< KeyT, ValT>::rehash()
{
  unsigned int oldLength, i;
  FtsHashTableCell< KeyT, ValT> **oldTable;

  oldLength = _length;
  _length = getNextPrime( _length);
  _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);

  oldTable = _table;
  _table = new FtsHashTableCell< KeyT, ValT> * [_length];

  for ( i = 0; i < _length; i++)
    _table[i] = 0;

  for ( i = 0; i < oldLength; i++)
    {
      FtsHashTableCell< KeyT, ValT> *c, *next;

      for ( c = oldTable[i]; c; c = next)
	{
	  int index = hash( c->_key) % _length;

	  next = c->_next;
	  c->_next = _table[index];
	  _table[index] = c;
	}
    }

  delete [] oldTable;
}


template <class KeyT, class ValT>
int FtsHashTable<KeyT,ValT>::put( KeyT key, ValT value)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      (*c)->_value = value;
      return 1;
    }

  *c = new FtsHashTableCell< KeyT, ValT>( key, value);

  _count++;
  if ( _count >= _rehashCount)
    rehash();

  return 0;
}

template <class KeyT, class ValT>
int FtsHashTable< KeyT, ValT>::remove( KeyT key)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      FtsHashTableCell< KeyT, ValT> *toDelete = *c;

      *c = (*c)->_next;
      delete toDelete;
      _count--;
      
      return 1;
    }

  return 0;
}

template <class KeyT, class ValT>
void FtsHashTable< KeyT, ValT>::stats( ostream &os)
{
  int minKeys = 38928392, maxKeys = 0, i;

  os << "Hashtable: length = " << _length << " count = " << _count << " rehash = " << _rehashCount << endl;

  for ( i = 0; i < _length; i++)
    {
      unsigned int nKeys = 0;
      FtsHashTableCell< KeyT, ValT> *c;

      for ( c = _table[i]; c; c = c->_next)
	nKeys++;

      if (nKeys < minKeys)
	minKeys = nKeys;
      if (nKeys > maxKeys)
	maxKeys = nKeys;

      os << "[" << i << "] " << nKeys;

      for ( c = _table[i]; c; c = c->_next)
	{
	  os << " " << c->_key;
	}

      os << endl;
    }

  os << "minimum " << minKeys << " maximum " << maxKeys << endl;
}
