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
#include <unistd.h>
#include <jack/jack.h>
#include <fts/fts.h>
#include <fts/thread.h>
#include "jackaudio.h"

#define DEFAULT_DELAY_MS 50
#define DEFAULT_THREAD_FIFO_SIZE 40

typedef struct _jackaudiomanager_thread_t
{
  fts_object_t o;

  /* connect fifo */
  fts_fifo_t connect_fifo;
  void* connect_buffer;
  int connect_size;
  
  int connect_state;

  /* disconnect fifo */
  fts_fifo_t disconnect_fifo;
  void* disconnect_buffer;
  int disconnect_size;

  int disconnect_state;

  /* register fifo */
  fts_fifo_t register_fifo;
  void* register_buffer;
  int register_size;

  int register_state;

  fts_symbol_t src_port;
} jackaudiomanager_thread_t;

typedef struct _jackaudiomanager_t
{
/*   fts_audiomanager_t manager; */
  fts_object_t o;

  fts_object_t* jack_communication;

  fts_thread_worker_t* connect_manager;
  fts_thread_worker_t* disconnect_manager;

  fts_thread_worker_t* register_manager;
  fts_thread_worker_t* unregister_manager;

} jackaudiomanager_t;


static fts_symbol_t s_connect;
static fts_symbol_t s_disconnect;

static fts_symbol_t s_register;
static fts_symbol_t s_unregister;

/**************************************************
 *
 * jack jmax client 
 *
 */
static fts_symbol_t s_default_client_name;
jack_client_t* manager_jack_client = NULL;
static int jack_count = 0;


/* JACK callback */
/* TODO:
   Change this function if we want more JACK port 
*/
static
int jackaudiomanager_process(jack_nframes_t nframes, void* arg)
{

/*   jackaudioport_t* self = (jackaudioport_t*)arg; */
/*   /\* Get JACK output port buffer pointer *\/ */
/*   jack_default_audio_sample_t* out = (jack_default_audio_sample_t*)jack_port_get_buffer(self->output_port, nframes); */
/*   /\* Get JACK input port buffer pointer *\/ */
/*   jack_default_audio_sample_t* in = (jack_default_audio_sample_t*)jack_port_get_buffer(self->input_port, nframes); */
    
  int n = 0;
  /* get number of samples of a FTS tick */
  int samples_per_tick = fts_dsp_get_tick_size();

/*   /\* TODO:  */
/*      Check if in/out are valid pointer  */
/*   *\/ */
/*   self->input_buffer = in; */
/*   self->output_buffer = out; */
    
  /* TODO: 
     Need to be fix if (nframes % samples_per_tick != 0) 

     Case 1: nframes < samples_per_tick
       
     Case 2: nframes > samples_per_tick

  */
  for (n = 0; n < nframes; n += samples_per_tick)
  {
    /*	fts_sched_run_one_tick_without_select(); */
    /* Run scheduler */
    fts_sched_run_one_tick();  
/*     /\* Step forward in input/output buffer *\/ */
/*     self->input_buffer += samples_per_tick; */
/*     self->output_buffer += samples_per_tick; */
  }
  return 0;
}



static
void create_jack_manager_client()
{
  if (NULL == manager_jack_client)
  {
    jack_count = 0;
    manager_jack_client = jack_client_new("jMax");
    if (NULL == manager_jack_client)
    {
      fts_log("[jackaudiomanager] cannot create jMax manager jack client\n");      
      return;
    }
    else
    {
      fts_log("[jackaudiomanager] jMax manager jack client created \n");
    }
  }
  ++jack_count;
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
jackaudiomanager_thread_connect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static fts_symbol_t src_port;
  static fts_symbol_t dest_port;
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;

/*   fts_log("[connect thread is running \n] ... ");   */
  /* empty fifo */
  while (fts_fifo_read_level(&self->connect_fifo) >= sizeof(fts_atom_t))
  {
    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&self->connect_fifo);
    
    fts_fifo_incr_read(&self->connect_fifo, sizeof(fts_atom_t));

    switch(self->connect_state)
    {
    case 0:
      src_port = fts_get_symbol(atom);
      self->connect_state++;
      break;
    case 1:
      dest_port = fts_get_symbol(atom);
      /* call jack_connect */
      jack_connect(manager_jack_client, src_port, dest_port);
      self->connect_state = 0;
      break;
      
    default:
      /* unreachable case ... */
      break;
    }
  }

}

static void
jackaudiomanager_thread_disconnect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static fts_symbol_t src_port;
  static fts_symbol_t dest_port;
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;

