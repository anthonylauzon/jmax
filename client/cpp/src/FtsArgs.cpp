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

#include <fts/client/ftsclient.h>

namespace ircam {
namespace fts {
namespace client {

#define INITIAL_SIZE 8

  FtsArgs::FtsArgs()
  {
    _size = INITIAL_SIZE;
    _array = new FtsAtom[_size];

    for ( int i = 0; i < _size; i++)
      _array[i].setVoid();

    _current = 0;
  }

  FtsArgs::~FtsArgs()
  {
      delete[] _array;
  }

  void FtsArgs::ensureCapacity( int wanted)
  {
    if ( _current + wanted < _size)
      return;

    while (_size <= _current + wanted)
      _size *= 2;

    FtsAtom *newArray = new FtsAtom[_size];

    for ( int i = 0; i < _current; i++)
      newArray[i] = _array[i];

    for ( int i = _current; i < _size; i++)
      newArray[i].setVoid();

    _array = newArray;
  }

}
}
}
