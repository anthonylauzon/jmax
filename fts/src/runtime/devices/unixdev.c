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
 * Standard device and functions for FTS on a UNIX platform.
 */ 

#include <fts/sys.h>


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h> /* ? */
/* should be: */
#include <time.h>
#include <netinet/in.h>
#if !defined(WIN32)
#include <netinet/tcp.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>

#include <fts/lang.h>
#include <fts/runtime.h>

#ifdef HAS_PTHREADS
#include <pthread.h>
#endif


/* Module initialization */

static void init_udp_client(void);

void unixdev_init(void)
{
 init_udp_client();
}

/* Forward declaration of host device/class functions */

static fts_status_t open_udp_client(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_udp_client(fts_dev_t *dev);
static fts_status_t udp_dev_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t udp_dev_put(fts_dev_t *dev, unsigned char c);
static fts_status_t udp_dev_flush(fts_dev_t *dev);

static void init_udp_client(void)
{
  fts_dev_class_t *udp_client_class;

  udp_client_class = fts_dev_class_new(fts_char_dev, fts_new_symbol("udp"));

  fts_dev_class_set_open_fun(udp_client_class, open_udp_client);
  fts_dev_class_set_close_fun(udp_client_class, close_udp_client);
  fts_dev_class_char_set_get_fun(udp_client_class, udp_dev_get);
  fts_dev_class_char_set_put_fun(udp_client_class, udp_dev_put);
  fts_dev_class_char_set_flush_fun(udp_client_class, udp_dev_flush);
}


/*
 * fts_socket_parse() 
 *
 * Utility to parse an parse an address of the forms:
 * - nadia
 * - 129.102.1.8
 * - <name>:<port>
 * - <addr>:<port>
 *
 * The dotted internet address is returned in a newly malloced string,
 */
static unsigned short ascii_to_ushort(const char *src)
{
  unsigned short v;

  v = 0;

  while ((*src != '\0') && (*src <= '9') && (*src >= '0'))
    {
      v = v * 10 + (*src - '0');
      src++;
    }

  return v;
}
      
static void fts_socket_parse(fts_symbol_t addr, char **host, unsigned short *port)
{
  struct hostent *hostptr;

  *port = 0;

  if (addr)
    {
      char address[256], *dst;
      const char *src;

      /* Separe name from port, if present, and extract the port  */

      src = fts_symbol_name(addr);
      dst = address;

      while (*src && (*src != ':'))
	*(dst++) = *(src++);

      *dst = '\0';

      if (*src == ':')
	*port = (unsigned short) ascii_to_ushort(src + 1);

      /* look for the host */

      hostptr = gethostbyname((char *)address);

      if (hostptr)
	{
	  /* hostname was given */

	  char *tp;
	  struct in_addr *i = (struct in_addr *)*(hostptr->h_addr_list);
	  tp = inet_ntoa(*i);	/* ascii of first host */

	  if (tp)
	    {
	      *host = (char *)malloc(strlen(tp) + 1);
	      strcpy(*host, tp);
	    }
	  else
	    *host = 0;
	}
      else
	{
	  /* maybe was supplied in dotted format already */

	  if (inet_addr((char *)address) == INADDR_NONE)
	    *host = 0;
	  else
	    {
	      *host = (char *)malloc(strlen(address) + 1);
	      strcpy(*host, address);
	    }
	}
    }
  else
    {
      *host = (char *)malloc(strlen("127.0.0.1") + 1);
      strcpy(*host, "127.0.0.1");
    }
}



/******************************************************************************/
/*                                                                            */
/*                              UDP  Device                                   */
/*                                                                            */
/******************************************************************************/

/* we emulate a byte oriented connection on top of a UDP packet; if we fully adopt 
   UDP, we may also drop the byte emulation and go straight for packets.
   Hostname and port are passed as mandatory argument  (host:port).
   */
   

/* 
   The UNIX udp_client  device, called "up" support input and output on the same
   device, mapped to a couple of UDP ports connection to a given IP address/port number (first argument
   of port);
   The device will send an init packet at the open.
   */


/******************************************************************************/
/*                                                                            */
/*            Generic  Character Device Support                               */
/*                                                                            */
/******************************************************************************/

/*
  Support for bidirectional character devices, based on intro(2) file descriptor;
  handle buffering and non-blocking wait, and eof and file closed errors and so on.
  */



#define UDP_PACKET_SIZE 2048
#define UDP_FIFO_SIZE (8*UDP_PACKET_SIZE)

typedef struct udp_dev_data
{
  pthread_t thread;
  pthread_mutex_t mutex;

  fts_fifo_t fifo;
  unsigned char get_buf[UDP_FIFO_SIZE];

  /* Out data */
  char put_buf[UDP_PACKET_SIZE];
  unsigned int put_buf_fill;

  /* socket related  */
  int  socket;
  struct sockaddr_in client_addr;

  /* Sequence control */
  char sequence;

} udp_dev_data_t;



static udp_dev_data_t *make_udp_data(void)
{
  udp_dev_data_t *p;

  p = fts_malloc(sizeof(udp_dev_data_t));
  p->put_buf_fill = 3;
  p->sequence = 0;
  return p;
}

static fts_status_t udp_dev_get(fts_dev_t *dev, unsigned char *cp)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  pthread_mutex_lock( &dev_data->mutex);

  if (fts_fifo_get_read_level( &dev_data->fifo) < 1)
    {
      pthread_mutex_unlock( &dev_data->mutex);

      return &fts_data_not_ready;
    }
    
  *cp = *fts_fifo_get_read_pointer( &dev_data->fifo);

  fts_fifo_incr_read_index( &dev_data->fifo, 1);

  pthread_mutex_unlock( &dev_data->mutex);

  return fts_Success;
}


