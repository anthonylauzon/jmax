/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#ifndef _PROTODEFS_H
#define _PROTODEFS_H

/* 
   Description:
     constants for the binary protocol
*/

/* Taken from fts/src/runtime/client/protocol.h */
enum protocol {
  STRING_START_CODE =  0x01,
  STRING_END_CODE =    0x02,

  INT_CODE =           0x03,
  FLOAT_CODE =         0x04,
  SYMBOL_CACHED_CODE = 0x05,
  SYMBOL_AND_DEF_CODE =0x06,
  SYMBOL_CODE =        0x07,
  OBJECT_CODE =        0x08,
  CONNECTION_CODE =    0x09,
  DATA_CODE =          0x0a,
  EOM_CODE =           0x0b
};

#endif
