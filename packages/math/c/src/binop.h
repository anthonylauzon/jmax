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
#ifndef _BINOP_H_
#define _BINOP_H_

#include <fts/fts.h>
#include "ivec.h"
#include "fvec.h"

extern fts_symbol_t math_sym_add;
extern fts_symbol_t math_sym_sub;
extern fts_symbol_t math_sym_mul;
extern fts_symbol_t math_sym_div;
extern fts_symbol_t math_sym_bus;
extern fts_symbol_t math_sym_vid;

extern fts_symbol_t math_sym_ee;
extern fts_symbol_t math_sym_ne;
extern fts_symbol_t math_sym_gt;
extern fts_symbol_t math_sym_ge;
extern fts_symbol_t math_sym_lt;
extern fts_symbol_t math_sym_le;

extern fts_symbol_t math_sym_min;
extern fts_symbol_t math_sym_max;

#endif
