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

#include <fts/ftsclient.h>

using namespace ircam::fts::client;

#define DEFAULT_INITIAL_CAPACITY 1031

namespace ircam {
namespace fts {
namespace client {

  class SymbolCache {

  public:
    SymbolCache( int initialCapacity = DEFAULT_INITIAL_CAPACITY)
    {
      _length = initialCapacity;
      _cache = new const FtsSymbol * [initialCapacity];

      for ( int i = 0; i < _length; i++)
	_cache[i] = 0;
    }

    int index( const FtsSymbol *s)
    {
      return (int)s % _length;
    }

    void put( int index, const FtsSymbol *s)
    {
      if ( index >= _length)
	{
	  const FtsSymbol **newCache = new const FtsSymbol* [index+1];

	  for ( int i = 0; i < _length; i++)
	    newCache[i] = _cache[i];
	  for ( int i = _length; i < index+1; i++)
	    newCache[i] = 0;

	  _cache = newCache;
	}

      _cache[index] = s;
    }

    const FtsSymbol *get( int index)
    {
      return _cache[index];
    }

  private:
    const FtsSymbol **_cache;
    int _length;
  };

};
};
};



