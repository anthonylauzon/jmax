/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

extern void accum_config(void);
extern void bag_config(void);
extern void float_config(void);
extern void symbol_obj_config(void);
extern void funbuff_config(void);
extern void integer_config(void);
extern void pbank_config(void);
extern void table_config(void);
extern void value_config(void);
extern void expr_config(void);
extern void expr_doctor_init(void);

static void
data_module_init(void)
{
  accum_config();
  bag_config();
  float_config();
  integer_config();
  symbol_obj_config();
  funbuff_config();
  pbank_config();
  table_config();
  value_config();

  /* Note: expr is here because it needs to be in the same
     .so library than table

     The expr doctor evaluating new expressions.
     */

  expr_config();
  expr_doctor_init();
}

fts_module_t data_module = {"data", "ISPW data classes", data_module_init};
