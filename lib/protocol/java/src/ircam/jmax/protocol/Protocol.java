//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.protocol;

/**
 * The <code>Protocol</code> class defines the values of the protocol tokens.
 *
 * @author        François Déchelle (dechelle@ircam.fr)
 * @version       first one
 */

interface Protocol {
  /* Taken from fts/src/runtime/client/protocol.h */
  public static final byte STRING_START_CODE =  0x01;
  public static final byte STRING_END_CODE =    0x02;
  public static final byte INT_CODE =           0x03;
  public static final byte FLOAT_CODE =         0x04;
  public static final byte SYMBOL_CACHED_CODE = 0x05;
  public static final byte SYMBOL_AND_DEF_CODE =0x06;
  public static final byte SYMBOL_CODE =        0x07;
  public static final byte OBJECT_CODE =        0x08;
  public static final byte CONNECTION_CODE =    0x09;
  public static final byte DATA_CODE =          0x0a;
  public static final byte EOM_CODE =           0x0b;
}
