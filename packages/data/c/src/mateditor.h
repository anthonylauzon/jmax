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

#ifndef _DATA_MATEDITOR_H_
#define _DATA_MATEDITOR_H_

#include <fts/packages/data/data.h>

typedef struct _mateditor_
{
  fts_object_t o;
  
  int type;
  
  int opened; /* non zero if editor open */
  int first_vis_row; /* first visible row index */
  int last_vis_row; /* last visible row index */
  
  fts_object_t *mat;
} mateditor_t;

DATA_API fts_symbol_t mateditor_symbol;
DATA_API fts_class_t *mateditor_type;

DATA_API int mateditor_get_m( mateditor_t *mateditor);
DATA_API int mateditor_get_n( mateditor_t *mateditor);

/*DATA_API void mateditor_dump_gui(mateditor_t *tabeditor, fts_dumper_t *dumper);
DATA_API void mateditor_upload_gui(tabeditor_t *tabeditor);
DATA_API void mateditor_upload_interval(tabeditor_t *tabeditor, int start, int end);*/

#endif
