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
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jack/jack.h>
#include <fts/fts.h>
#include <fts/thread.h>
#include <sys/time.h>

#include "jackaudio.h"

#define DEFAULT_DELAY_MS 100
#define DEFAULT_THREAD_FIFO_SIZE 40

#define FTS_JACK_CREATE_THREAD 1

/* #define JACK_AUDIO_MANAGER_DEBUG 1 */


#define JACK_AUDIOMANAGER_READY_STATE 0
#define JACK_AUDIOMANAGER_CLOSE_STATE 1
#define JACK_AUDIOMANAGER_OPEN_STATE 2
#define JACK_AUDIOMANAGER_CONNECT_STATE 3

typedef struct _jackaudiomanager_thread_t
{
  fts_object_t o;

  /* run fifo */
  fts_fifo_t run_fifo;
  void* run_buffer;
  int run_size;

  int run_state;
  int open_state;
  int close_state;

} jackaudiomanager_thread_t;

typedef struct _jackaudiomanager_t
{
/*   fts_audiomanager_t manager; */
  fts_object_t o;

  fts_object_t* jack_communication;

  fts_thread_worker_t* run_manager;

} jackaudiomanager_t;




static int nb_jack_audio_port = 0;

static int jack_process_nframes = 0;
static int jack_process_consumed = 0;
/**************************************************
 *
 * jack jmax client 
 *
 */
static fts_symbol_t s_default_client_name;


jack_client_t* manager_jack_client = NULL;
static int jack_count = 0;


static fts_class_t* jackaudiomanager_type = NULL;
static fts_object_t* jackaudiomanager_object = NULL;


static fts_symbol_t s_running;
static fts_symbol_t s_activate;
static fts_symbol_t s_deactivate;
static fts_symbol_t s_connect;

/* non connected jMax jack port name */
static fts_symbol_t s_jmax_jack_input;
static fts_symbol_t s_jmax_jack_output;


static fts_hashtable_t jack_port_input_ht;
static fts_hashtable_t jack_port_output_ht;

static int pipedes[2];


static fts_hashtable_t jackaudiomanager_table;

int get_jack_process_nframes()
{
  return jack_process_nframes;
}

int get_jack_process_consumed()
{
  return jack_process_consumed;
}

fts_object_t* jackaudiomanager_get_manager_object()
{
  return jackaudiomanager_object;
}

static void jackaudiomanager_scan_ports(fts_hashtable_t* ht, int flags);

static void spost_hashtable(fts_hashtable_t* ht, fts_bytestream_t* bytestream)
{
  fts_iterator_t keys, values;
  
  fts_hashtable_get_keys(ht, &keys);
  fts_hashtable_get_values(ht, &values);
  while (fts_iterator_has_more(&keys))
  {
    fts_atom_t k,v;
    
    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);
    fts_spost(bytestream, "%s\n", fts_get_symbol(&k));
  }
}

/**************************************************/
/*****              JACK CALLBACKS            *****/
/**************************************************/
/* JACK process callback */
static
int jackaudiomanager_process(jack_nframes_t nframes, void* arg)
{
  int n = 0;
  /* get number of samples of a FTS tick */
  int samples_per_tick = fts_dsp_get_tick_size();
  jack_process_nframes = nframes;
  jack_process_consumed = 0;
    
  /* TODO: 
     Need to be fix if (nframes % samples_per_tick != 0) 

     Case 1: nframes < samples_per_tick
       
     Case 2: nframes > samples_per_tick

  */

  for (n = 0; n < nframes; n += samples_per_tick)
  {
    /* Run scheduler */
    fts_sched_run_one_tick();
    jack_process_consumed += samples_per_tick;
  }

  return 0;
}


static void
jackaudiomanager_on_shutdown(void* arg)
{
  fts_iterator_t keys, values;
  int dummy = 0;

  fts_log("[jackaudiomanager] jackd is shutdown \n");
  post("[jackaudiomanager] jackd is shutdown \n");
  fts_hashtable_get_keys(&jack_port_input_ht, &keys);
  fts_hashtable_get_values(&jack_port_input_ht, &values);
  while (fts_iterator_has_more(&keys))
  {
    fts_atom_t k,v;
    fts_object_t* port;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);

    port = fts_get_object(&v);
    fts_log("[jackaudiomanager] remove port %s from input hashtable\n", fts_get_symbol(&k));
    fts_audiomanager_remove_port(fts_get_symbol(&k));
    /* check if port was opened */
    
    fts_object_release(port);
  }

  fts_hashtable_get_keys(&jack_port_output_ht, &keys);
  fts_hashtable_get_values(&jack_port_output_ht, &values);
  while (fts_iterator_has_more(&keys))
  {
    fts_atom_t k,v;
    fts_object_t* port;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);

    port = fts_get_object(&v);
    fts_log("[jackaudiomanager] remove port %s from output hashtable \n", fts_get_symbol(&k));
    fts_audiomanager_remove_port(fts_get_symbol(&k));
    /* check if port was opened */
    fts_object_release(port);
  }
 
  /* restart FTS scheduler if needed */
  write(pipedes[1], &dummy, sizeof(int));

  /* @@@@@ DO MEMORY CLEAN ... @@@@@ */
}

