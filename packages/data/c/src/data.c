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

#include <fts/fts.h>
#include <fts/packages/data/data.h>

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
extern void tabeditor_config(void);

extern void getrange_config(void);

extern void dumpfile_config(void);

void
data_object_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

void
data_config(void)
{
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
  tabeditor_config();

  getrange_config();  

  dumpfile_config();
}
