//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.fts.client;

class SymbolCache {
  private static final int DEFAULT_INITIAL_CAPACITY = 1031;

  SymbolCache( int initialCapacity)
  {
    cache = new FtsSymbol[initialCapacity];
  }

  SymbolCache()
  {
    this( DEFAULT_INITIAL_CAPACITY);
  }

  final int index( FtsSymbol v)
  {
    return v.hashCode() % cache.length;
  }

  final void put( int index, FtsSymbol s)
  {
    if ( index >= cache.length)
      {
	FtsSymbol[] newCache = new FtsSymbol[index+1];
	System.arraycopy( cache, 0, newCache, 0, cache.length);
	cache = newCache;
      }

    cache[index] = s;
  }

  final FtsSymbol get( int index)
  {
    return cache[index];
  }

  private FtsSymbol[] cache;
}

