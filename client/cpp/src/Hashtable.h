/**
 * A template hash table
 */

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <iostream.h>

namespace ircam {
namespace fts {
namespace client {

  extern unsigned int getNextPrime( unsigned int n);

#define FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY 101
#define FTSHASHTABLE_STANDARD_LOAD_FACTOR 0.75

  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable;

  template <class KeyT, class ValT> class FTSCLIENT_API HashtableCell {
    
    friend class Hashtable< KeyT, ValT>;

  private:
    HashtableCell( KeyT k, ValT v) : _key(k), _value(v), _next(0) {}
    KeyT _key;
    ValT _value;
    HashtableCell *_next;

  };

  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable {
  public:
    Hashtable( int initialCapacity = FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY);

    ~Hashtable() { 
      clear(); 
      delete [] _table; 
    }

    void clear();

    int put( KeyT key, ValT value);
    int get( KeyT key, ValT &value);
    int remove( KeyT key);

    void stats( ostream &os);

    static unsigned int hash( const char *s);

    static unsigned int hash( int k)
      {
	return (unsigned int)k; 
      }

    static unsigned int hash( void *p)
      {
	return (unsigned int)p>>3; 
      }

    static int equals( int k1, int k2)
      {
	return k1 == k2; 
      }

    static int equals( const char *s1, const char *s2)
      {
	return std::strcmp(s1, s2) == 0; 
      }

    static int equals( void *p1, void *p2)
      {
	return p1 == p2; 
      }

  protected:

    void rehash();

  private:

    HashtableCell< KeyT, ValT> **lookupCell( const KeyT &key);
    int getInitialCapacity( int initialCapacity);

    unsigned int _length;
    unsigned int _count;
    unsigned int _rehashCount;
    HashtableCell< KeyT, ValT> **_table;
  };

  template <class KeyT, class ValT> 
    Hashtable< KeyT, ValT>::Hashtable( int initialCapacity)
    {
      _length = getNextPrime( initialCapacity);
      _count = 0;
      _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);
      _table = new HashtableCell< KeyT, ValT> * [_length];

      for ( unsigned int i = 0; i < _length; i++)
	_table[i] = 0;
    }

  template <class KeyT, class ValT> 
    void Hashtable< KeyT, ValT>::clear()
    {
      for ( unsigned int i = 0; i < _length; i++)
	{
	  HashtableCell< KeyT, ValT> *p, *next;

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
    unsigned int Hashtable< KeyT, ValT>::hash( const char *s)
    {
      unsigned int h = 0;

      while( *s)
	h = (h<<1) + *s++;

      return h;
    }

  template <class KeyT, class ValT> 
    HashtableCell<KeyT, ValT> **Hashtable< KeyT, ValT>::lookupCell( const KeyT &key)
    {
      HashtableCell< KeyT, ValT> **c;

      c = &_table[ Hashtable<KeyT, ValT>::hash( key) % _length];

      while (*c && !equals( (*c)->_key, key))
	c = &(*c)->_next;

      return c;
    }

  template <class KeyT, class ValT> 
    int Hashtable< KeyT, ValT>::get( KeyT key, ValT &value)
    {
      HashtableCell< KeyT, ValT> **c = lookupCell( key);

      if (*c)
	{
	  value = (*c)->_value;
	  return 1;
	}

      return 0;
    }


  template <class KeyT, class ValT> 
    void Hashtable< KeyT, ValT>::rehash()
    {
      unsigned int oldLength, i;
      HashtableCell< KeyT, ValT> **oldTable;

      oldLength = _length;
      _length = getNextPrime( _length);
      _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);

      oldTable = _table;
      _table = new HashtableCell< KeyT, ValT> * [_length];

      for ( i = 0; i < _length; i++)
	_table[i] = 0;

      for ( i = 0; i < oldLength; i++)
	{
	  HashtableCell< KeyT, ValT> *c, *next;

	  for ( c = oldTable[i]; c; c = next)
	    {
	      int index = Hashtable<KeyT, ValT>::hash( c->_key) % _length;

	      next = c->_next;
	      c->_next = _table[index];
	      _table[index] = c;
	    }
	}

      delete [] oldTable;
    }


  template <class KeyT, class ValT>
    int Hashtable<KeyT,ValT>::put( KeyT key, ValT value)
    {
      HashtableCell< KeyT, ValT> **c = lookupCell( key);

      if (*c)
	{
	  (*c)->_value = value;
	  return 1;
	}

      *c = new HashtableCell< KeyT, ValT>( key, value);

      _count++;
      if ( _count >= _rehashCount)
	rehash();

      return 0;
    }

  template <class KeyT, class ValT>
    int Hashtable< KeyT, ValT>::remove( KeyT key)
    {
      HashtableCell< KeyT, ValT> **c = lookupCell( key);

      if (*c)
	{
	  HashtableCell< KeyT, ValT> *toDelete = *c;

	  *c = (*c)->_next;
	  delete toDelete;
	  _count--;
      
	  return 1;
	}

      return 0;
    }

  template <class KeyT, class ValT>
    void Hashtable< KeyT, ValT>::stats( std::ostream &os)
    {
      int minKeys = 38928392, maxKeys = 0, i;

      os << "Hashtable: length = " << _length << " count = " << _count << " rehash = " << _rehashCount << endl;

      for ( i = 0; i < _length; i++)
	{
	  unsigned int nKeys = 0;
	  HashtableCell< KeyT, ValT> *c;

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

};
};
};

#endif
