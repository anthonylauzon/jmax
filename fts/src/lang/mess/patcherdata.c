#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

extern void fts_client_updates_sync(void);

/* Remote call codes */

#define REMOTE_REDEFINE_START    0
#define REMOTE_REDEFINED         1
#define REMOTE_SET_CONTAINER     2
#define REMOTE_ADD_OBJECT        3
#define REMOTE_ADD_CONNECTION    4
#define REMOTE_START_UPDATES     5
#define REMOTE_STOP_UPDATES      6
#define REMOTE_UPDATE            7
#define REMOTE_SET_WX            8
#define REMOTE_SET_WY            9
#define REMOTE_SET_WW            10
#define REMOTE_SET_WH            11

static fts_heap_t *patcher_data_heap;

static fts_data_class_t *fts_patcher_data_class = 0;

struct fts_patcher_data
{
  fts_data_t dataobj;
  fts_patcher_t *patcher;
};


fts_patcher_data_t *fts_patcher_data_new(fts_patcher_t *patcher)
{
  fts_patcher_data_t *data;

  data = (fts_patcher_data_t *) fts_heap_alloc(patcher_data_heap);

  data->patcher = patcher;

  fts_data_init((fts_data_t *) data, fts_patcher_data_class);

  return data;
}


void fts_patcher_data_free( fts_patcher_data_t *data)
{
  data->patcher = 0;
  fts_heap_free((char *)data, patcher_data_heap);
}


/*
 * Content handling functions 
 */


void fts_patcher_data_redefine(fts_patcher_data_t *d)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;
  fts_object_t *p;
  fts_atom_t a;

  /* First, reset the container */

  fts_data_remote_call((fts_data_t *)d, REMOTE_REDEFINE_START, 0, 0);

  /* Send the window geometric property if any */

  fts_object_get_prop((fts_object_t *)this->patcher, fts_s_wx, &a);
  if (fts_is_int(&a))
    fts_data_remote_call((fts_data_t *)d, REMOTE_SET_WX, 1, &a);

  fts_object_get_prop((fts_object_t *)this->patcher, fts_s_wy, &a);
  if (fts_is_int(&a))
    fts_data_remote_call((fts_data_t *)d, REMOTE_SET_WY, 1, &a);

  fts_object_get_prop((fts_object_t *)this->patcher, fts_s_ww, &a);
  if (fts_is_int(&a))
    fts_data_remote_call((fts_data_t *)d, REMOTE_SET_WW, 1, &a);

  fts_object_get_prop((fts_object_t *)this->patcher, fts_s_wh, &a);
  if (fts_is_int(&a))
    fts_data_remote_call((fts_data_t *)d, REMOTE_SET_WH, 1, &a);

  /* Then set the container */

  fts_data_start_remote_call((fts_data_t *)d, REMOTE_SET_CONTAINER);
  fts_client_mess_add_object((fts_object_t *) this->patcher);
  fts_data_end_remote_call();

  /* For each object and each connection,
     if they are already uploaded (like by a find operation),
     then add the manually  to the container, otherwise just
     upload them (they will be added in the client); this is done
     to reduce the client/server traffic during patcher opening
     */

  for (p = this->patcher->objects; p ; p = p->next_in_patcher)
    if (p->id == FTS_NO_ID)
      fts_client_upload_object(p);
    else
      {
	fts_data_start_remote_call((fts_data_t *)d, REMOTE_ADD_OBJECT);
	fts_client_mess_add_object(p);
	fts_data_end_remote_call();
      }

  for (p = this->patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    if (c->id == FTS_NO_ID)
	      fts_client_upload_connection(c);
	    else
	      {
		fts_data_start_remote_call((fts_data_t *)d, REMOTE_ADD_CONNECTION);
		fts_client_mess_add_connection(c);
		fts_data_end_remote_call();
	      }
	}
    }

  /* Third, empty the update list (i.e. do the effect of an FTS sync
     without the handshacking), otherwise the editor will open before
     all the uploaded property are sent ! */

  fts_client_updates_sync();

  /*
   * Then tell the patcher data the redefinition is completed, and can call
   * the various listeners
   */

  fts_data_remote_call((fts_data_t *)d, REMOTE_REDEFINED, 0, 0);
}

/*
 * The export function
 */

static void fts_patcher_data_export_fun(fts_data_t *d)
{
  fts_patcher_data_redefine((fts_patcher_data_t *)d);
}

/*
 * The remote functions
 */

static void fts_patcher_data_start_updates( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_message_send((fts_object_t *) this->patcher, fts_SystemInlet, fts_s_open, 0, 0);
}

static void fts_patcher_data_stop_updates( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_message_send((fts_object_t *) this->patcher, fts_SystemInlet, fts_s_close, 0, 0);  
}

/* Handle geometric properties */

static void fts_patcher_data_set_wx( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_object_put_prop((fts_object_t *)this->patcher, fts_s_wx, at);
}

static void fts_patcher_data_set_wy( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_object_put_prop((fts_object_t *)this->patcher, fts_s_wy, at);
}


static void fts_patcher_data_set_wh( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_object_put_prop((fts_object_t *)this->patcher, fts_s_wh, at);
}

static void fts_patcher_data_set_ww( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;

  fts_object_put_prop((fts_object_t *)this->patcher, fts_s_ww, at);
}

/**
 * This function send all the objects and connections in the patcher that have not
 * yet been uploaded; it is usefull after paste operations.
 */

  
static void fts_patcher_data_update( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_patcher_data_t *this = (fts_patcher_data_t *)d;
  fts_object_t *p;

  /* upload all the not uploaded objects */

  for (p = this->patcher->objects; p ; p = p->next_in_patcher)
    if (p->id == FTS_NO_ID)
      fts_client_upload_object(p);

  /* For each object, for each outlet, upload all the not uploaded connections */

  for (p = this->patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    if (c->id == FTS_NO_ID)
	      fts_client_upload_connection(c);
	}
    }
}


/********************************************************************/
/*                                                                  */
/*            INIT_DATA functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void fts_patcher_data_config(void)
{
  patcher_data_heap          = fts_heap_new(sizeof( fts_patcher_data_t));

  fts_patcher_data_class = fts_data_class_new( fts_new_symbol( "patcher_data"));
  fts_data_class_define_export_function(fts_patcher_data_class, fts_patcher_data_export_fun);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_START_UPDATES, fts_patcher_data_start_updates);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_STOP_UPDATES, fts_patcher_data_stop_updates);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_UPDATE, fts_patcher_data_update);

  fts_data_class_define_function(fts_patcher_data_class, REMOTE_SET_WX, fts_patcher_data_set_wx);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_SET_WY, fts_patcher_data_set_wy);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_SET_WW, fts_patcher_data_set_ww);
  fts_data_class_define_function(fts_patcher_data_class, REMOTE_SET_WH, fts_patcher_data_set_wh);
}









