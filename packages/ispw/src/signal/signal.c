/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

extern void zerocross_config(void);
extern void sigthru_config(void);
extern void sigthrow_config(void);
extern void sigcatch_config(void);
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
extern void vecarith_config(void);
extern void vectrigon_config(void);
extern void vecsqrt_config(void);
extern void veclog_config(void);
extern void vecclip_config(void);
extern void sigarctan2_config(void);
extern void sigabs_config(void);
extern void sbinop_config(void);
extern void fft_config(void);
extern void sigsamphold_config(void);

static void
signal_module_init(void)
{
  zerocross_config();
  sigthru_config();
  sigthrow_config();
  sigcatch_config();
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
  vecarith_config();
  vectrigon_config();
  vecsqrt_config();
  veclog_config();
  vecclip_config();
  sigarctan2_config();
  sigabs_config();
  sbinop_config();
  fft_config();
  sigsamphold_config();
}

fts_module_t signal_module = {"signal", "ISPW miscellaneous signal classes", signal_module_init};
