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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include "protoencode.h"

static int create_socket()
{
  struct sockaddr_in local_addr;
  int sock;

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if ( sock == -1)
    {
      fprintf( stderr, "Cannot open socket\n");
      perror( "socket");
      return -1;
    }

  memset( &local_addr, 0, sizeof( local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  local_addr.sin_port = 0;

  if ( bind( sock, &local_addr, sizeof(local_addr)) == -1)
    {
      fprintf( stderr, "Cannot bind socket\n");
      perror( "bind");
      close( sock);
      return -1;
    }

  return sock;
}

static int get_next_word( char **pp, char *word)
{
  char *p;

  p = *pp;

  while ( isspace(*p))
    p++;

  if ( *p == '\0')
    return 0;

  while ( ! isspace( *p))
    *word++ = *p++;

  *word = '\0';

  *pp = p;

  return 1;
}

static int get_int( char *s, int *p)
{
  char *save;

  save = s;
  while ( *s)
    {
      if ( *s != '-' && !isdigit( *s))
	return 0;
      s++;
    }

  sscanf( save, "%d", p);

  return 1;
}

static int get_float( char *s, float *p)
{
  return (sscanf( s, "%f", p) == 1);
}

#define LINE_LENGTH 128

static void send_loop( int socket, int port, char *address)
{
  struct sockaddr_in send_addr;
  protoencode_t encoder;

  memset((char *)&send_addr, 0, sizeof( send_addr));
  send_addr.sin_family = AF_INET;
  send_addr.sin_addr.s_addr = inet_addr(address);
  send_addr.sin_port = htons(port);

  protoencode_init( &encoder);

  printf( "Sending on socket %d, port %d, host %s\n", socket, port, address);

  while (1)
    {
      char line[LINE_LENGTH], word[LINE_LENGTH];
      char *p;
      
      if (fgets( line, LINE_LENGTH, stdin) == NULL)
	break;

      protoencode_start( &encoder);

      p = line;

      while ( get_next_word( &p, word))
	{
	  int ret, ivalue;
	  float fvalue;

	  fprintf( stderr, "%s\n", word);

	  if ( get_int( word, &ivalue) )
	    {
	      fprintf( stderr, "int %d\n", ivalue);
	      protoencode_put_int( &encoder, ivalue);
	    }
	  else if ( get_float( word, &fvalue))
	    {
	      fprintf( stderr, "float %f\n", fvalue);
	      protoencode_put_float( &encoder, fvalue);
	    }
	  else
	    {
	      fprintf( stderr, "string %s\n", word);
	      protoencode_put_string( &encoder, word);
	    }
	}

      protoencode_end( &encoder);

      if (sendto( socket, protoencode_get_mess( &encoder), protoencode_get_size( &encoder),
		  0, &send_addr, sizeof( send_addr)) < 0)
	perror( "sendto");
    }
	     
}

main( int argc, char **argv)
{
  int socket, port;

  if (argc < 3)
    {
      fprintf( stderr, "Usage: %s <port> <host name>\n", argv[0]);
      exit( 1);
    }

  sscanf( argv[1], "%d", &port);
  socket = create_socket();

  if ( socket < 0)
    exit( 1);

  send_loop( socket, port, argv[2]);
}
