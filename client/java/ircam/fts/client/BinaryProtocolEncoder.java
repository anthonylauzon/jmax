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

package ircam.fts.client;

import java.io.*;

class BinaryProtocolEncoder {

  // We don't use java.io.BufferedOutputStream here because it does not resize
  // the buffer, and in case of an UDP connection, we want to be sure that the
  // formatted message will be send in one packet.

  class OutputBuffer {
    OutputBuffer()
    {
      length = 256;
      buffer = new byte[length];
      current = 0;
    }

    void clear()
    {
      current = 0;
    }

    final void append( byte b)
    {
      if (current + 1 >= length)
	{
	  length *= 2;
	  byte[] newBuffer = new byte[length];
	  System.arraycopy( buffer, 0, newBuffer, 0, current);
	  buffer = newBuffer;
	}

      buffer[current++] = b;
    }

    byte[] getBytes()
    {
      return buffer;
    }

    int getLength()
    {
      return current;
    }

    private byte[] buffer;
    private int current;
    private int length;
  }

  BinaryProtocolEncoder( FtsServerConnection connection)
  {
    this.connection = connection;
    outputBuffer = new OutputBuffer();
    symbolCache = new SymbolCache();
  }

  private final void write( int v)
  {
    outputBuffer.append( (byte) ((v >> 24) & 0xff));
    outputBuffer.append( (byte) ((v >> 16) & 0xff));
    outputBuffer.append( (byte) ((v >> 8) & 0xff));
    outputBuffer.append( (byte) ((v >> 0) & 0xff));
  }

  private final void write( String v)
  {
    for ( int i = 0; i < v.length(); i++)
      outputBuffer.append( (byte)v.charAt(i));

    outputBuffer.append( (byte)0);
  }

  void writeInt( int v) throws IOException
  {
    outputBuffer.append( BinaryProtocol.INT);
    write( v);
  }

  void writeFloat( float v) throws IOException
  {
    outputBuffer.append( BinaryProtocol.FLOAT);
    write( Float.floatToRawIntBits(v));
  }

  void writeSymbol( FtsSymbol v) throws IOException
  {
    int index = symbolCache.index( v);

    if (symbolCache.get(index) == v)
      {
	outputBuffer.append( BinaryProtocol.SYMBOL_INDEX);
	write( index);
      }
    else
      {
	symbolCache.put( index, v);

	outputBuffer.append( BinaryProtocol.SYMBOL_CACHE);
	write( index);
	write( v.toString());
      }
  }

  void writeString( String v) throws IOException
  {
    outputBuffer.append( BinaryProtocol.STRING);
    write( v);
  }

  void writeRawString( String v) throws IOException
  {
    outputBuffer.append( BinaryProtocol.RAW_STRING);
    write( v);
  }

  void writeObject( FtsObject v) throws IOException
  {
    outputBuffer.append( BinaryProtocol.OBJECT);

    if (v != null)
      write( v.getID());
    else
      write( 0);
  }

  void writeAtoms( FtsAtom[] atoms, int offset, int length) throws IOException
  {
    for ( int i = offset; i < length; i++)
      {
	if ( atoms[i].isInt())
	  writeInt( atoms[i].intValue);
	else if ( atoms[i].isFloat())
	  writeFloat( atoms[i].floatValue);
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

  void writeArgs( FtsArgs v) throws IOException
  {
    writeAtoms( v.getAtoms(), 0, v.getLength());
  }

  void flush() throws IOException
  {
    outputBuffer.append( BinaryProtocol.END_OF_MESSAGE);

    connection.write( outputBuffer.getBytes(), 0, outputBuffer.getLength());

    outputBuffer.clear();
  }

  private OutputBuffer outputBuffer;
  private SymbolCache symbolCache;
  private FtsServerConnection connection;
}

