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

/*
 *  Communication with the client: this file define the client logical
 *  device, the client module including the client poll function
 * 
 *
 */

#include <string.h>

#include "protocol.h"
#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"
#include "runtime/devices.h"
#include "runtime/client/client.h"
#include "runtime/client/incoming.h"
#include "runtime/client/outgoing.h"
#include "runtime/client/sync.h"
#include "runtime/client/updates.h"

/* forward declarations */

/* Global declarations */

/* The device we use to communicate with the host */

fts_dev_t *client_dev = 0;

/******************************************************************************/
/*                                                                            */
/*             Client SubSystem Declaration                                   */
/*                                                                            */
/******************************************************************************/

extern void fts_client_updates_init(void);
extern void fts_client_sync_init(void);

static void client_init(void);
static void client_restart(void);
static void client_shutdown(void);

static void client_poll(void);

static fts_status_t fts_set_client_logical_dev(fts_dev_t *dev, int ac, const fts_atom_t *at);
static fts_status_t fts_unset_client_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_reset_client_logical_dev(void);
static fts_dev_t   *fts_get_client_logical_dev(int ac, const fts_atom_t *at);

fts_module_t fts_client_module = {"Client", "Client communication", client_init, client_restart, client_shutdown, 0};

static void
client_init(void)
{
  /* Initialization of sub modules */

  fts_client_incoming_init();
  fts_client_sync_init();
  fts_client_updates_init();

  /* schedule declaration */

  fts_sched_declare(client_poll, provide, fts_new_symbol("control"), "client_poll");

  /* client logical device  */

  fts_declare_logical_dev(fts_new_symbol("client"),
			  fts_char_dev,
			  fts_set_client_logical_dev,
			  fts_get_client_logical_dev,
			  fts_unset_client_logical_dev,
			  fts_reset_client_logical_dev);
}


static void
client_restart(void)
{
  fts_client_incoming_restart();
}

static void
client_shutdown(void)
{
  if (client_dev)
    {
      fts_dev_close(client_dev);
      client_dev = 0;
    }
}

/******************************************************************************/
/*                                                                            */
/*             CLIENT logical device                                          */
/*                                                                            */
/******************************************************************************/

static fts_status_t
fts_set_client_logical_dev(fts_dev_t *dev, int ac, const fts_atom_t *at)
{
  client_dev = dev;

  return fts_Success;
}

static fts_status_t
fts_unset_client_logical_dev(int ac, const fts_atom_t *at)
{
  client_dev = 0;

  return fts_Success;
}

static fts_status_t
fts_reset_client_logical_dev(void)
{
  /* No reset action for the client (otherwise, bye bye FTS) */

  return fts_Success;
}

static fts_dev_t *
fts_get_client_logical_dev(int ac, const fts_atom_t *at)
{
  return client_dev;
}

/******************************************************************************/
/*                                                                            */
/*             CLIENT  polling function                                       */
/*                                                                            */
/******************************************************************************/

/* experimentally, we do the real polling every 3 ticks */

static int restart_on_eof = 0;
static int poll_count = 0;

static int debug_count = 0;

static void
client_poll(void)
{
  poll_count++;

  if (poll_count >= 3)
    {
      poll_count = 0;
      return;
    }

  if (client_dev)
    {
      /* then, loop until there are data */

      while (1)
	{
	  unsigned char c;
	  fts_status_t ret;

	  ret = fts_char_dev_get(client_dev, &c);

	  if (ret == &fts_dev_eof)
	    {
	      /* End of file for client device;
		 do a restart or shutdown */

	      if (restart_on_eof)
		fts_restart();
	      else
		fts_halt();

	      return;
	    }
	  else if (ret != fts_Success)
	    {
	      /* flush the output and return */

	      fts_char_dev_flush(client_dev);
	      return;
	    }

	  fts_client_parse_char((char) c);
	}
    }
}


/* functions to set the action on end of file */

void set_restart_on_eof(int v)
{
  restart_on_eof = v;
}