/* jack port registration callback */
void jackaudiomanager_port_registration_callback(jack_port_id_t port_id, int n, void* arg)
{
  jack_port_t* cur_port = jack_port_by_id(manager_jack_client, port_id);
  fts_symbol_t port_name = fts_new_symbol(jack_port_name(cur_port));
  fts_atom_t k, v;
  fts_object_t* audioconfig;
  fts_bytestream_t* bytestream = fts_post_get_stream(0,NULL);

  fts_log("[jackaudiomanager] port registration callback port name: %s\n", port_name);
  post("[jackaudiomanager] registration callback, port name: %s, flag:%d\n", port_name, n);

  if (0 == n)
  {
    /* check if port is a jMax port */
    if (1 == jack_port_is_mine(manager_jack_client, cur_port))
    {
      /* do nothing */
      fts_log("[jackaudiomanager] port %s (id:%d) is mine \n", port_name, port_id);
      return;
    }    
    post("[jackaudiomanager] unregistered port : %s\n", port_name);

    /* look in hashtable to remove port */
    fts_set_symbol(&k, port_name);
    if (fts_hashtable_get(&jack_port_input_ht, &k, &v))
    {
      fts_object_t* port = fts_get_object(&v);
      fts_hashtable_remove(&jack_port_input_ht, &k);
      fts_log("[jackaudiomanager] remove %s from input hashtable \n", port_name);
      fts_audiomanager_remove_port(port_name);
    }
    if (fts_hashtable_get(&jack_port_output_ht, &k, &v))
    {
      fts_object_t* port = fts_get_object(&v);
      fts_hashtable_remove(&jack_port_output_ht, &k);
      fts_log("[jackaudiomanager] remove %s from output hashtable \n", port_name);
      fts_audiomanager_remove_port(port_name);
    }
  }
  else
  {
    /* update jack port hashtable */
    post("[jackaudiomanager] registered port : %s\n", port_name);
    jackaudiomanager_scan_ports(&jack_port_input_ht, JackPortIsOutput);
    jackaudiomanager_scan_ports(&jack_port_output_ht, JackPortIsInput);    
  }
  
  /* @@@@@@ tell audio config to upload current configuration .... ?  @@@@@ */

  /* @@@@@@ debug print @@@@@@@@ */
  fts_spost(bytestream, "[jackaudiomanager] Input Ports\n");
  spost_hashtable(&jack_port_input_ht, bytestream);

  fts_spost(bytestream, "[jackaudiomanager] Output Ports\n");
  spost_hashtable(&jack_port_output_ht, bytestream);
  
}
/**************************************************/
/*****         END OF JACK CALLBACKS          *****/
/**************************************************/



static
int create_jack_manager_client()
{
  if (NULL == manager_jack_client)
  {
    jack_count = 0;
    manager_jack_client = jack_client_new("jMax");
    if (NULL == manager_jack_client)
    {
      fts_log("[jackaudiomanager] cannot create jMax manager jack client\n");      
      return -1;
    }
    else
    {
      fts_log("[jackaudiomanager] jMax manager jack client created \n");
    }
    /* set shutdown callback */
    jack_on_shutdown(manager_jack_client, jackaudiomanager_on_shutdown, NULL);
  }
  ++jack_count;
  return 0;
}

static void
destroy_jack_manager_client()
{
  --jack_count;
  if (NULL != manager_jack_client && (jack_count <= 0))
  {
    jack_client_close(manager_jack_client);
  }
  jack_count = 0;
}

jack_client_t* jackaudiomanager_get_jack_client()
{
  if (NULL == manager_jack_client)
  {
    fts_log("[jackaudiomanager] create_jack_manager_client (set_jack_client)\n"); 
    create_jack_manager_client();
  }
  return manager_jack_client;
}

/**************************************************
 * jackaudiomanager_connect
 *
 */
fts_class_t* jackaudiomanager_thread_type = NULL;

