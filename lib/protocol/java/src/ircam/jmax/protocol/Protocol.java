//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
