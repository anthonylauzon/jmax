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

#ifndef _DATA_FVEC_H_
#define _DATA_FVEC_H_

#include <fts/packages/data/data.h>

#define fvec_get_size(v) ((v)->m * (v)->n)
#define fvec_set_size(v, s) fmat_set_size((v), (s), 1)

#define fvec_get_ptr(v) ((v)->values)
#define fvec_get_element(m, i) ((m)->values[(i)])
#define fvec_set_element(v, i, x) ((v)->values[i] = (x))

#define fvec_copy(o, c) fmat_copy((o), (c))
#define fvec_set_const(v, c) fmat_set_const((v), (c))
#define fvec_set_with_onset_from_atoms(v, o, n, a) fmat_set_from_atoms((v), (o), 1, (n), (a))

#endif
