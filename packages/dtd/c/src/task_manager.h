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

#ifndef _FTS_TASK_MANAGER_H_
#define _FTS_TASK_MANAGER_H_

#include "dtd.h"

typedef struct _fts_task_manager_t fts_task_manager_t;

DTD_API fts_task_manager_t* fts_task_manager_new(int num_threads);
DTD_API void fts_task_manager_delete(fts_task_manager_t* man);

DTD_API int fts_task_manager_add_task(fts_task_manager_t* man, fts_object_t* obj, int winlet, 
				      fts_symbol_t s, int ac, const fts_atom_t *at);



#endif /* _FTS_TASK_MANAGER_H_ */