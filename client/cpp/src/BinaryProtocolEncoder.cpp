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

#include "BinaryProtocolEncoder.h"

using namespace ircam::fts::client;

namespace ircam {
namespace fts {
namespace client {

  class OutputBuffer {
  public:
    OutputBuffer()
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

BinaryProtocolEncoder::BinaryProtocolEncoder( FtsServerConnection *connection)
{
  _connection = connection;
  _outputBuffer = new OutputBuffer();
  _symbolCache = new SymbolCache();
}

void BinaryProtocolEncoder::writeInt( int v) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::INT);
  write( v);
}

void BinaryProtocolEncoder::writeDouble( double v) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::FLOAT);
  //    write( Double.doubleToRawLongBits( v));
}

void BinaryProtocolEncoder::writeSymbol( const FtsSymbol *v) throw( FtsClientException)
{
  int index = _symbolCache->index( v);

  if (_symbolCache->get(index) == v)
    {
      _outputBuffer->append( BinaryProtocol::SYMBOL_INDEX);
      write( index);
    }
  else
    {
      _symbolCache->put( index, v);

      _outputBuffer->append( BinaryProtocol::SYMBOL_CACHE);
      write( index);
      write( v->getString());
    }
}

void BinaryProtocolEncoder::writeString( const char *v) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::STRING);
  write( v);
}

void BinaryProtocolEncoder::writeRawString( const char *v) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::RAW_STRING);
  write( v);
}

void BinaryProtocolEncoder::writeObject( const FtsObject *v) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::OBJECT);

  //     if (v != null)
  //       write( v.getID());
  //     else
  //       write( 0);
}

/* This version is used for object that have predefined IDs */
void BinaryProtocolEncoder::writeObject( int id) throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::OBJECT);
  write( id);
}

void BinaryProtocolEncoder::writeAtoms( const FtsAtom *atoms, int offset, int length) throw (FtsClientException)
{
  for ( int i = offset; i < length; i++)
    {
      if ( atoms[i].isInt())
	writeInt( atoms[i].intValue);
      else if ( atoms[i].isDouble())
	writeDouble( atoms[i].doubleValue);
      else if ( atoms[i].isSymbol())
	writeSymbol( atoms[i].symbolValue);
      else if ( atoms[i].isString())
	writeString( atoms[i].stringValue);
      else if ( atoms[i].isRawString())
	writeRawString( atoms[i].stringValue);
      else if ( atoms[i].isObject())
	writeObject( atoms[i].objectValue);
    }
}

void BinaryProtocolEncoder::writeArgs( const FtsArgs &v) throw( FtsClientException)
{
  writeAtoms( v.getAtoms(), 0, v.getLength());
}

void BinaryProtocolEncoder::flush() throw( FtsClientException)
{
  _outputBuffer->append( BinaryProtocol::END_OF_MESSAGE);

  _connection->write( _outputBuffer->getBytes(), 0, _outputBuffer->getLength());

  _outputBuffer->clear();
}


void BinaryProtocolEncoder::write( int v)
{
  _outputBuffer->append( (unsigned char) ((v >> 24) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 16) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 8) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 0) & 0xff));
}

void BinaryProtocolEncoder::write( long v)
{
  // FIXME
  //     _outputBuffer->append( (unsigned char) ((v >> 56) & 0xff));
  //     _outputBuffer->append( (unsigned char) ((v >> 48) & 0xff));
  //     _outputBuffer->append( (unsigned char) ((v >> 40) & 0xff));
  //     _outputBuffer->append( (unsigned char) ((v >> 32) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 24) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 16) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 8) & 0xff));
  _outputBuffer->append( (unsigned char) ((v >> 0) & 0xff));
}

void BinaryProtocolEncoder::write( const char *v)
{
  while (*v)
    {
      _outputBuffer->append( *v);
      v++;
    }

  _outputBuffer->append( '\0');
}

