/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 *
 */

#ifndef JMAX25_PROTOCOLS_H
#define JMAX25_PROTOCOLS_H 1


/* Value coding */

/*
   Objects are represented as a positive number corresponding to its ID.
*/
   
#define JMAX25_STRING_CODE         0x01
#define JMAX25_STRING_END_CODE     0x02
#define JMAX25_INT_CODE            0x03
#define JMAX25_FLOAT_CODE          0x04
#define JMAX25_SYMBOL_CACHED_CODE  0x05
#define JMAX25_SYMBOL_AND_DEF_CODE 0x06
#define JMAX25_SYMBOL_CODE         0x07
#define JMAX25_OBJECT_CODE         0x08
#define JMAX25_CONNECTION_CODE     0x09
#define JMAX25_DATA_CODE           0x0a
#define JMAX25_EOM_CODE            0x0b

#endif /* JMAX25_PROTOCOL_H */

