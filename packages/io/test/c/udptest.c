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


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

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

static void send_loop( int socket, int port, char *address)
{
  struct sockaddr_in send_addr;
  int value, r;
  char buffer[10];

  memset((char *)&send_addr, 0, sizeof( send_addr));
  send_addr.sin_family = AF_INET;
  send_addr.sin_addr.s_addr = inet_addr(address);
  send_addr.sin_port = htons(port);

  printf( "Sending on socket %d, port %d, host %s\n", socket, port, address);

  while ( scanf( "%d", &value) != EOF)
    {
      buffer[0] = (unsigned char)(value &0xff);

      r = sendto( socket, buffer, 1, 0, &send_addr, sizeof( send_addr));
      if ( r < 0)
	{
	  perror( "sendto");
	}
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