static void
jackaudiomanager_thread_run(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static jackaudioport_t* port;
  static fts_symbol_t port_name;
  static fts_symbol_t port_flags;
  static int is_jack_client_active = 0;

  static fts_symbol_t tmp_symbol;

  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;

  while (fts_fifo_read_level(&self->run_fifo) >= sizeof(fts_atom_t))
  {
    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&self->run_fifo);
    fts_fifo_incr_read(&self->run_fifo, sizeof(fts_atom_t));
    
    switch(self->run_state)
    {
    case JACK_AUDIOMANAGER_READY_STATE:
      if (!fts_is_symbol(atom))
      {
	fts_log("[jackaudiomanager_thread_run] error in READY STATE (symbol wanted) \n");
	return;
      }
      tmp_symbol = fts_get_symbol(atom);
      if (fts_s_close == tmp_symbol)
      {
	self->run_state = JACK_AUDIOMANAGER_CLOSE_STATE;
      }
      if (fts_s_open == tmp_symbol)
      {
	self->run_state = JACK_AUDIOMANAGER_OPEN_STATE;
      }
      if (s_connect == tmp_symbol)
      {
	self->run_state = JACK_AUDIOMANAGER_CONNECT_STATE;
      }
      if (s_activate == tmp_symbol)
      {
	/* jack client will be activate next tick */
	fts_sched_add(jackaudiomanager_object, FTS_SCHED_ALWAYS);
	/* return so fifo will be empty at next call */
	return;
      }
      if (s_deactivate == tmp_symbol)
      {
	int val = 0;
	val = jack_deactivate(manager_jack_client);
	if (0 != val)
	{
	  fts_log("[jackaudiomanager_thread] error while deactivating client .... \n");
	}
	else
	{
	  fts_log("[jackaudiomanager_thread] client deactivated ... \n");
	}
	is_jack_client_active = 0;
	/* we have deactivated jack client, so we need to 
	   restart FTS scheduler */
	write(pipedes[1], &val, sizeof(int));
	/* return so fifo will be empty at next call */
	return;
      }
      break;

    case JACK_AUDIOMANAGER_CLOSE_STATE:
      switch(self->close_state)
      {
      case 0:
	if (!fts_is_object(atom))
	{
	  fts_log("[jackaudiomanger_thread_close] not an object for first state \n");
	}
	port = (jackaudioport_t*)fts_get_object(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
	fts_log("[jackaudiomanager_thread_close] receive audio port object \n");
#endif /* JACK_AUDIO_MANAGER_DEBUG */
	self->close_state++;
	break;
      case 1:
	port_flags = fts_get_symbol(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
	fts_log("[jackaudiomanager_thread_close] receive port flag %s \n", port_flags);
#endif /* JACK_AUDIO_MANAGER_DEBUG */
	if (fts_s_input == port_flags)
	{
	  if (NULL != port->input_port)
	  {
	    /* call jack_port_disconnect */
	    if (-1 != jack_port_disconnect(manager_jack_client, port->input_port))
	    {
	      fts_log("[jackaudiomanager] disconnect %s to %s \n", 
		      port->port_name, 
		      jack_port_name(port->input_port));
	    }
	    else
	    {
	      fts_log("[jackaudiomanager] error while disconnection %s to %s \n",
		      port->port_name,
		      jack_port_name(port->input_port));
	    }

	    /* call jack_port_unregister */
	    if (-1 != jack_port_unregister(manager_jack_client, port->input_port))
	    {
	      fts_log("[jackaudiomanager] jack input port unregistered \n");
	    }
	  
	    port->input_port = NULL;
	  }
	}
	else
	{
	  if (NULL != port->output_port)
	  {
	    /* call jack_disconnect */
	    if (-1 != jack_port_disconnect(manager_jack_client, port->output_port))
	    {
	      fts_log("[jackaudiomanager] disconnect %s to %s \n", 
		      jack_port_name(port->output_port), 
		      port->port_name);
	    }
	    else
	    {
	      fts_log("[jackaudiomanager] error while disconnection %s to %s \n",
		      jack_port_name(port->output_port),
		      port->port_name);
	    }

	    /* call jack_port_unregister */
	    if (-1 != jack_port_unregister(manager_jack_client, port->output_port))
	    {
	      fts_log("[jackaudiomanager] jack output port unregistered \n");
	    }
	  
	    port->output_port = NULL;
	  }
	}
	self->close_state = 0;
	self->run_state = JACK_AUDIOMANAGER_READY_STATE;
	break;
      
      default:
	/* unreachable case ... */
	fts_log("[jackaudiomanager] unreachable case in  STATE %d\n", self->close_state);	
	break;
      }    
      break;

    case JACK_AUDIOMANAGER_OPEN_STATE:
      switch(self->open_state)
      {
      case 0:
	port = (jackaudioport_t*)fts_get_object(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
	fts_log("[jackaudiomanager_thread_open] receive audio port object \n");
#endif /* JACK_AUDIO_MANAGER_DEBUG */
	self->open_state++;
	break;
      case 1:
	port_name = fts_get_symbol(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
	fts_log("[jackaudiomanager_thread_open] receive port name %s \n", port_name);
#endif /* JACK_AUDIO_MANAGER_DEBUG */
	self->open_state++;
	break;
      case 2:
	port_flags = fts_get_symbol(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
	fts_log("[jackaudiomanager_thread_open] receive port flag %s \n", port_flags);
#endif /* JACK_AUDIO_MANAGER_DEBUG */
	if (fts_s_input == port_flags)
	{
	  /* call jack_port_register */
	  port->input_port = jack_port_register(manager_jack_client, 
						port_name, 
						JACK_DEFAULT_AUDIO_TYPE, 
						JackPortIsInput, 
						0);
	  if (NULL != port->input_port)
	  {
	    fts_log("[jackaudiomanager] jack input port %s registered \n", port_name);
	    if (is_jack_client_active)
	    {
	      if (port->port_name != s_jmax_jack_input)
	      {
		/* call jack_connect if jack client is active */
		jack_connect(manager_jack_client, port->port_name, jack_port_name(port->input_port));
		fts_log("[jackaudiomanager] connect %s to %s \n", 
			port->port_name, 
			jack_port_name(port->input_port));
	      }
	      else
	      {
		fts_log("[jackaudiomanager] open %s\n", s_jmax_jack_input);
	      }
	      fts_audioport_set_open((fts_audioport_t*)port, FTS_AUDIO_INPUT);
	    }
	  }
	}
	else
	{
	  /* call jack_port_register */
	  port->output_port = jack_port_register(manager_jack_client, 
						 port_name, 
						 JACK_DEFAULT_AUDIO_TYPE, 
						 JackPortIsOutput, 
						 0);
	  if (port->output_port != NULL)
	  {
	    fts_log("[jackaudiomanager] jack output port %s registered \n", port_name);
	    if (is_jack_client_active)
	    {
	      if (port->port_name != s_jmax_jack_output)
	      {
		/* call jack_connect if jack is active */
		jack_connect(manager_jack_client, jack_port_name(port->output_port), port->port_name);	
		fts_log("[jackaudiomanager] connect %s to %s \n", 
			jack_port_name(port->output_port), 
			port->port_name);
	      }
	      else
	      {
		fts_log("[jackaudiomanager] open %s\n", s_jmax_jack_output);
	      }
	      fts_audioport_set_open((fts_audioport_t*)port, FTS_AUDIO_OUTPUT);
	    }
	  }
	}
	
	self->open_state = 0;
	self->run_state = JACK_AUDIOMANAGER_READY_STATE;
	break;

      default:
	/* unreachable case ... */
	fts_log("[jackaudiomanager] unreachable case in OPEN STATE %d\n", self->open_state);	
	break;
      }
      break;

    case JACK_AUDIOMANAGER_CONNECT_STATE:
      /* jack has just been activate and we connect the registred port */
      is_jack_client_active = 1;
      if (!fts_is_object(atom))
      {
	fts_log("[jackaudiomanger_thread_connect] not an object\n");
      }
      port = (jackaudioport_t*)fts_get_object(atom);
#ifdef JACK_AUDIO_MANAGER_DEBUG
      fts_log("[jackaudiomanager_thread_connect] receive audio port object\n");
#endif /* JACK_AUDIO_MANAGER_DEBUG */
      if (NULL != port->input_port)
      {   
	if (port->port_name != s_jmax_jack_input)
	{
	  /* call jack_connect */
	  jack_connect(manager_jack_client, port->port_name, jack_port_name(port->input_port));
	  fts_log("[jackaudiomanager] connect %s to %s \n", 
		  port->port_name, 
		  jack_port_name(port->input_port));
	}
	fts_audioport_set_open((fts_audioport_t*)port, FTS_AUDIO_INPUT);
      }
      else if (port->output_port != NULL)
      {
	if (port->port_name != s_jmax_jack_output)
	{
	  /* call jack_connect */
	  jack_connect(manager_jack_client, jack_port_name(port->output_port), port->port_name);	
	  fts_log("[jackaudiomanager] connect %s to %s \n", 
		  jack_port_name(port->output_port), 
		  port->port_name);
	}
	fts_audioport_set_open((fts_audioport_t*)port, FTS_AUDIO_OUTPUT);
      }

      self->run_state = JACK_AUDIOMANAGER_READY_STATE;
      break;

    default: 
      /* unreachable case */
      break;
    }

  } /* while */
}

static void
jackaudiomanager_thread_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;
  int bytes = 0;
  int size = DEFAULT_THREAD_FIFO_SIZE;
  fts_atom_t* atoms;
  int i;

  /* run */
  bytes = size * sizeof(fts_atom_t);
  self->run_buffer = fts_malloc(bytes);
  self->run_size = size;

  fts_fifo_init(&self->run_fifo, self->run_buffer, bytes);
  atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->run_fifo);
  for (i = 0; i < size; ++i)
  {
    fts_set_void(atoms + i);
  }
  self->run_state = JACK_AUDIOMANAGER_READY_STATE;

  self->open_state = 0;
  self->close_state = 0;  
}

static void
jackaudiomanager_thread_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;

  fts_free(self->run_buffer);
}

static void
jackaudiomanager_thread_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_thread_t), 
		 jackaudiomanager_thread_init, jackaudiomanager_thread_delete);
  fts_class_message_varargs(cl, s_running, jackaudiomanager_thread_run);
}


