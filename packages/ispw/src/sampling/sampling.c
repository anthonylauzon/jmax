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
#include "sampfilt.h"

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

static void sampling_module_init(void)
{
  if(!make_sampfilt_tab())
    post("fts_sampling_init: out of memory\n");

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
}

fts_module_t sampling_module = {"sampling", "ISPW sampling and delayline classes", sampling_module_init, 0};
