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

extern void zerocross_config(void);
extern void sigthru_config(void);
extern void sigthrow_config(void);
extern void sigthreshold_config(void);
extern void sigswitch_config(void);
extern void sigsnapshot_config(void);
extern void sigline_config(void);
extern void sig_config(void);
extern void sigdown_config(void);
extern void sigup_config(void);
extern void phasor_config(void);
extern void osc_config(void);
extern void sigtab1_config(void);
extern void noise_config(void);
extern void sigparam_config(void);
extern void sigprint_config(void);
extern void vectrigon_config(void);
extern void vecsqrt_config(void);
extern void veclog_config(void);
extern void vecclip_config(void);
extern void sigarctan2_config(void);
extern void sigabs_config(void);
extern void fft_config(void);
extern void sigsamphold_config(void);

static void ispw_signal_module_init(void)
{
  zerocross_config();
  sigthru_config();
  sigthrow_config();
  sigthreshold_config();
  sigswitch_config();
  sigsnapshot_config();
  sigline_config();
  sig_config();
  sigdown_config();
  sigup_config();
  phasor_config();
  osc_config();
  sigtab1_config();
  noise_config();
  sigparam_config();
  sigprint_config();
  vectrigon_config();
  vecsqrt_config();
  veclog_config();
  vecclip_config();
  sigarctan2_config();
  sigabs_config();
  fft_config();
  sigsamphold_config();
}

fts_module_t ispw_signal_module = {"ISPW signal", "ISPW miscellaneous signal classes", ispw_signal_module_init, 0, 0};
