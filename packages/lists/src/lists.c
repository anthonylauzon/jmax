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

extern void list_config(void);
extern void listarith_config(void);
extern void listsum_config(void);
extern void listelement_config(void);
extern void listhead_config(void);
extern void listjoin_config(void);
extern void listlength_config(void);
extern void listreverse_config(void);
extern void listsublist_config(void);
extern void listtail_config(void);

static void
lists_module_init(void)
{
  list_config();
  listarith_config();
  listsum_config();
  listelement_config();
  listhead_config();
  listjoin_config();
  listlength_config();
  listreverse_config();
  listsublist_config();
  listtail_config();
}

fts_module_t lists_module = {"lists", "basic list handling classes", lists_module_init};