/**************************************************
 *
 * jackaudiomanager
 *
 */


static fts_thread_worker_t*
create_thread(fts_object_t* obj, fts_symbol_t mess)
{
  fts_thread_worker_t* worker = fts_malloc(sizeof(fts_thread_worker_t));
  fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));

  thread_job->object = obj;
  
  thread_job->method = fts_class_get_method_varargs(fts_object_get_class(thread_job->object),
						    mess);
  
  if (0 == thread_job->method)
    {
	fts_free(thread_job);
	fts_free(worker);
	return NULL;
    }
    
    thread_job->ac = 0;
    thread_job->at = NULL;
    thread_job->symbol = mess;
    
    thread_job->delay_ms = DEFAULT_DELAY_MS;
    worker->thread_function = thread_job;

    fts_thread_manager_create_thread(worker);
    return worker;
}


static void
delete_thread(fts_thread_worker_t* worker)
{
  fts_thread_manager_cancel_thread(worker);
  fts_free(worker->thread_function);
  fts_free(worker);
}

static void
create_run_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = create_thread(self->jack_communication, s_running);
  if (NULL != worker)
  {
    self->run_manager = worker;
  }
  else
  {
    fts_object_error((fts_object_t*)self, "Cannot create connect thread \n");
  }
}

static void
delete_run_thread(jackaudiomanager_t* self)
{
  delete_thread(self->run_manager);
}


