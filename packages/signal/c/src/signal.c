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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include "signal.h"

extern void signal_tilda_config(void);
extern void signal_binop_config(void);

extern void signal_pink_config(void);
extern void signal_white_config(void);

extern void signal_osc_config(void);
extern void signal_wave_config(void);

extern void signal_matrix_config(void);

extern void signal_cut_config(void);

extern void signal_play_config(void);
extern void signal_play_fvec_config(void);
extern void signal_play_bpf_config(void);

extern void signal_rec_fvec_config(void);



void
signal_config(void)
{
  signal_tilda_config();
  signal_binop_config();

  signal_pink_config();
  signal_white_config();

  signal_osc_config();
  signal_wave_config();

  signal_matrix_config();

  signal_cut_config();

  signal_play_config();

  signal_play_fvec_config();
  signal_rec_fvec_config();

  signal_env_config();
}