/*   fts_log("[disconnect thread is running \n] ... "); */
  /* empty fifo */
  while (fts_fifo_read_level(&self->disconnect_fifo) >= sizeof(fts_atom_t))
  {
    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&self->disconnect_fifo);
    fts_fifo_incr_read(&self->disconnect_fifo, sizeof(fts_atom_t));

    switch(self->disconnect_state)
    {
    case 0:
      src_port = fts_get_symbol(atom);
      self->disconnect_state++;
      break;
    case 1:
      dest_port = fts_get_symbol(atom);
      /* call jack_connect */
      jack_disconnect(manager_jack_client, src_port, dest_port);
      self->disconnect_state = 0;
      break;
      
    default:
      /* unreachable case ... */
      break;
    }
  }
}

static void
jackaudiomanager_thread_register(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static fts_symbol_t port_name;
  static fts_symbol_t port_flags;
  unsigned long flags = 0;
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;
  
/*   fts_log("[register thread is running \n] ... "); */
  /* empty fifo */
  while (fts_fifo_read_level(&self->register_fifo) >= sizeof(fts_atom_t))
  {
    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&self->register_fifo);
    fts_fifo_incr_read(&self->register_fifo, sizeof(fts_atom_t));

    switch(self->register_state)
    {
    case 0:
      port_name = fts_get_symbol(atom);
      self->register_state++;
      break;
    case 1:
      port_flags = fts_get_symbol(atom);
      if (fts_s_input == port_flags)
      {
	flags = JackPortIsInput;
      }
      else
      {
	flags = JackPortIsOutput;
      }
      /* call jack_port_register */
      /* return newly created jack port => how to get it ? */
      jack_port_register(manager_jack_client, port_name, JACK_DEFAULT_AUDIO_TYPE, flags, 0);
      self->register_state = 0;
      break;
      
    default:
      /* unreachable case ... */
      break;
    }
  }

}

static void
jackaudiomanager_thread_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;
  int bytes = 0;
  int size = DEFAULT_THREAD_FIFO_SIZE;
  fts_atom_t* atoms;
  int i;

  bytes = size * sizeof(fts_atom_t);
  self->connect_buffer = fts_malloc(bytes);
  self->connect_size = size;

  fts_fifo_init(&self->connect_fifo, self->connect_buffer, bytes);
  atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->connect_fifo);
  for (i = 0; i < size; ++i)
  {
    fts_set_void(atoms + i);
  }

  self->connect_state = 0;

  self->disconnect_buffer = fts_malloc(bytes);
  self->disconnect_size = size;
  fts_fifo_init(&self->disconnect_fifo, self->disconnect_buffer, bytes);

  atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->disconnect_fifo);
  for (i = 0; i < size; ++i)
  {
    fts_set_void(atoms + i);
  }

  self->disconnect_state = 0;

  self->register_buffer = fts_malloc(bytes);
  self->register_size = size;

  fts_fifo_init(&self->register_fifo, self->register_buffer, bytes);
  atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->register_fifo);
  for (i = 0; i < size; ++i)
  {
    fts_set_void(atoms + i);
  }

  self->register_state = 0;
  
}

static void
jackaudiomanager_thread_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_thread_t* self = (jackaudiomanager_thread_t*)o;

  fts_free(self->connect_buffer);
  fts_free(self->disconnect_buffer);

  fts_free(self->register_buffer);
/*   fts_free(self->unregister_buffer); */
}

static void
jackaudiomanager_thread_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_thread_t), 
		 jackaudiomanager_thread_init, jackaudiomanager_thread_delete);
  fts_class_message_varargs(cl, s_connect, jackaudiomanager_thread_connect);
  fts_class_message_varargs(cl, s_disconnect, jackaudiomanager_thread_disconnect);

  fts_class_message_varargs(cl, s_register, jackaudiomanager_thread_register);
/*   fts_class_message_varargs(cl, s_unregister, jackaudiomanager_thread_unregister); */

}


/**************************************************
 *
 * jackaudiomanager
 *
 */

/* array of devices names */
/* HASHTABLE ? */
static fts_array_t jackaudiomanager_inputs_array;
static fts_array_t jackaudiomanager_outputs_array;

static fts_thread_worker_t*
create_thread(fts_object_t* obj, fts_symbol_t mess)
{
  fts_thread_worker_t* worker = fts_malloc(sizeof(fts_thread_worker_t));
  fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));

  thread_job->object = obj;
  fts_object_refer(obj);
  
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
  fts_object_release(worker->thread_function->object);
  fts_free(worker->thread_function);
  fts_free(worker);
}


