/* 
   This file define the update generation scheduling mechanism for objects.

   NEW version for FTS 2.0.

   In FTS 2.0, the flow control is gone; updates are sent directly as
   property values; we allow update-per-ticks updates per ticks, where
   the update-per-ticks parameter can be set with a UCS command (for the moment),
   and default to 100.

   The unidirectional flow control is gone because we estimate that
   on a UNIX machine, a more "statistical" control may be more
   meaningfull; also, the whole determinism of the flow control make
   little or no sense compared to the complete undeterminism of the
   load given by a control patch.

   The update scheduler use the property change registration mechanism:
   when scheduled, it get the first N property change in the change fifo,
   and send them to the client.
   */


#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"
#include "runtime/devices.h"
#include "runtime/client/protocol.h"
#include "runtime/client/client.h"
#include "runtime/client/updates.h"



void fts_client_updates_init(void);
static void fts_client_updates_sched(void);

/* Default values are for 400 updates per seconds,
 */

static int fts_updates_per_ticks = 40;
static int fts_update_period = 70; /* every how many ticks we do an update */

void
fts_client_updates_init(void)
{
  fts_sched_declare(fts_client_updates_sched, provide, fts_new_symbol("control"), "fts_client_updates_sched");
}

void fts_client_send_property(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a;

  if (obj && name)
    {
      fts_object_get_prop(obj, name, &a);

      if (fts_is_data(&a))
	{
	  /* If the property value is an fts_data, we
	     export the data needed  */

	  fts_data_t *d;
	  
	  d = fts_get_data(&a);

	  if (! fts_data_is_exported(d))
	    fts_data_export(d);
	}
      else if (fts_is_object(&a))
	{
	  /* If the property is an fts_object and is not 
	     uploaded, upload it; note that this will shortly
	     be unified with fts_data model */

	  fts_object_t *obj;

	  obj = fts_get_object(&a);

	  if (obj->id == FTS_NO_ID)
	    fts_client_upload_object(obj);
	}

      /* If the value is void, send a null value only
	 if the property is a special registered property;
	 for the moment, only fts_s_data; it should be done
	 better */

      if (fts_is_void(&a))
	if (name == fts_s_data)
	  fts_set_data(&a, (fts_data_t *) 0);
	else
	  return;

      fts_client_mess_start_msg(CLIENTPROP_CODE);
      fts_client_mess_add_object(obj);
      fts_client_mess_add_symbol(name);
      fts_client_mess_add_atoms(1, &a);
      fts_client_mess_send_msg();
    }
}


static void 
update_group_start(void)
{
  fts_client_mess_start_msg(UPDATE_GROUP_START_CODE);
  fts_client_mess_send_msg();
}


static void 
update_group_end(void)
{
  fts_client_mess_start_msg(UPDATE_GROUP_END_CODE);
  fts_client_mess_send_msg();
}

static int period_count = 0;

static void
fts_client_updates_sched(void)
{
  fts_object_t *obj;
  int one_done = 0;
  fts_symbol_t property;

  if (period_count >=  fts_update_period)
    {
      int update_count;

      period_count = 0;

      for (update_count = 0;
	   (update_count < fts_updates_per_ticks) && fts_object_get_next_change(&property, &obj);
	   update_count++)
	{
	  if (one_done == 0)
	    {
	      update_group_start();
	      one_done = 1;
	    }


	  fts_client_send_property(obj, property);
	}
    }
  else
    period_count++;

  if (one_done)
    update_group_end();

}

/* Sync empty the change fifo; this is used to introduce a sequence
   point between the client and the server, with the sync protocol,
   under request from the server 
   */

void
fts_client_updates_sync(void)
{
  fts_symbol_t property;
  fts_object_t *obj;
  int one_done = 0;

  while (fts_object_get_next_change(&property, &obj))
    {
      if (one_done == 0)
	{
	  update_group_start();
	  one_done = 1;
	}

      fts_client_send_property(obj, property);
    }

  if (one_done)
    update_group_end();
}


void
fts_updates_set_updates_per_ticks(int upt)
{
  if (upt > 0)
    fts_updates_per_ticks = upt;
}

void
fts_updates_set_update_period(int upt)
{
  if (upt > 0)
    fts_update_period = upt;
}





