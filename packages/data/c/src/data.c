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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

extern void value_config(void);

extern void ivec_config(void);
extern void fvec_config(void);
extern void fmat_config(void);
extern void vec_config(void);
extern void mat_config(void);
extern void col_config(void);
extern void row_config(void);
extern void bpf_config(void);

extern void getval_config(void);
extern void getinter_config(void);
extern void getsize_config(void);
extern void getrange_config(void);
extern void getlist_config(void);

extern void fill_config(void);
extern void copy_config(void);

extern void preset_config(void);

static void
data_init(void)
{
  value_config();

  ivec_config();
  fvec_config();
  fmat_config();
  vec_config();
  mat_config();
  col_config();
  row_config();
  bpf_config();

  getval_config();  
  getinter_config();  
  getsize_config();  
  getrange_config();  
  getlist_config();  

  fill_config();
  copy_config();

  preset_config();
}

fts_module_t data_module = {"data", "data structures", data_init, 0, 0};
