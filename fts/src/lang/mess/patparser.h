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
#ifndef _PATPARSER_H_
#define _PATPARSER_H_


extern fts_object_t *fts_load_dotpat_patcher(fts_object_t *parent, fts_symbol_t name);
extern void fts_patparse_set_font_size_table(int ac, const fts_atom_t *at);

#endif