static int
add_object_to_fifo(fts_fifo_t* fifo, fts_object_t* obj)
{
  fts_atom_t new_object;
  fts_atom_t* write_atom;

  if (fts_fifo_write_level(fifo) < sizeof(fts_atom_t))
  {
    fts_log("[jackaudiomanager] add_symbol_to_fifo: not enough space to add symbol in fifo \n");
    return -1;
  }
  
  fts_set_object(&new_object, obj);
  write_atom = (fts_atom_t*)fts_fifo_write_pointer(fifo);
  fts_atom_assign(write_atom, &new_object);
  fts_fifo_incr_write(fifo, sizeof(fts_atom_t));

  return 0;
}

static int
add_symbol_to_fifo(fts_fifo_t* fifo, fts_symbol_t s)
{
  fts_atom_t new_symbol;
  fts_atom_t* write_atom;

  if (fts_fifo_write_level(fifo) < sizeof(fts_atom_t))
  {
    fts_log("[jackaudiomanager] add_symbol_to_fifo: not enough space to add symbol in fifo \n");
    return -1;
  }
  
  fts_set_symbol(&new_symbol, s);
  write_atom = (fts_atom_t*)fts_fifo_write_pointer(fifo);
  fts_atom_assign(write_atom, &new_symbol);
  fts_fifo_incr_write(fifo, sizeof(fts_atom_t));

  return 0;
}

