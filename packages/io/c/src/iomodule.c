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

extern void udp_config(void);
extern void udpmessage_config(void);

static void
fts_io_init(void)
{
  udp_config();
  udpmessage_config();
}

fts_module_t io_module = {"io", "I/O objects", fts_io_init, 0, 0, 0};
