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
 */

#include "data.h"

extern void value_config(void);
extern void vec_config(void);
extern void ivec_config(void);
extern void fvec_config(void);
extern void cvec_config(void);
extern void mat_config(void);
extern void fmat_config(void);
extern void col_config(void);
extern void row_config(void);
extern void bpf_config(void);
extern void dict_config(void);
extern void preset_config(void);

extern void getval_config(void);
extern void getinter_config(void);
extern void getsize_config(void);
extern void getrange_config(void);

extern void dumpfile_config(void);

void 
data_object_set_dirty(fts_object_t *o)
{
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  
  if(patcher != NULL && data_object_get_keep((data_object_t *)o) == fts_s_yes)
    fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

void
data_object_daemon_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  data_object_t *this = (data_object_t *)obj;

  if(fts_is_symbol(value) && this->keep != fts_s_args)
    this->keep = fts_get_symbol(value);
}

void
data_object_daemon_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  data_object_t *this = (data_object_t *)obj;

  fts_set_symbol(value, this->keep);
}

void
data_config(void)
{
  value_config();
  vec_config();
  ivec_config();
  fvec_config();
  cvec_config();
  mat_config();
  fmat_config();
  col_config();
  row_config();
  bpf_config();
  dict_config();
  preset_config();

  getval_config();  
  getinter_config();
  getsize_config();  
  getrange_config();  

  dumpfile_config();
}