static void
create_connect_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = create_thread(self->jack_communication, s_connect);
  if (NULL != worker)
  {
    self->connect_manager = worker;
  }
  else
  {
    fts_object_error((fts_object_t*)self, "Cannot create connect thread \n");
  }
}

static void
delete_connect_thread(jackaudiomanager_t* self)
{
  delete_thread(self->connect_manager);
}


static void
create_disconnect_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = create_thread(self->jack_communication, s_disconnect);
  if (NULL != worker)
  {
    self->disconnect_manager = worker;
  }
  else
  {
    fts_object_error((fts_object_t*)self, "Cannot create connect thread \n");
  }
}

static void
delete_disconnect_thread(jackaudiomanager_t* self)
{
  delete_thread(self->disconnect_manager);
}


static void
create_register_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = create_thread(self->jack_communication, s_register);
  if (NULL != worker)
  {
    self->register_manager = worker;
  }
  else
  {
    fts_object_error((fts_object_t*)self, "Cannot create connect thread \n");
  }
}

static void
delete_register_thread(jackaudiomanager_t* self)
{
  delete_thread(self->register_manager);
}


static void
create_unregister_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = create_thread(self->jack_communication, s_unregister);
  if (NULL != worker)
  {
    self->unregister_manager = worker;
  }
  else
  {
    fts_object_error((fts_object_t*)self, "Cannot create connect thread \n");
  }
}

static void
delete_unregister_thread(jackaudiomanager_t* self)
{
  delete_thread(self->unregister_manager);
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
jackaudiomanager_scan_ports(fts_array_t* array, int flags)
{
  const char** ports;
  int i;
  fts_symbol_t cur_sym;
  fts_audioport_t* port;
  ports = jack_get_ports(manager_jack_client, 
			 NULL, /* no pattern on port name NULL */
			 NULL, /* no pattern on type */
			 flags);

  i = 0;
  while(NULL != ports[i])
  {
    fts_atom_t at;
    cur_sym = fts_new_symbol(ports[i]);
    fts_array_append_symbol(array, cur_sym);
    fts_log("[jackaudiomanager] append symbol : %s\n", ports[i]);
    fts_set_int(&at, flags);
    port = (fts_audioport_t*)fts_object_create(jackaudioport_type, NULL, 1, &at);
    fts_audiomanager_put_port(cur_sym, port);
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


static void
jackaudiomanager_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self;
  fts_bytestream_t* bytestream = fts_post_get_stream(ac, at);
  int i;

  /* 
     jack input are jmax output and
     jack output are jmax input 
  */
  fts_array_clear(&jackaudiomanager_inputs_array);
  jackaudiomanager_scan_ports(&jackaudiomanager_inputs_array, JackPortIsOutput);

  fts_array_clear(&jackaudiomanager_outputs_array);
  jackaudiomanager_scan_ports(&jackaudiomanager_outputs_array, JackPortIsInput);

  fts_spost(bytestream, "[jackaudiomanager] Input Ports\n");
  for (i = 0; i < fts_array_get_size(&jackaudiomanager_inputs_array); ++i)
  {
    fts_spost(bytestream, "%s\n", fts_get_symbol(fts_array_get_element(&jackaudiomanager_inputs_array, i)));
  }

  fts_spost(bytestream, "[jackaudiomanager] Output Ports\n");
  for (i = 0; i < fts_array_get_size(&jackaudiomanager_inputs_array); ++i)
  {
    fts_spost(bytestream, "%s\n", fts_get_symbol(fts_array_get_element(&jackaudiomanager_outputs_array, i)));
  }
}

static void
jackaudiomanager_connect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_symbol_t src_port;
  fts_symbol_t dest_port;
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* connect_fifo;

  src_port = fts_get_symbol(at);
  dest_port = fts_get_symbol(at + 1);
  fts_log("[jackaudiomanager:] connection between %s and %s\n", src_port, dest_port);
  
  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  connect_fifo = &manager_object->connect_fifo;

  if (0 != add_symbol_to_fifo(connect_fifo, src_port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add src port symbol in connect FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(connect_fifo, dest_port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add dest port symbol in connect FIFO \n");
    return;
  }
  
}

static void
jackaudiomanager_disconnect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_symbol_t src_port;
  fts_symbol_t dest_port;
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* disconnect_fifo;

  src_port = fts_get_symbol(at);
  dest_port = fts_get_symbol(at + 1);
  fts_log("[jackaudiomanager:] disconnection between %s and %s\n", src_port, dest_port);
  
  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  disconnect_fifo = &manager_object->disconnect_fifo;

  if (0 != add_symbol_to_fifo(disconnect_fifo, src_port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add src port symbol in disconnect FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(disconnect_fifo, dest_port))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add dest port symbol in disconnect FIFO \n");
    return;
  }
  
}

static void
jackaudiomanager_register(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_symbol_t port_name = fts_get_symbol(at);
  fts_symbol_t port_flags = fts_get_symbol(at + 1);
  jackaudiomanager_thread_t* manager_object;
  fts_fifo_t* register_fifo;

  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_thread_t*)self->jack_communication;
  register_fifo = &manager_object->register_fifo;

  if (0 != add_symbol_to_fifo(register_fifo, port_name))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add src port symbol in disconnect FIFO \n");
    return;
  }
  if (0 != add_symbol_to_fifo(register_fifo, port_flags))
  {
    /* error occur */
    fts_log("[jackaudiomanager] cannot add dest port symbol in disconnect FIFO \n");
    return;
  }

}


