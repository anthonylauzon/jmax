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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _PHASOR_H_
#define _PHASOR_H_

extern fts_symbol_t phasor_function;
extern fts_symbol_t phasor_inplace_function;

typedef struct _phasor_ftl phasor_ftl_t;

extern ftl_data_t phasor_ftl_data_new(void);
extern void phasor_ftl_data_init(ftl_data_t ftl_data, float sr);
extern void phasor_ftl_set_phase(ftl_data_t ftl_data, float phi);

extern void phasor_ftl(fts_word_t *argv);
extern void phasor_ftl_inplace(fts_word_t *argv);

#endif





