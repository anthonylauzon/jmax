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

#include <string.h>

#include "protocol.h"
#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"
#include "runtime/devices.h"
#include "runtime/client/client.h"

static void fts_sync_dispatch(int ac, const fts_atom_t *av);


void fts_client_sync_init(void)
{
  fts_client_mess_install(SYNC_CODE, fts_sync_dispatch);
}


/* The Sync subsystem; just answer to message 
   SYNC_CODE with a message SYNC_DONE_CODE; it is used for syncronization;
   A client can send a number of requests, and then a PING; at the reception
   of the PONG, it is sure that all the answerback invoked by the previous 
   requests has been processed.
 */

static void
fts_sync_dispatch(int ac, const fts_atom_t *av)
{
  fts_client_updates_sync();

  fts_client_mess_start_msg(SYNC_DONE_CODE);
  fts_client_mess_send_msg();
}

