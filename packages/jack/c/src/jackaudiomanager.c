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
#define DEFAULT_CONNECT_THREAD_FIFO_SIZE 40

typedef struct _jackaudiomanager_connect_thread_t
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

  fts_symbol_t src_port;
} jackaudiomanager_connect_thread_t;

typedef struct _jackaudiomanager_t
{
/*   fts_audiomanager_t manager; */
  fts_object_t o;
  
  fts_thread_worker_t* connect_manager;
  fts_thread_worker_t* disconnect_manager;
} jackaudiomanager_t;


static fts_symbol_t s_connect;
static fts_symbol_t s_disconnect;

/**************************************************
 *
 * jack jmax client 
 *
 */
static fts_symbol_t s_default_client_name;
jack_client_t* manager_jack_client = NULL;
static int jack_count = 0;

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
fts_class_t* jackaudiomanager_connect_thread_type = NULL;

static void
jackaudiomanager_connect_thread_connect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static fts_symbol_t src_port;
  static fts_symbol_t dest_port;
  jackaudiomanager_connect_thread_t* self = (jackaudiomanager_connect_thread_t*)o;
  
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
jackaudiomanager_connect_thread_disconnect(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  static fts_symbol_t src_port;
  static fts_symbol_t dest_port;
  jackaudiomanager_connect_thread_t* self = (jackaudiomanager_connect_thread_t*)o;
  
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
jackaudiomanager_connect_thread_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_connect_thread_t* self = (jackaudiomanager_connect_thread_t*)o;
  int bytes = 0;
  int size = DEFAULT_CONNECT_THREAD_FIFO_SIZE;
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

  
}

static void
jackaudiomanager_connect_thread_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_connect_thread_t* self = (jackaudiomanager_connect_thread_t*)o;

  fts_free(self->connect_buffer);
  fts_free(self->disconnect_buffer);
}

static void
jackaudiomanager_connect_thread_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_connect_thread_t), 
		 jackaudiomanager_connect_thread_init, jackaudiomanager_connect_thread_delete);
  fts_class_message_varargs(cl, s_connect, jackaudiomanager_connect_thread_connect);
  fts_class_message_varargs(cl, s_disconnect, jackaudiomanager_connect_thread_disconnect);
}


/**************************************************
 *
 * jackaudiomanager
 *
 */

/* array of devices names */
static fts_array_t jackaudiomanager_inputs_array;
static fts_array_t jackaudiomanager_outputs_array;

static void
create_connect_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = fts_malloc(sizeof(fts_thread_worker_t));
  jackaudiomanager_connect_thread_t* connection_manager = (jackaudiomanager_connect_thread_t*)fts_object_create(jackaudiomanager_connect_thread_type, NULL, 0, 0);
  fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));

  thread_job->object = (fts_object_t*)connection_manager;
  thread_job->method = fts_class_get_method_varargs(fts_object_get_class(thread_job->object),
						    s_connect);
  
  if (0 == thread_job->method)
    {
	fts_log("[jackaudiomanager] no such method \n");
	fts_object_set_error((fts_object_t*)self, "no such method, init failed \n");
	fts_free(thread_job);
	fts_free(worker);
	return;
    }
    
    thread_job->ac = 0;
    thread_job->at = NULL;
    thread_job->symbol = s_connect;
    
    thread_job->delay_ms = DEFAULT_DELAY_MS;
    worker->thread_function = thread_job;
    self->connect_manager = worker;
}

static void
delete_connect_thread(jackaudiomanager_t* self)
{
  fts_object_destroy(self->connect_manager->thread_function->object);
  fts_free(self->connect_manager->thread_function);
  fts_free(self->connect_manager);
}

