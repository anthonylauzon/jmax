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

extern void biquad_config(void);
extern void sigcoef_bpass2_config(void);
extern void sigcoef_hlshelf1_config(void);
extern void iir_config(void);
extern void sigapass3_config(void);
extern void wahwah_config(void);

static void
ispw_filters_module_init(void)
{
  biquad_config();
  sigcoef_bpass2_config();
  sigcoef_hlshelf1_config();
  iir_config();
  sigapass3_config();
  wahwah_config();
}

fts_module_t ispw_filters_module = {"filters", "ISPW signal filter classes", ispw_filters_module_init};
