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

namespace ircam {
namespace fts {
namespace client {

  class Buffer {
  public:
    Buffer()
    {
      _length = 256;
      _buffer = new unsigned char[_length];
      _current = 0;
    }

    void clear()
    {
      _current = 0;
    }

    void append( unsigned char b)
    {
      if (_current + 1 >= _length)
	{
	  _length *= 2;
	  unsigned char *newBuffer = new unsigned char[_length];
	  for ( int i = 0; i < _current; i++)
	    newBuffer[i] = _buffer[i];

	  _buffer = newBuffer;
	}

      _buffer[_current++] = b;
    }

    unsigned char *getBytes()
    {
      return _buffer;
    }

    int getLength()
    {
      return _current;
    }

  private:
    unsigned char *_buffer;
    int _current;
    int _length;
  };

};
};
};