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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include "fts.h"
#include "protocol.h"

#define UDP_PACKET_SIZE 512

typedef struct {
    fts_object_t o;
    int socket;
    char buffer[UDP_PACKET_SIZE];
} updmessage_t;

#define MAXATOMS 128

static void updmessage_fd_fun( int fd, void *data)
{
  updmessage_t *this = (updmessage_t *)data;
  fts_atom_t at[MAXATOMS];
  fts_symbol_t selector = fts_s_list;
  int size, i, ac, first_token;
  protodecode_t pr;

  size = recvfrom( this->socket, this->buffer, UDP_PACKET_SIZE, 0, NULL, NULL);

  if ( size <= 0)
    return;

  protodecode_init( &pr);
  ac = 0;
  first_token = 0;

  for ( i = 0; i < size; i++)
    {
      protodecode_status_t t;

      t = protodecode_run( &pr, this->buffer[i]);

      if ( t == INT_TOKEN)
	{
	  if (first_token == 0)
	    first_token = 1;

	  fts_set_int( &(at[ac]), protodecode_get_int( &pr));
	  ac++;
	}
      else if ( t == FLOAT_TOKEN)
	{
	  if (first_token == 0)
	    first_token = 1;

	  fts_set_float( &(at[ac]), protodecode_get_float( &pr));
	  ac++;
	}
      else if ( t == STRING_TOKEN)
	{
	  if (first_token == 0)
	    {
	      selector = fts_new_symbol_copy( protodecode_get_string( &pr));
	      first_token = 1;
	    }
	  else
	    {
	      fts_set_symbol( &(at[ac]), fts_new_symbol_copy( protodecode_get_string( &pr)));
	      ac++;
	    }
	}
      else if ( t == EOM_TOKEN)
	break;
    }

  fts_outlet_send( (fts_object_t *)this, 0, selector, ac, at);
}


static void updmessage_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    updmessage_t *this = (updmessage_t *)o;
    int port;
    struct sockaddr_in addr;

    port = fts_get_int( &at[1]);

    post( "Created UDPmessage object on port %d\n", port);

    this->socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->socket == -1)
	{
	    post( "Cannot open socket\n");
	    return;
	}

    memset( &addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if ( bind( this->socket, &addr, sizeof(struct sockaddr_in)) == -1)
	{
	    post( "Cannot bind socket\n");
	    close( this->socket);
	    return;
	}

  fts_thread_add_fd( fts_thread_get_current(), this->socket, updmessage_fd_fun, this);
}

static void updmessage_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    updmessage_t *this = (updmessage_t *)o;

    if ( this->socket >= 0)
      {
	fts_thread_remove_fd( fts_thread_get_current(), this->socket);

	close( this->socket);
      }
}

static fts_status_t updmessage_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[2];

  fts_class_init( cl, sizeof( updmessage_t), 0, 1, 0);

  t[0] = fts_t_symbol;
  t[1] = fts_t_int;

  fts_method_define(cl, fts_SystemInlet, fts_s_init, updmessage_init, 2, t);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, updmessage_delete, 0, 0);

  return fts_Success;
}

void udpmessage_config( void)
{
  fts_class_install( fts_new_symbol("udpmessage"), updmessage_instantiate);
}
