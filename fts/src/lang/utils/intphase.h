/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _INTPHASE_H_
#define _INTPHASE_H_

/* this type should have at least 24 bits and be efficient to be calculated */
typedef int fts_intphase_t;

#define FTS_INTPHASE_BITS (31)
#define FTS_INTPHASE_MAX (0x7fffffffL)
#define FTS_INTPHASE_RANGE (2147483648.)

#define fts_intphase_wrap(phi) \
  ((phi) & FTS_INTPHASE_MAX)

#define fts_intphase_get_index(phi, bits) \
  ((phi) >> (FTS_INTPHASE_BITS - (bits)))

#define fts_intphase_get_frac(phi, bits) \
  ((float)((phi) & ((1 << (FTS_INTPHASE_BITS - (bits))) - 1)) * (float)(1.0 / (1 << (FTS_INTPHASE_BITS - (bits)))))

#define fts_intphase_float(phi) \
  ((float)(phi) * (float)(1.0 / FTS_INTPHASE_RANGE))

#define fts_intphase_double(phi) \
  ((double)((phi) * (double)(1.0 / FTS_INTPHASE_RANGE)))

#define fts_intphase_lookup(phi, array, bits) \
  ((array)[fts_intphase_get_index((phi), (bits))])

#endif
