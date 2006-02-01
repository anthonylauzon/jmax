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

#ifndef _DATA_TABEDITOR_H_
#define _DATA_TABEDITOR_H_

#include <fts/packages/data/data.h>

typedef struct _tabeditor_
{
  fts_object_t o;

  int type;
  
  int opened; /* non zero if editor open */
  int vsize; /* visible points */
  int win_size; /* possible visible scope in window */
  int vindex; /* first visible point */
  float zoom; /* current zoom */
  int pixsize; /* visible pixels size */

  float min_val;/* range */
  float max_val;
  
  fts_object_t *vec;
  fts_object_t *copy;

} tabeditor_t;

DATA_API fts_symbol_t tabeditor_symbol;
DATA_API fts_class_t *tabeditor_type;

DATA_API void tabeditor_insert_append(tabeditor_t *tabeditor, int onset, int ac, const fts_atom_t *at);
DATA_API void tabeditor_send( tabeditor_t *tabeditor);

DATA_API int tabeditor_get_size( tabeditor_t *tabeditor);

DATA_API void tabeditor_dump_gui(tabeditor_t *tabeditor, fts_dumper_t *dumper);
DATA_API void tabeditor_upload_gui(tabeditor_t *tabeditor);

#endif