/* output buffering */

static fts_status_t udp_dev_flush(fts_dev_t *dev);

static fts_status_t udp_dev_put(fts_dev_t *dev, unsigned char c)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  dev_data->put_buf[dev_data->put_buf_fill++] = c;

  if (dev_data->put_buf_fill >= UDP_PACKET_SIZE)
    {
      udp_dev_flush(dev);
    }
  
  return fts_Success;
}

static fts_status_t udp_dev_flush(fts_dev_t *dev)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  if (dev_data->put_buf_fill > 3)
    {
      unsigned int r;

      /* set the sequence number and send */

      dev_data->put_buf[0] = dev_data->sequence;

      /* Set the packet size */

      dev_data->put_buf[1] = dev_data->put_buf_fill / 256;
      dev_data->put_buf[2] = dev_data->put_buf_fill % 256;

      r = sendto(dev_data->socket, dev_data->put_buf, dev_data->put_buf_fill, 0,
		 &(dev_data->client_addr), sizeof(dev_data->client_addr));

      dev_data->sequence = (dev_data->sequence + 1) % 128;

      if (r != dev_data->put_buf_fill)
	{
	  dev_data->put_buf_fill = 3; /* leave place for the sequence number and packet size*/
	  return &fts_dev_io_error; /* error: File IO error */
	}
      else
	dev_data->put_buf_fill = 3;/* leave place for the sequence number and packet size */
    }
  
  return fts_Success;
}


void *udp_thread_run( void *arg)
{
  int sock, size, i;
  fts_fifo_t *fifo;
  char buffer[UDP_PACKET_SIZE];

  sock = ((udp_dev_data_t *)arg)->socket;
  fifo = &((udp_dev_data_t *)arg)->fifo;

  while (1) 
    {
      pthread_testcancel();
      size = recvfrom( sock, buffer, UDP_PACKET_SIZE, 0, 0, 0);
      pthread_testcancel();
                                   
      if (size < 0)
	{
	  if (errno == EAGAIN)
	    continue;
	  else
	    return NULL;
	}
      else if (size == 0)
	continue;

      if (fts_fifo_get_write_level( fifo) < size)
	continue; /* overflow */
      
      for ( i = 0; i < size; i++)
	{
	  volatile unsigned char *p = fts_fifo_get_write_pointer( fifo);
	  
	  *p = buffer[i];
	  fts_fifo_incr_write_index( fifo, 1);
	}
    }
}


static fts_status_t
open_udp_client(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct sockaddr_in my_addr;
  char *address;
  unsigned short port;
  int sock;
  udp_dev_data_t *dev_data;

  dev_data = make_udp_data();

  fts_socket_parse(fts_get_symbol_arg(nargs, args, 0, 0), &address, &port);

  if (address == NULL)
    return &fts_dev_open_error;

  /* Create an Internet stream socket */

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if (sock == -1)
    return &fts_dev_open_error;

  /* Bind the socket to an arbitrary available port  */

  memset( (char *)&my_addr, 0, sizeof(struct sockaddr_in));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = 0;

  if (bind(sock, &my_addr, sizeof(struct sockaddr_in)) == -1)
    return &fts_dev_open_error;

  memset((char *)&(dev_data->client_addr), 0, sizeof(dev_data->client_addr));
  dev_data->client_addr.sin_family = AF_INET;
  dev_data->client_addr.sin_addr.s_addr = inet_addr(address);
  dev_data->client_addr.sin_port = htons(port);

  dev_data->socket = sock;

  fts_dev_set_device_data(dev, (void *) dev_data);

  /* Send an init package: empty content, just the packet */

  {
    char *init = "init";
    
    sendto(sock, init, 4, 0, &(dev_data->client_addr), sizeof(dev_data->client_addr));
  }

  /* init fifo */
  fts_fifo_init( &dev_data->fifo, UDP_FIFO_SIZE, dev_data->get_buf);

  /* start reader thread */
  {
    pthread_attr_t attr;

    pthread_attr_init( &attr);
    pthread_create( &dev_data->thread, &attr, udp_thread_run, dev_data);
    pthread_attr_destroy( &attr);

    pthread_mutex_init( &dev_data->mutex, NULL);
  }

  return fts_Success;
}


static fts_status_t close_udp_client(fts_dev_t *dev)
{
  udp_dev_data_t *dev_data = fts_dev_get_device_data(dev);
  
  /* stop reader thread and join */
  if ( pthread_cancel( dev_data->thread) < 0)
    post( "Cannot terminate udp thread\n");

  if (pthread_join( dev_data->thread, NULL) < 0)
    post( "Cannot terminate udp thread\n");

  close( dev_data->socket);/* only one needed, in and out are the same fd */

  fts_free( dev_data);

  return fts_Success;
}
