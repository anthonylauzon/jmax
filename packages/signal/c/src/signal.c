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
#include <utils/c/include/utils.h>

extern void signal_bus_config(void);

extern void signal_binop_config(void);
extern void signal_abs_config(void);

extern void signal_tilda_config(void);
extern void signal_env_config(void);

extern void signal_pink_config(void);
extern void signal_white_config(void);
extern void signal_osc_config(void);
extern void signal_wave_config(void);

extern void signal_fir_config(void);
extern void signal_iir_config(void);
extern void signal_biquad_config(void);
extern void signal_wahwah_config(void);

extern void signal_samphold_config(void);
extern void signal_snapshot_config(void);
extern void signal_threshold_config(void);
extern void signal_zerocross_config(void);

extern void signal_matrix_config(void);
extern void signal_delay_config(void);
extern void signal_harmtap_config(void);

extern void signal_pick_config(void);
extern void signal_cut_config(void);
extern void signal_paste_config(void);

extern void signal_play_fvec_config(void);
extern void signal_rec_fvec_config(void);

void
signal_config(void)
{
  signal_bus_config();

  signal_binop_config();
  signal_abs_config();

  signal_tilda_config();
  signal_env_config();

  signal_pink_config();
  signal_white_config();
  signal_osc_config();
  signal_wave_config();

  signal_fir_config();
  signal_iir_config();
  signal_biquad_config();
  signal_wahwah_config();

  signal_samphold_config();
  signal_snapshot_config();
  signal_threshold_config();
  signal_zerocross_config();

  signal_matrix_config();
  signal_delay_config();
  signal_harmtap_config();

  signal_pick_config();
  signal_cut_config();
  signal_paste_config();

  signal_play_fvec_config();
  signal_rec_fvec_config();
}
