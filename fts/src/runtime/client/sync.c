/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

