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
 */
#ifndef _DATA_H
#define _DATA_H

#include <fts/fts.h>

#ifdef WIN32
#if defined(DATA_EXPORTS)
#define DATA_API __declspec(dllexport)
#else
#define DATA_API __declspec(dllimport)
#endif
#else
#define DATA_API extern
#endif

#define data_float_equals(f, g) (fabs((f) - (g)) < 1.0e-7)

DATA_API void data_object_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);  

#include <fts/packages/data/mat.h>
#include <fts/packages/data/vec.h>
#include <fts/packages/data/ivec.h>
#include <fts/packages/data/fmat.h>
#include <fts/packages/data/fvec.h>
#include <fts/packages/data/cvec.h>
#include <fts/packages/data/dict.h>
#include <fts/packages/data/bpf.h>

DATA_API void data_config(void);

#endif /* _DATA_H */
