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

package ircam.ftsclient;

import java.io.*;

abstract class FtsProtocolEncoder {
  abstract void writeInt( int v) throws IOException;
  abstract void writeFloat( float v) throws IOException;
  abstract void writeSymbol( FtsSymbol v) throws IOException;
  abstract void writeString( String v) throws IOException;
  abstract void writeObject( FtsObject v) throws IOException;

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
	else if ( atoms[i].isObject())
	  writeObject( atoms[i].objectValue);
      }
  }

  void writeArgs( FtsArgs v) throws IOException
  {
    writeAtoms( v.getAtoms(), 0, v.getLength());
  }

  abstract void flush() throws IOException;
}

