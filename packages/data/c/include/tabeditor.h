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
 */

#ifndef _DATA_TABEDITOR_H_
#define _DATA_TABEDITOR_H_

#include "data.h"

typedef struct _tabeditor_
{
  fts_object_t o;

  int opened; /* non zero if editor open */
  int vsize; /* visible points */
  int vindex; /* first visible point */
  float zoom; /* current zoom */
  int pixsize; /* visible pixels size */

  fts_object_t *vec;

} tabeditor_t;

DATA_API fts_symbol_t tabeditor_symbol;
DATA_API fts_class_t *tabeditor_type;

DATA_API void tabeditor_insert_append(tabeditor_t *tabeditor, int onset, int ac, const fts_atom_t *at);
DATA_API void tabeditor_send( tabeditor_t *tabeditor);

#endif
