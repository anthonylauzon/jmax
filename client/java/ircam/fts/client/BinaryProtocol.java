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

interface BinaryProtocol {
  public static final byte INT               = (byte)0x01;
  public static final byte FLOAT             = (byte)0x02;
  public static final byte SYMBOL_INDEX      = (byte)0x03;
  public static final byte SYMBOL_CACHE      = (byte)0x04;
  public static final byte STRING            = (byte)0x05;
  public static final byte OBJECT            = (byte)0x06;
  public static final byte RAW_STRING        = (byte)0x07;
  public static final byte END_OF_MESSAGE    = (byte)0x0F;
}

