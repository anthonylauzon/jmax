/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:52:55 $
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
