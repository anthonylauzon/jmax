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
#include <fts/fts.h>

extern void messtab_config(void);
extern void message_table_config(void);

static void
mess_init(void)
{
  messtab_config();
  message_table_config();
}

fts_module_t mess_module = {"mess", "message classes", mess_init, 0, 0};

