/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
filters_module_init(void)
{
  biquad_config();
  sigcoef_bpass2_config();
  sigcoef_hlshelf1_config();
  iir_config();
  sigapass3_config();
  wahwah_config();
}

fts_module_t filters_module = {"filters", "ISPW signal filter classes", filters_module_init};