static void
jackaudiomanager_scan_ports(fts_hashtable_t* ht, int flags)
{
  const char** ports;
  int i;
  fts_symbol_t cur_sym;
  fts_audioport_t* port;
  fts_atom_t at[2];

  ports = jack_get_ports(manager_jack_client, 
			 NULL, /* no pattern on port name NULL */
			 NULL, /* no pattern on type */
			 flags);

  i = 0;
  while(ports && (NULL != ports[i]))
  {
    fts_atom_t k,a;
    cur_sym = fts_new_symbol(ports[i]);
    fts_set_symbol(&k, cur_sym);
    /* add port to hashtable */
    if (!fts_hashtable_get(ht, &k, &a))
    {
      fts_set_int(at, flags);
      fts_set_symbol(at + 1, fts_new_symbol(ports[i]));
      port = (fts_audioport_t*)fts_object_create(jackaudioport_type, 2, at);
      fts_object_refer((fts_object_t*)port);
      fts_audiomanager_put_port(cur_sym, port);
      fts_set_object(&a,(fts_object_t*)port);
      fts_hashtable_put(ht, &k, &a);
      fts_log("[jackaudiomanager] append symbol : %s with flags: %d\n", ports[i], flags);      
    }
    ++i;
  }

}


static
void jackaudiomanager_set_jack_client(jackaudiomanager_t* self)
{
  if (NULL == manager_jack_client)
  {
    fts_log("[jackaudiomanager] create_jack_manager_client (set_jack_client)\n"); 
    create_jack_manager_client();
  }
}

static 
void jackaudiomanager_unset_jack_client(jackaudiomanager_t* self)
{
    destroy_jack_manager_client();
}

void
jackaudiomanager_open_port(fts_object_t* port, fts_symbol_t port_name, fts_symbol_t port_flags)
{
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* run_fifo;
  jackaudiomanager_t* self = (jackaudiomanager_t*)jackaudiomanager_get_manager_object();

  fts_log("[jackaudiomanager] open a new jack audio port %p\n", port);

  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  run_fifo = &manager_object->run_fifo;


  /* Check if a jackaudioport is alreay open */
  if (0 == nb_jack_audio_port)
  {    
    /* 
       set process callback
       activate jack client
       halt scheduler 
       DONE in jackaudiomanager_halt
    */
    /* call jackaudiomanager_halt in next tick */
    fts_log("[jackaudiomanager] add jackaudiomanager callback  \n");
    if (0 != add_symbol_to_fifo(run_fifo, s_activate))
    {
      fts_log("[jackaudiomanager] cannot add activate symbol in run FIFO \n");
    }
    fts_log("[jackaudiomanager] activate symbol added in FIFO \n");
/*     fts_sched_add(jackaudiomanager_object, FTS_SCHED_ALWAYS); */
  }

  if (0 != add_symbol_to_fifo(run_fifo, fts_s_open))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add open symbol in run FIFO \n");
    return;    
  }
  if (0 != add_object_to_fifo(run_fifo, port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add port object in run FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(run_fifo, port_name))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add src port symbol in run FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(run_fifo, port_flags))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add dest port symbol in run FIFO \n");
    return;
  }
  ++nb_jack_audio_port;
  fts_log("[jackaudiomanager] jackaudiomanager_open_port, nb_jack_audio_port=%d\n", nb_jack_audio_port);
}

void
jackaudiomanager_close_port(fts_object_t* port, fts_symbol_t flag)
{
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* run_fifo;
  jackaudiomanager_t* self = (jackaudiomanager_t*)jackaudiomanager_get_manager_object();
  
  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  run_fifo = &manager_object->run_fifo;

  if (0 != add_symbol_to_fifo(run_fifo, fts_s_close))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add close symbol in run FIFO \n");
    return;    
  }
  if (0 != add_object_to_fifo(run_fifo, port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add audioport object in run FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(run_fifo, flag))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add disconnection flag in run FIFO \n");
    return;
  }

  /* decrement jackaudioport counter */
  fts_log("[jackaudiomanager] jackaudiomanager_close_port \n");
  fts_log("[jackaudiomanager] nb_jack_audio_port = %d\n", nb_jack_audio_port);
  --nb_jack_audio_port;
  if (0 == nb_jack_audio_port)
  {
    fts_log("[jackaudiomanager] no more jack audio port opened \n");
    if (0 != add_symbol_to_fifo(run_fifo, s_deactivate))
    {
      fts_log("[jackaudiomanager] cannot add deactivate symbol in run FIFO \n");
    }
  }
}

static void
jackaudiomanager_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self;
  fts_bytestream_t* bytestream = fts_post_get_stream(ac, at);

  fts_spost(bytestream, "[jackaudiomanager] Input Ports\n");
  spost_hashtable(&jack_port_input_ht, bytestream);

  fts_spost(bytestream, "[jackaudiomanager] Output Ports\n");
  spost_hashtable(&jack_port_output_ht, bytestream);
}

