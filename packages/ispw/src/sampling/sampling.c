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

extern void sampbuf_table_init(void);
extern void sigtable_config(void);
extern void sampread_config(void);
extern void sampwrite_config(void);
extern void samppeek_config(void);
extern void tabpoke_config(void);
extern void tabpeek_config(void);
extern void tabcycle_config(void);

extern void delay_table_init(void);
extern void delwrite_config(void);
extern void delread_config(void);
extern void vd_config(void);

extern void samples_unit_init(void);

static void ispw_sampling_module_init(void)
{
  sampbuf_table_init();
  
  sigtable_config();
  sampwrite_config();
  sampread_config();
  samppeek_config();
  tabpeek_config();
  tabpoke_config();
  tabcycle_config();
  
  delay_table_init();
  delwrite_config();
  delread_config();
  vd_config();

  samples_unit_init();
}

fts_module_t ispw_sampling_module = {"ISPW sampling", "ISPW sampling and delayline classes", ispw_sampling_module_init, 0, 0};
