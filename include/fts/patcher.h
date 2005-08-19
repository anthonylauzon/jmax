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

typedef struct patcher_inout patcher_inout_t;

struct fts_patcher
{
  fts_object_t o;
  
  int description_ac;
  fts_atom_t *description_at;
  
  patcher_inout_t **inlets; /* array of patcher inlets */
  patcher_inout_t **outlets; /* array of patcher outlets */
  
  int n_inlets;
  int n_outlets;
  
  fts_object_t *objects; /* list of patcher objects*/
  
  int open; /* the open flag */
  int editor_open; /* the open editor flag */
  int load_init_fired; /* the multiple load init protection flag */
  int deleted; /* set to one during content deleting */
  int dirty; /* set to one if patcher's content is not saved */
  
  fts_tuple_t *args; /* the arguments used for the "args" variable */
  
  enum {fts_p_standard, fts_p_abstraction, fts_p_template} type;
  
  int scope; /* scope flag: non-zero for top level patchers and templates */
  
  /* If this patcher is a template, point to the template definition */
  fts_template_t *template_definition;
  fts_hashtable_t *definitions;
  fts_symbol_t file_name;
  
  int save_id;
};    

FTS_API fts_patcher_t *fts_patcher_get_scope(fts_patcher_t *patcher);

#define fts_patcher_is_open(p)     ((p)->open)
FTS_API void fts_patcher_set_dirty(fts_patcher_t *self, int is_dirty);

FTS_API fts_patcher_t *fts_get_root_patcher(void);
#define fts_get_global_definitions() (fts_get_root_patcher()->definitions)

/** 
* Return an iterator to enumerate all objects contained by the given patcher
* 
* @fn void fts_patcher_get_objects(fts_patcher_t* patcher, fts_iterator_t* i)
* @param patcher the patcher
* @param i the iterator
*/
FTS_API void fts_patcher_get_objects(fts_patcher_t* patcher, fts_iterator_t* i);

/**
 * get scope (that bears variable definitions) for this patcher
 * is either the top level patcher or the root patcher for global variables 
 */
FTS_API fts_patcher_t *fts_patcher_get_scope(fts_patcher_t *patcher);
