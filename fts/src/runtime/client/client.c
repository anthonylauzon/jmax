/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


/*
 *  Communication with the client: this file define the client logical
 *  device, the client module including the client poll function
 * 
 *
 */

#include <string.h>

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>

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
static void client_shutdown(void);

static fts_status_t fts_set_client_logical_dev(fts_dev_t *dev, int ac, const fts_atom_t *at);
static fts_status_t fts_unset_client_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_reset_client_logical_dev(void);
static fts_dev_t   *fts_get_client_logical_dev(int ac, const fts_atom_t *at);

fts_module_t fts_client_module = {"Client", "Client communication", client_init, client_shutdown, 0};

static void
client_init(void)
{
  /* Initialization of sub modules */
  fts_client_sync_init();
  fts_client_updates_init();

  /* client logical device  */

  fts_declare_logical_dev(fts_new_symbol("client"),
			  fts_char_dev,
			  fts_set_client_logical_dev,
			  fts_get_client_logical_dev,
			  fts_unset_client_logical_dev,
			  fts_reset_client_logical_dev);
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

void 
fts_client_poll(void)
{
  static int poll_count = 0;

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
	      /* End of file for client device (do a shutdown) */

	      fts_sched_halt(fts_sched_get_current());

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








