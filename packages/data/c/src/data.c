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

#include <data/c/include/data.h>

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

extern void getinter_config(void);
extern void getsize_config(void);
extern void getrange_config(void);

extern void dumpfile_config(void);

void 
data_object_init(fts_object_t *o)
{
  data_object_t *this = (data_object_t *)o;
  
  this->persistence = data_object_persistence_no;
}

void 
data_object_set_dirty(fts_object_t *o)
{
  data_object_t *this = (data_object_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  
  if(patcher != NULL && data_object_is_persistent(o))
    fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

void
data_object_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  data_object_t *this = (data_object_t *)o;

  if(ac > 0)
    {
      /* set persistence flag (if its not set to args) */
      if(fts_is_number(at) && this->persistence > data_object_persistence_args)
	{
	  this->persistence = (fts_get_number_int(at) != 0);
	  fts_client_send_message(o, fts_s_persistence, 1, at);
	}
    }
  else
    {
      /* return persistence flag */
      fts_atom_t a;

      fts_set_int(&a, this->persistence);
      fts_return(&a);
    }
}

void
data_object_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  data_object_t *this = (data_object_t *)o;
  fts_atom_t a;

  fts_set_int(&a, (this->persistence > data_object_persistence_no));
  fts_client_send_message(o, fts_s_persistence, 1, &a);
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

  getinter_config();
  getsize_config();  
  getrange_config();  

  dumpfile_config();
}

