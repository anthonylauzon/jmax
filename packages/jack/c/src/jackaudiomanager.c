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
#include "jackaudio.h"


typedef struct _jackaudiomanager_t
{
  fts_audiomanager_t manager;
} jackaudiomanager_t;


static fts_symbol_t s_default_client_name;

static jack_client_t* manager_jack_client = NULL;
static int jack_count = 0;

/* array of devices names */
static fts_array_t jackaudiomanager_inputs_array;
static fts_array_t jackaudiomanager_outputs_array;

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
  jack_port_t* in_port;
  jack_port_t* out_port;

  src_port = fts_get_symbol(at);
  dest_port = fts_get_symbol(at + 1);

  in_port = jack_port_by_name(manager_jack_client, src_port);
  out_port = jack_port_by_name(manager_jack_client, dest_port);

  fts_log("[jackaudiomanager:] connection between %s and %s\n", src_port, dest_port);
  post("[jackaudiomanager:] connection between %s and %s\n", src_port, dest_port);
  post("[jackaudiomanager:} jack port name: %s and %s \n", jack_port_name(in_port), jack_port_name(out_port));
  jack_connect(jackaudiomanager_get_jack_client(), src_port, dest_port);
}

static void
jackaudiomanager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  fts_log("[jackaudiomanager_init call \n");
  jackaudiomanager_set_jack_client(self);
}

static void
jackaudiomanager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  jackaudiomanager_t* self = (jackaudiomanager_t*)o;
  jackaudiomanager_unset_jack_client(self);
}

static void 
jackaudiomanager_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(jackaudiomanager_t), jackaudiomanager_init, jackaudiomanager_delete);

  fts_log("[jackaudiomanager_instantiate\n");
  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, jackaudiomanager_print);
  fts_class_message_varargs(cl, fts_new_symbol("connect"), jackaudiomanager_connect);
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
  fts_class_install( s, jackaudiomanager_instantiate);
  fts_log("[jackaudiomanager] class install \n");

  fts_array_init(&jackaudiomanager_inputs_array, 0, 0);
  fts_array_init(&jackaudiomanager_outputs_array, 0, 0);

}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
