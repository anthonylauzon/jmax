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

#include "fts.h"
#include "intvec.h"
#include "floatvec.h"
#include "vector.h"
#include "matrix.h"

extern void val_config(void);
extern void ref_config(void);

extern void ivec_config(void);
extern void fvec_config(void);
extern void mat_config(void);

extern void getelem_config(void);
extern void getsize_config(void);
extern void getlist_config(void);

extern void fill_config(void);
extern void copy_config(void);

static void
data_init(void)
{
  val_config();
  ref_config();

  ivec_config();
  fvec_config();
  mat_config();

  getelem_config();  
  getsize_config();  
  getlist_config();  

  fill_config();
  copy_config();
}

fts_module_t data_module = {"data", "data structures", data_init, 0, 0};