static void
jackaudiomanager_sched_listener(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* run_fifo;

  if (!fts_sched_is_running())
  {
    /* add src and dest symbol to thread fifo */
    manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
    run_fifo = &manager_object->run_fifo;
    if (0 != add_symbol_to_fifo(run_fifo, s_deactivate))
    {
      fts_log("[jackaudiomanager_sched_listener] cannot add deactivate symbol in run FIFO \n");
    }
  }
}

/* This function is used to remove object from scheduler and to activate JACK client */
static void
jackaudiomanager_halt(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fd_set rfds;
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jack_client_t* client = jackaudiomanager_get_jack_client();
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* run_fifo;
  fts_iterator_t keys, values;

  /* Remove object of FTS scheduler */
  fts_sched_remove(o);
  fts_log("[jackaudiomanager] jackaudiomanager removed from scheduler \n");

  /* JACK client process callback setting */
  jack_set_process_callback(client,
			    jackaudiomanager_process, /* callback function */
			    (void*)self);          /* we need to have our object
						      in our callback function
						   */
  fts_log("[jackaudiomanager] set jackaudiomanager process callback \n");
    
  fts_dsp_set_sample_rate((double)jack_get_sample_rate(client));
  /* Activate jack client */    
  if (jack_activate(client) == -1)
  {
    fts_log("[jackaudiomanager] cannot activate JACK client \n");
    fts_object_error(o, "cannot activate JACK client \n");
    return;
  }

  /* connect registred input/output ports */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  run_fifo = &manager_object->run_fifo;
  /* input ports */
  fts_hashtable_get_keys(&jack_port_input_ht, &keys);
  fts_hashtable_get_values(&jack_port_input_ht, &values);
  while (fts_iterator_has_more(&keys))
  {
    fts_atom_t k,v;
    fts_object_t* port;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);

    port = fts_get_object(&v);
    fts_log("[jackaudiomanager] add connection symbol in run FIFO \n");
    if (0 != add_symbol_to_fifo(run_fifo, s_connect))
    {
      /* error occur */
      fts_log("[jackaudiomanager] cannot add connection symbol in run FIFO \n");
      continue;
    }
    if (0 != add_object_to_fifo(run_fifo, port))
    {
      /* error occur */
      fts_log("[jackaudiomanager] cannot add port object in run FIFO \n");
      continue;
    }
  }
  /* output ports */
  fts_hashtable_get_keys(&jack_port_output_ht, &keys);
  fts_hashtable_get_values(&jack_port_output_ht, &values);
  while (fts_iterator_has_more(&keys))
  {
    fts_atom_t k,v;
    fts_object_t* port;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &v);

    port = fts_get_object(&v);
    if (0 != add_symbol_to_fifo(run_fifo, s_connect))
    {
      /* error occur */
      fts_log("[jackaudiomanager] cannot add connection symbol in run FIFO \n");
      continue;
    }
    if (0 != add_object_to_fifo(run_fifo, port))
    {
      /* error occur */
      fts_log("[jackaudiomanager] cannot add port object in run FIFO \n");
      continue;
    }
  }

  fts_log("[jackaudiomanager] STOP FTS SCHEDULER \n");
  /* Stop FTS scheduler */
  FD_ZERO(&rfds);
  FD_SET(pipedes[0], &rfds);

  /* check return value of select */
  if (select(pipedes[0] + 1, &rfds, NULL, NULL, NULL) < 0)
  {
    fts_log("[jackaudiomanager] select failed \n");
  }

  if (FD_ISSET(pipedes[0], &rfds))
  {
    int val;
    fts_log("[jackaudiomanager] trying to restart FTS scheduler \n");
    read(pipedes[0], &val, sizeof(int));
  }
  fts_log("[jackaudiomanager] RESTART FTS SCHEDULER \n");
}

static void
jackaudiomanager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;

  /* check if a manager already exist ? */
  if (NULL != jackaudiomanager_object)
  {
    o = jackaudiomanager_object;
    return;
  }
  jackaudiomanager_set_jack_client(self);


#ifdef FTS_JACK_CREATE_THREAD
  /* start the fts thread manager */
  fts_thread_manager_start();

  /* create jack communication object */
  self->jack_communication = fts_object_create(jackaudiomanager_thread_type, 0, 0);

  /* create connections thread */
  create_run_thread(self);
#endif

  /* port_registration_callback */
#ifdef JACK_AUDIOMANAGER_DEBUG
  jack_set_port_registration_callback(manager_jack_client,
				      jackaudiomanager_port_registration_callback,
				      NULL);