static void
jackaudiomanager_unregister(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_symbol_t port_name = fts_get_symbol(at);


}

/* This function is used to remove object from scheduler and to activate JACK client */
static void
jackaudiomanager_halt(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fd_set rfds;
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jack_client_t* client = jackaudiomanager_get_jack_client();
  /* Remove object of FTS scheduler */
  fts_sched_remove(o);
  fts_log("[jackaudioport] jackaudioport removed from scheduler \n");

    
  /* Activate jack client */    
  if (jack_activate(client) == -1)
  {
    fts_log("[jackaudiomanager] cannot activate JACK client \n");
    fts_object_error(o, "cannot activate JACK client \n");
    return;
  }


  fts_log("[jackaudiomanager] jack client activated \n");


  /* Stop FTS scheduler */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  /* check return value of select */
  if (select(1, &rfds, NULL, NULL, NULL) < 0)
  {
    fts_log("[jackaudiomanager] select failed \n");
  }

  fts_log("[jackaudiomanager] FTS scheduler stopped \n");

}


static void
jackaudiomanager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_set_jack_client(self);

  /* start the fts thread manager */
  fts_thread_manager_start();

  /* create jack communication object */
  self->jack_communication = fts_object_create(jackaudiomanager_thread_type, NULL, 0, 0);
  fts_object_refer(self->jack_communication);

  /* create connections threads */
  create_connect_thread(self);
  create_disconnect_thread(self);

  /* create register and unregister threads */
  create_register_thread(self);
/*   create_unregister_thread(self); */


  /* JACK client process callback setting */
  jack_set_process_callback(jackaudiomanager_get_jack_client(),
			    jackaudiomanager_process, /* callback function */
			    (void*)self);          /* we need to have our object
						      in our callback function
						   */
  fts_log("[jackaudioport] set jackaudioport process callback \n");

  fts_sched_add(o, FTS_SCHED_ALWAYS);
}

static void
jackaudiomanager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_unset_jack_client(self);

  delete_connect_thread(self);
  delete_disconnect_thread(self);

  delete_register_thread(self);
/*   delete_unregister_thread(self); */
}

static void 
jackaudiomanager_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_t), jackaudiomanager_init, jackaudiomanager_delete);

  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, jackaudiomanager_print);
  
  fts_class_message_varargs(cl, s_connect, jackaudiomanager_connect);
  fts_class_message_varargs(cl, s_disconnect, jackaudiomanager_disconnect);

  fts_class_message_varargs(cl, s_register, jackaudiomanager_register);
  fts_class_message_varargs(cl, s_unregister, jackaudiomanager_unregister);

  fts_class_message_varargs(cl, fts_s_sched_ready, jackaudiomanager_halt);
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

  /* create jack manager */
  create_jack_manager_client();

  s_default_client_name = fts_new_symbol("jMax_jackaudiomanager");
  s_connect = fts_new_symbol("connect");
  s_disconnect = fts_new_symbol("disconnect");

  s_register = fts_new_symbol("register");
  s_unregister = fts_new_symbol("unregister");

  fts_class_install( s, jackaudiomanager_instantiate);

  fts_array_init(&jackaudiomanager_inputs_array, 0, 0);
  fts_array_init(&jackaudiomanager_outputs_array, 0, 0);

  fts_array_clear(&jackaudiomanager_inputs_array);
  jackaudiomanager_scan_ports(&jackaudiomanager_inputs_array, JackPortIsOutput);

  fts_array_clear(&jackaudiomanager_outputs_array);
  jackaudiomanager_scan_ports(&jackaudiomanager_outputs_array, JackPortIsInput);
  
  jackaudiomanager_thread_type = fts_class_install(fts_new_symbol("jackaudiomanager_thread"), 
							   jackaudiomanager_thread_instantiate);

}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