static void
create_disconnect_thread(jackaudiomanager_t* self)
{
  fts_thread_worker_t* worker = fts_malloc(sizeof(fts_thread_worker_t));
  jackaudiomanager_connect_thread_t* disconnection_manager = (jackaudiomanager_connect_thread_t*)fts_object_create(jackaudiomanager_connect_thread_type, NULL, 0, 0);
  fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));

  thread_job->object = (fts_object_t*)disconnection_manager;
  thread_job->method = fts_class_get_method_varargs(fts_object_get_class(thread_job->object),
						    s_disconnect);
  
  if (0 == thread_job->method)
    {
	fts_log("[jackaudiomanager] no such method \n");
	fts_object_set_error((fts_object_t*)self, "no such method, init failed \n");
	fts_free(thread_job);
	fts_free(worker);
	return;
    }
    
    thread_job->ac = 0;
    thread_job->at = NULL;
    thread_job->symbol = s_disconnect;
    
    thread_job->delay_ms = DEFAULT_DELAY_MS;
    worker->thread_function = thread_job;
    self->disconnect_manager = worker;
}

static void
delete_disconnect_thread(jackaudiomanager_t* self)
{
  fts_object_destroy(self->disconnect_manager->thread_function->object);
  fts_free(self->disconnect_manager->thread_function);
  fts_free(self->disconnect_manager);
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
  fts_log("jack_get_ports\n");
  ports = jack_get_ports(manager_jack_client, 
			 NULL, /* no pattern on port name NULL */
			 NULL, /* no pattern on type */
			 flags);

  fts_log("begin to store ports\n");
  i = 0;
  while(NULL != ports[i])
  {
    fts_array_append_symbol(array, fts_new_symbol(ports[i]));
    fts_log("[jackaudiomanager] append symbol : %s\n", ports[i]);
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
  jackaudiomanager_connect_thread_t* manager_object;
  fts_fifo_t* connect_fifo;

  src_port = fts_get_symbol(at);
  dest_port = fts_get_symbol(at + 1);
  fts_log("[jackaudiomanager:] connection between %s and %s\n", src_port, dest_port);
  post("[jackaudiomanager:] connection between %s and %s\n", src_port, dest_port);
  
  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_connect_thread_t*)self->connect_manager->thread_function->object;
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
  jackaudiomanager_connect_thread_t* manager_object;
  fts_fifo_t* disconnect_fifo;

  src_port = fts_get_symbol(at);
  dest_port = fts_get_symbol(at + 1);
  fts_log("[jackaudiomanager:] disconnection between %s and %s\n", src_port, dest_port);
  post("[jackaudiomanager:] disconnection between %s and %s\n", src_port, dest_port);
  
  /* add src and dest symbol to thread fifo */
  manager_object = (jackaudiomanager_connect_thread_t*)self->disconnect_manager->thread_function->object;
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
jackaudiomanager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_log("[jackaudiomanager_init call \n");
  jackaudiomanager_set_jack_client(self);

  /* create connections object */
  create_connect_thread(self);
  create_disconnect_thread(self);

  /* start the fts thread manager */
  fts_thread_manager_start();

  /* start connections thread */
  fts_thread_manager_create_thread(self->connect_manager);
  fts_thread_manager_create_thread(self->disconnect_manager);

}

static void
jackaudiomanager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_unset_jack_client(self);
  fts_thread_manager_cancel_thread(self->connect_manager);
  fts_thread_manager_cancel_thread(self->disconnect_manager);

  delete_connect_thread(self);
  delete_disconnect_thread(self);
}

static void 
jackaudiomanager_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_t), jackaudiomanager_init, jackaudiomanager_delete);

  fts_log("[jackaudiomanager_instantiate\n");
  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, jackaudiomanager_print);
  fts_class_message_varargs(cl, s_connect, jackaudiomanager_connect);
  fts_class_message_varargs(cl, s_disconnect, jackaudiomanager_disconnect);
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

  fts_log("[jackaudiomanager] config called \n");
  /* create jack manager */
  create_jack_manager_client();

  s_default_client_name = fts_new_symbol("jMax_jackaudiomanager");
  s_connect = fts_new_symbol("connect");
  s_disconnect = fts_new_symbol("disconnect");
  fts_class_install( s, jackaudiomanager_instantiate);
  fts_log("[jackaudiomanager] class install \n");

  fts_array_init(&jackaudiomanager_inputs_array, 0, 0);
  fts_array_init(&jackaudiomanager_outputs_array, 0, 0);
  
  jackaudiomanager_connect_thread_type = fts_class_install(fts_new_symbol("jackaudiomanager_connect_thread"), 
							   jackaudiomanager_connect_thread_instantiate);

}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
