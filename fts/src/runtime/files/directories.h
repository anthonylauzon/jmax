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
#ifndef _DIRECTORIES_H_
#define _DIRECTORIES_H_


extern void fts_set_search_path(fts_symbol_t search_path);
extern fts_symbol_t fts_get_search_path(void);

extern void fts_set_project_dir(fts_symbol_t project_dir);
extern fts_symbol_t fts_get_project_dir(void);

#endif
