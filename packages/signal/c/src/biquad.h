/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

typedef struct
{
  float a0;
  float a1;
  float a2;
  float b1;
  float b2;
} biquad_coefs_t;

typedef struct
{
  float ynm1; /* y(n-1) */
  float ynm2; /* y(n-2) */
  float xnm1; /* X(n-1) */
  float xnm2; /* x(n-2) */
} biquad_state_t;

extern void ftl_biquad(fts_word_t *argv);
extern void ftl_biquad_inplace(fts_word_t *argv);
