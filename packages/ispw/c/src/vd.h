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
 */

typedef struct{
  float write_advance; /* delwrite~'s tick size if write before read (if not 0) */
  float conv; /* unit to samples conversion */
  float max_span; /* maximum travel during one vector (max_span / (<vector size> - 1) = <max. increment> = <max. transp.> + 1 */
} ftl_vd_t;

extern void ftl_vd(fts_word_t *argv);
extern void ftl_vd_inplace(fts_word_t *argv);
extern void ftl_vd_miller(fts_word_t *argv);
