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

extern void qlist_config(void);

static void
fts_qlist_init(void)
{
  qlist_config();
}

fts_module_t qlist_module = {"qlist", "qlist que file classes", fts_qlist_init};