#endif /* JACK_AUDIOMANAGER_DEBUG */
  fts_sched_running_add_listener(o, jackaudiomanager_sched_listener);
}

static void
jackaudiomanager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* run_fifo;

  fts_sched_running_remove_listener(o);
#ifdef JACK_AUDIO_MANAGER_DEBUG
  post("jackaudiomanager_delete called !!!!! \n");
#endif /* JACK_AUDIO_MANAGER_DEBUG */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  run_fifo = &manager_object->run_fifo;
  if (0 != add_symbol_to_fifo(run_fifo, s_deactivate))
  {
    fts_log("[jackaudiomanager] cannot add deactivate symbol in run FIFO \n");
  }

#ifdef FTS_JACK_CREATE_THREAD
  delete_run_thread(self);
#endif

  jackaudiomanager_unset_jack_client(self);  
}

static void 
jackaudiomanager_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_t), jackaudiomanager_init, jackaudiomanager_delete);

  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, jackaudiomanager_print);

  fts_class_message_varargs(cl, fts_s_sched_ready, jackaudiomanager_halt);
}


void
jackaudiomanager_at_exit(void)
{
  fts_object_destroy(jackaudiomanager_object);
}

/***********************************************************************
 *
 * Config
 *
 */
void jackaudiomanager_config( void)
{
  fts_symbol_t s = fts_new_symbol("jackaudiomanager");
  fts_object_t* o;
  fts_class_t* jam;

  fts_atom_t at[2]; /* for non connected jMax jack port */
  fts_atom_t k, a;  /* for non connected jMax jack port */

  /* create pipe */
  if (0 != pipe(pipedes))
  {
    fts_log("[jackaudiomanager] cannot create pipe descriptors \n");
    return;
  }

  /* create jack manager */
  if (-1 == create_jack_manager_client())
  {
    fts_log("[jackaudiomanager] cannot connect to jack server, so no jackaudioport available \n");
    post("[jackaudiomanager] cannot connect to jack server, so no jackaudioport available \n");
    return;
  }

  s_default_client_name = fts_new_symbol("jMax_jackaudiomanager");

  /* for connect/disconnect thread */
  s_running = fts_new_symbol("run");
  s_activate = fts_new_symbol("activate");
  s_deactivate = fts_new_symbol("deactivate");
  s_connect = fts_new_symbol("connect");

  /* non connected jMax jack port name */
  s_jmax_jack_input = fts_new_symbol("jMax jack input");
  s_jmax_jack_output = fts_new_symbol("jMax jack output");

  jackaudiomanager_type = fts_class_install( s, jackaudiomanager_instantiate);

  fts_hashtable_init(&jack_port_input_ht, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&jack_port_output_ht, FTS_HASHTABLE_SMALL);
  /* 
     jack output ports are input jMax ports 
     and 
     jack input ports are output jMax ports 
  */
  jackaudiomanager_scan_ports(&jack_port_input_ht, JackPortIsOutput);
  jackaudiomanager_scan_ports(&jack_port_output_ht, JackPortIsInput);

  /*
    create non connected jMax jack port 
    and add them to hashtables
  */
  /* input */
  fts_set_int(at, JackPortIsOutput);
  fts_set_symbol(at + 1, s_jmax_jack_input);
  o = fts_object_create(jackaudioport_type, 2, at);
  if (NULL != o)
  {
    fts_object_refer(o);
    fts_audiomanager_put_port(s_jmax_jack_input, (fts_audioport_t*)o);
    fts_set_symbol(&k, s_jmax_jack_input);
    fts_set_object(&a, o);
    fts_hashtable_put(&jack_port_input_ht, &k, &a);      
  }
  /* output */
  fts_set_int(at, JackPortIsInput);
  fts_set_symbol(at + 1, s_jmax_jack_output);
  o = fts_object_create(jackaudioport_type, 2, at);
  if (NULL != o)
  {
    fts_object_refer(o);
    fts_audiomanager_put_port(s_jmax_jack_output, (fts_audioport_t*)o);
    fts_set_symbol(&k, s_jmax_jack_output);
    fts_set_object(&a, o);
    fts_hashtable_put(&jack_port_output_ht, &k, &a);      
  }


  jackaudiomanager_thread_type = fts_class_install(fts_new_symbol("jackaudiomanager_thread"), 
							   jackaudiomanager_thread_instantiate);

  jackaudiomanager_object = fts_object_create(jackaudiomanager_type, 0, NULL);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
