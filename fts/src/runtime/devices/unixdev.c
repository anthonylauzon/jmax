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

/* Standard device and functions for FTS on a UNIX platform.

   Of course, this file do not include audio devices; it just
   include an implementation of a pipe based fts device to communicate
   with the server, and may be something else, like a basic serial
   line based MIDI.

   primitive version, should get better soon ...
*/ 


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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef HAS_TTY_DEV
#include <termios.h> 
#endif


#include "sys.h"
#include "lang.h"
#include "runtime/devices/devices.h"
#include "runtime/devices/unixdev.h"

/* Errors */

/* Module initialization */


static void init_stdio(void);
static void init_npipe(void);
static void init_socket_server(void);
static void init_socket_client(void);
static void init_udp_client(void);
static void file_dev_init(void);

#ifdef HAS_TTY_DEV
static void init_tty(void);
#endif

void
unixdev_init(void)
{
 init_stdio();
 init_npipe();
 init_socket_server();
 init_socket_client();
 init_udp_client();
 file_dev_init();

#ifdef HAS_TTY_DEV
 init_tty();
#endif

}

/******************************************************************************/
/*                                                                            */
/*            Generic  Character Device Support                               */
/*                                                                            */
/******************************************************************************/

/*
  Support for bidirectional character devices, based on intro(2) file descriptor;
  handle buffering and non-blocking wait, and eof and file closed errors and so on.
  */


struct fd_dev_data
{
  /* In data */

  char *get_buf;
  int get_bufsize;
  int  get_read_p; 
  int  get_size;
  int  get_fd;

  /* Out data */

  char *put_buf;
  int put_bufsize;
  int  put_buf_fill;
  int  put_fd;

  /* socket server related  */

  int listener;
  int block_on_listen;

};


fd_dev_data_t *fd_data_new( int get_bufsize, int put_bufsize)
{
  fd_dev_data_t *p;

  p = fts_malloc( sizeof(fd_dev_data_t));

  p->get_bufsize = get_bufsize;
  p->get_buf = fts_malloc( p->get_bufsize);
  p->get_read_p = 0;
  p->get_size = 0;

  p->put_bufsize = put_bufsize;
  p->put_buf = fts_malloc( p->put_bufsize);
  p->put_buf_fill = 0;

  p->block_on_listen = 0;	/* by default, don't block on listen */

  return p;
}

void fd_data_delete( fd_dev_data_t *d)
{
  fts_free( d->get_buf);
  fts_free( d->put_buf);
  fts_free( d);
}

void fd_data_set_input_fd( fd_dev_data_t *d, int fd)
{
  d->get_fd = fd;
}

int fd_data_get_input_fd( fd_dev_data_t *d)
{
  return d->get_fd;
}

void fd_data_set_output_fd( fd_dev_data_t *d, int fd)
{
  d->put_fd = fd;
}

int fd_data_get_output_fd( fd_dev_data_t *d)
{
  return d->put_fd;
}


/* Buffered non-blocking select based char read */

fts_status_t fd_dev_get( fts_dev_t *dev, unsigned char *cp)
{
  fd_dev_data_t *d = (fd_dev_data_t *) fts_dev_get_device_data(dev);

  if (d->get_read_p < d->get_size)
    {
      *cp = d->get_buf[d->get_read_p];
      d->get_read_p++;
      
      return fts_Success;
    }
  else
    {
      fd_set check;
      struct timeval timeout;

      timeout.tv_sec = 0;
      timeout.tv_usec = 0;

      FD_ZERO(&check);
      FD_SET(d->get_fd, &check);
      select(64, &check, (fd_set *)0, (fd_set *)0, &timeout);

      if (FD_ISSET(d->get_fd, &check))
	{
	  d->get_size = read(d->get_fd, d->get_buf, d->get_bufsize);

	  if (d->get_size < 0)
	    {
	      if (errno == EAGAIN)
		return &fts_data_not_ready;
	      else
		return &fts_dev_io_error; /* error: File IO error */
	    }
	  else if (d->get_size == 0)
	    return &fts_dev_eof;
	  
	  d->get_read_p = 0;
      
	  /* read ok */

	  *cp = d->get_buf[d->get_read_p];
	  d->get_read_p++;
	  
	  return fts_Success;
	}
      else
	return &fts_data_not_ready;
    }
}

/* output buffering */

fts_status_t fd_dev_put( fts_dev_t *dev, unsigned char c)
{
  fd_dev_data_t *d = (fd_dev_data_t *) fts_dev_get_device_data(dev);

  d->put_buf[d->put_buf_fill++] = c;

  if (d->put_buf_fill >= d->put_bufsize)
    {
      int r;

      r = write(d->put_fd, d->put_buf, d->put_buf_fill);

      if (r != d->put_buf_fill)
	{
	  d->put_buf_fill = 0;
	  return &fts_dev_io_error; /* error: File IO error */
	}
      else
	d->put_buf_fill = 0;
    }
  
  return fts_Success;
}

fts_status_t fd_dev_flush( fts_dev_t *dev)
{
  fd_dev_data_t *d = (fd_dev_data_t *) fts_dev_get_device_data(dev);

  if (d->put_buf_fill > 0)
    {
      int r;

      r = write(d->put_fd, d->put_buf, d->put_buf_fill);


      if (r != d->put_buf_fill)
	{
	  d->put_buf_fill = 0;
	  return &fts_dev_io_error; /* error: File IO error */
	}
      else
	d->put_buf_fill = 0;
    }
  
  return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                              STDIO  Device                                 */
/*                                                                            */
/******************************************************************************/

/* This device, called "stdio" support input and output on the 
   device, mapped to the current stdin and stdout file descriptor;
   good for using fts with local (pipe) connections.

   The device have no argument and no options.

   We use intro(2) functions because all the intro(3) i/o functions
   are blocking, and we need to do select; we do our own buffering.

   Only one device of this kind should be instantiated, of course.

   */

/* Forward declaration of host device/class functions */

static fts_status_t open_stdio(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_stdio(fts_dev_t *dev);

/*
 * create and install the device class 
 */

static void
init_stdio(void)
{
  fts_dev_class_t *stdio_class;

  stdio_class = fts_dev_class_new(fts_char_dev);

  /* No class level function defined */

  set_open_fun(stdio_class, open_stdio);
  set_close_fun(stdio_class, close_stdio);
  set_char_dev_get_fun(stdio_class, fd_dev_get);
  set_char_dev_put_fun(stdio_class, fd_dev_put);
  set_char_dev_flush_fun(stdio_class, fd_dev_flush);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("stdio"), stdio_class);
}


static fts_status_t open_stdio( fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fd_dev_data_t *p;

  p = fd_data_new( DEFAULT_GET_BUFSIZE, DEFAULT_PUT_BUFSIZE);
  p->get_fd = STDIN_FILENO;
  p->put_fd = STDOUT_FILENO;

  fts_dev_set_device_data(dev, (void *) p);

  return fts_Success;
}


static fts_status_t close_stdio( fts_dev_t *dev)
{
  fd_data_delete( fts_dev_get_device_data( dev));

  return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                              Named Pipe Device                             */
/*                                                                            */
/******************************************************************************/

/* The UNIX named pipe  device, called "npipe" support input and output on the same
   device, mapped to two named pipes calle <arg>.from and <arg>.to, where
   <arg> is the argument to open.
   */

/* Forward declaration of host device/class functions */

static fts_status_t open_npipe(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_npipe(fts_dev_t *dev);

static void
init_npipe(void)
{
  fts_dev_class_t *npipe_class;

  npipe_class = fts_dev_class_new(fts_char_dev);

  set_open_fun(npipe_class, open_npipe);
  set_close_fun(npipe_class, close_npipe);
  set_char_dev_get_fun(npipe_class, fd_dev_get);
  set_char_dev_put_fun(npipe_class, fd_dev_put);
  set_char_dev_flush_fun(npipe_class, fd_dev_flush);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("npipe"), npipe_class);
}


static fts_status_t
open_npipe(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  const char *pipe_name;
  char ftsTo[1024], ftsFrom[1024];
  fd_dev_data_t *p;

  p = fd_data_new( DEFAULT_GET_BUFSIZE, DEFAULT_PUT_BUFSIZE);

  pipe_name = fts_symbol_name(fts_get_symbol_arg(nargs, args, 0, fts_new_symbol("/tmp/fts")));

  sprintf(ftsTo, "%s.to", pipe_name);
  sprintf(ftsFrom, "%s.from", pipe_name);

  mkfifo(ftsFrom, S_IRWXU | S_IRWXG | S_IRWXO);
  mkfifo(ftsTo, S_IRWXU | S_IRWXG | S_IRWXO);

  p->get_fd = open(ftsFrom, O_RDWR);
  p->put_fd = open(ftsTo, O_RDWR);

  fts_dev_set_device_data(dev, (void *) p);

  return fts_Success;
}


static fts_status_t
close_npipe(fts_dev_t *dev)
{
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  close(p->get_fd);
  close(p->put_fd);

  fd_data_delete( fts_dev_get_device_data( dev));

  return fts_Success;
}



/******************************************************************************/
/*                                                                            */
/*                              Socket Server Device                          */
/*                                                                            */
/******************************************************************************/

/* 
   The UNIX socket_server  device, called "socket_server" support input and output on the same
   device, mapped to a TCP connection to a given port number (first argument
   of port); note that the implement the SERVER side of a TCP socket based connection,
   and FTS will stop until a client connect.

   Note that also if multiple "socket server" devices can be instantiated, only
   one connection for port is accepted, at least in the current implementation
   */

/* Forward declaration of host device/class functions */

static fts_status_t open_socket_server(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_socket_server(fts_dev_t *dev);
static fts_status_t socket_server_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t socket_server_put(fts_dev_t *dev, unsigned char c);


static void
init_socket_server(void)
{
  fts_dev_class_t *socket_server_class;

  socket_server_class = fts_dev_class_new(fts_char_dev);

  set_open_fun(socket_server_class, open_socket_server);
  set_close_fun(socket_server_class, close_socket_server);
  set_char_dev_get_fun(socket_server_class, socket_server_get);
  set_char_dev_put_fun(socket_server_class, socket_server_put);
  set_char_dev_flush_fun(socket_server_class, fd_dev_flush);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("socket_server"), socket_server_class);
}


static fts_status_t
open_socket_server(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct sockaddr_in serv_addr;
  unsigned short port;
  fd_dev_data_t *p;

  p = fd_data_new( DEFAULT_GET_BUFSIZE, DEFAULT_PUT_BUFSIZE);

  port = (unsigned short) fts_get_int_by_name(nargs, args, fts_new_symbol("port"), 2000);/* 2000 is the default server port */

  p->block_on_listen = fts_get_boolean_by_name(nargs, args, fts_new_symbol("blocking"), 0);

  p->listener = socket(AF_INET, SOCK_STREAM, 0);

  if (p->listener == -1)
    return &fts_dev_open_error;

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if (bind(p->listener, &serv_addr, sizeof(serv_addr)) == -1)
    {
      perror("error in open_socket_server: bind :");
      return &fts_dev_open_error;
    }

  if (listen(p->listener, 1) == -1)
    {
      perror("error in open_socket_server: listen :");
      return &fts_dev_open_error;
    }

  p->get_fd = -1;
  p->put_fd = -1;

  fts_dev_set_device_data(dev, (void *) p);

  return fts_Success;
}

static fts_status_t
close_socket_server(fts_dev_t *dev)
{
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  close(p->get_fd);/* only one needed, in and out are the same fd */
  close(p->listener);

  fd_data_delete( fts_dev_get_device_data(dev));

  return fts_Success;
}

/* An input or output operation on an not connected 
   socket_server device will cause a call to accept.
   If the device is blocking, FTS will halt waiting
   for a connection.

   If the device is not blocking, the listener socket
   will be polled with select, and the operation will
   not be blocked (??? what about writing ??).
 */


static fts_status_t
socket_server_get(fts_dev_t *dev, unsigned char *cp)
{
  fts_status_t ret;
  struct sockaddr peer;
  int addrlen = sizeof(peer);
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  if (p->get_fd == -1)
    {
      if (p->block_on_listen)
	{
	  int newfd;

	  newfd = accept(p->listener, &peer, &addrlen);

	  if (newfd == -1)
	    {
	      perror("error in socket_server_get: accept :");
	      return &fts_dev_io_error; 
	    }
	  
	  p->get_fd = newfd;      
	  p->put_fd = newfd;

	  /* '6' is the TCP protocol number */

	  setsockopt(newfd, 6,  TCP_NODELAY, 0, 0);
	}
      else
	{
	  fd_set fdvar;
	  int numfds, nfound;
	  struct timeval timeout;

	  timeout.tv_sec = 0;
	  timeout.tv_usec = 0;

	  FD_ZERO(&fdvar);
	  FD_SET(p->listener, &fdvar);
	  
	  nfound = select(p->listener+1, &fdvar, (fd_set *)0, (fd_set *)0, &timeout);

	  if ((nfound > 1) && FD_ISSET(p->listener, &fdvar))
	    {
	      int newfd;

	      newfd = accept(p->listener, &peer, &addrlen);

	      if (newfd == -1)
		{
		  perror("error in socket_server_get: accept :");
		  return &fts_dev_io_error; 
		}

	      p->get_fd = newfd;      
	      p->put_fd = newfd;

	      /* '6' is the TCP protocol number */

	      setsockopt(newfd, 6,  TCP_NODELAY, 0, 0);
	    }
	}
    }

  ret = fd_dev_get(dev, cp);

  /*
    if the connection is closed, put fd back to -1 so
    next get we will block on accept
   */

  if ((ret == &fts_dev_eof) || (ret == &fts_dev_io_error))
    {
      p->get_fd = -1;
      p->put_fd = -1;

      return &fts_data_not_ready;
    }

  return ret;
}

static fts_status_t
socket_server_put(fts_dev_t *dev, unsigned char c)
{
  fts_status_t ret;
  struct sockaddr peer;
  int addrlen = sizeof(peer);
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  if (p->get_fd == -1)
    {
      int newfd;

      newfd = accept(p->listener, &peer, &addrlen);

      if (newfd == -1)
	{
	  perror("error in socket_server_put: accept :");
	  return &fts_data_not_ready;
	}

      p->get_fd = newfd;      
      p->put_fd = newfd;

      /* '6' is the TCP protocol number */

      setsockopt(newfd, 6,  TCP_NODELAY, 0, 0);
    }

  ret = fd_dev_put(dev, c);

  /*
    if the connection is closed, put fd back to -1 so
    next get we will block on accept
   */

  if ((ret == &fts_dev_eof) || (ret == &fts_dev_io_error))
    {
      p->get_fd = -1;
      p->put_fd = -1;

      return &fts_dev_eof;	/* do the appropriate action, probabily a restart */
    }

  return ret;
}




/******************************************************************************/
/*                                                                            */
/*                              Socket Client Device                          */
/*                                                                            */
/******************************************************************************/

/* 
   The UNIX socket_client  device, called "socket" support input and output on the same
   device, mapped to a TCP connection to a given IP address/port number (first argument
   of port); on the other side, a server should be waiting for connection requests !!!!
   */

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
static unsigned short ascii_to_ushort(const char *s);

static void
fts_socket_parse(fts_symbol_t addr, char **host, unsigned short *port)
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
      

/* Forward declaration of host device/class functions */

static fts_status_t open_socket_client(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_socket_client(fts_dev_t *dev);


static void
init_socket_client(void)
{
  fts_dev_class_t *socket_client_class;

  socket_client_class = fts_dev_class_new(fts_char_dev);

  set_open_fun(socket_client_class, open_socket_client);
  set_close_fun(socket_client_class, close_socket_client);
  set_char_dev_get_fun(socket_client_class, fd_dev_get);
  set_char_dev_put_fun(socket_client_class, fd_dev_put);
  set_char_dev_flush_fun(socket_client_class, fd_dev_flush);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("socket"), socket_client_class);
}


static fts_status_t
open_socket_client(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct sockaddr_in serv_addr;
  char *address;
  unsigned short port;
  int sock;
  fd_dev_data_t *p;

  p = fd_data_new( DEFAULT_GET_BUFSIZE, DEFAULT_PUT_BUFSIZE);

  fts_socket_parse(fts_get_symbol_arg(nargs, args, 0, 0), &address, &port);

  /* */

  if (address == NULL)
    return &fts_dev_open_error;

  if (! port)
    port = 2000;

  /* Create an Internet stream socket */

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == -1)
    return &fts_dev_open_error;

  /* Fill out address of server we want to connect to */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(address);
  serv_addr.sin_port = htons(port);

  /* connect */

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    return &fts_dev_open_error;

  p->get_fd = sock;
  p->put_fd = sock;

  /* '6' is the TCP protocol number */

  setsockopt(sock, 6,  TCP_NODELAY, 0, 0);

  fts_dev_set_device_data(dev, (void *) p);

  return fts_Success;
}


static fts_status_t
close_socket_client(fts_dev_t *dev)
{
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  close(p->get_fd);/* only one needed, in and out are the same fd */

  fd_data_delete(fts_dev_get_device_data(dev));

  return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                              TTY Device                                    */
/*                                                                            */
/******************************************************************************/

#ifdef HAS_TTY_DEV

/* TTY DEV */


/* The UNIX TTY device, called "tty" support input and output on the same
   device, on a tty serial line */

/* Forward declaration of host device/class functions */

static fts_status_t open_tty(fts_dev_t *dev, int nargs, fts_atom_t *args);
static fts_status_t close_tty(fts_dev_t *dev);

/* system initialization for a tty: it should derive all the paramenters
   from the open arguments. */

static void
tty_sys_initialize(int fd)
{
  struct termios tios;

  if (ioctl(fd, TCGETS, &tios) < 0)
    fprintf(stderr, "cannot get termios for tty `%d'\n", fd);
  else
    {
      tios.c_iflag      = IGNBRK;
      tios.c_oflag      = 0;
      tios.c_cflag      = /*B38400*/ B9600 | CS8 | CREAD | CLOCAL | HUPCL;
      tios.c_lflag      = 0;
      tios.c_cc[VINTR]  = 0;
      tios.c_cc[VQUIT]  = 0;
      tios.c_cc[VERASE] = 0;
      tios.c_cc[VKILL]  = 0;
      tios.c_cc[VMIN]   = 1;
      tios.c_cc[VTIME]  = 0;
      
      if (ioctl(fd, TCSETS, &tios) < 0)
	fprintf(stderr, "cannot set termios for tty `%d'\n", fd);
    }
}

static void
init_tty(void)
{
  tty_class = fts_dev_class_new(fts_char_dev);

  /* No class level function defined */

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  set_open_fun(tty_class, open_tty);
  set_close_fun(tty_class, close_tty);
  set_char_dev_get_fun(tty_class, tty_get);
  set_char_dev_put_fun(tty_class, tty_put);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("tty"), tty_class);
}


/* The instantiation function ignore the extra arguments */

static fts_status_t
open_tty(fts_dev_t *dev, int nargs, fts_atom_t *args)
{
  char *serial_port;

  /*
    Derived from the Eric Viara TTY code, i don't understand it completely ;->
     */

  p = fd_data_new( DEFAULT_GET_BUFSIZE, DEFAULT_PUT_BUFSIZE);

  serial_port = fts_symbol_name(fts_get_symbol_arg(nargs, args, 0, "/dev/ttyd2"));

  p->get_fd = open(serial_port, O_RDWR);
  tty_sys_initialize(p->get_fd);

  p->put_fd = open(serial_port, O_RDWR);
  tty_sys_initialize(p->put_fd);

  if (p->put_fd < 0 || p->get_fd < 0)
    return &fts_dev_open_error;

  return fts_Success;
}


static fts_status_t
close_tty_client(fts_dev_t *dev)
{
  fd_dev_data_t *p = fts_dev_get_device_data(dev);

  close(p->get_fd);/* only one needed, in and out are the same fd */

  fd_data_delete( fts_dev_get_device_data(dev));

  return fts_Success;
}

#endif

/******************************************************************************/
/*                                                                            */
/*                              FILE  Device                                  */
/*                                                                            */
/******************************************************************************/

/* file device; should be converted to use the same primitives
   as the above devices !!!
 */

static fts_status_t open_file_dev(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_file_dev(fts_dev_t *dev);

static fts_status_t file_dev_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t file_dev_put(fts_dev_t *dev, unsigned char c);
static fts_status_t file_dev_seek(fts_dev_t *dev, long offset, int whence); /* NOP */

static void
file_dev_init(void)
{
  fts_dev_class_t *file_dev_class;

  file_dev_class = fts_dev_class_new(fts_char_dev);

  /* No class level function defined */

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  set_open_fun(file_dev_class, open_file_dev);
  set_close_fun(file_dev_class, close_file_dev);
  set_char_dev_get_fun(file_dev_class, file_dev_get);
  set_char_dev_put_fun(file_dev_class, file_dev_put);
  set_char_dev_seek_fun(file_dev_class, file_dev_seek);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("file"), file_dev_class);
}


static fts_status_t
open_file_dev(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{

  if ((nargs >= 2) && fts_is_symbol(&args[0]) & fts_is_symbol(&args[1]))
    {
      FILE *f;

      f = fopen(fts_symbol_name(fts_get_symbol(&args[0])), fts_symbol_name(fts_get_symbol(&args[1])));
      
      if (f)
	{
	  fts_dev_set_device_data(dev, (void *) f);

	  return fts_Success;
	}
      else
	{
	  return &fts_dev_open_error;
	}
    }
	
  return &fts_dev_invalid_value_error;		/* ERROR */
}


static fts_status_t
close_file_dev(fts_dev_t *dev)
{
  fclose((FILE *) fts_dev_get_device_data(dev));
  
  return fts_Success;
}

static fts_status_t
file_dev_get(fts_dev_t *dev, unsigned char *cp)
{
  FILE *f = (FILE *) fts_dev_get_device_data(dev);

  *cp = getc(f);

  return fts_Success;
}

static fts_status_t
file_dev_put(fts_dev_t *dev, unsigned char c)
{
  FILE *f = (FILE *) fts_dev_get_device_data(dev);

  putc(c, f);

  return fts_Success;
}

static fts_status_t
file_dev_seek(fts_dev_t *dev, long offset, int whence)
{
  FILE *f = (FILE *) fts_dev_get_device_data(dev);

  fseek(f, offset, whence);

  return fts_Success;
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


#define UDP_PACKET_SIZE 512

typedef struct udp_dev_data
{
  /* In data */

  char get_buf[UDP_PACKET_SIZE];
  int  get_read_p; 
  int  get_size;

  /* Out data */

  char put_buf[UDP_PACKET_SIZE];
  int  put_buf_fill;

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
  p->get_read_p = 0;
  p->get_size = 0;
  p->put_buf_fill = 1;
  p->sequence = 0;
  return p;
}


static void
free_udp_data(udp_dev_data_t *d)
{
  fts_free(d);
}

/* Buffered non-blocking select based char read */

static fts_status_t 
udp_dev_get(fts_dev_t *dev, unsigned char *cp)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  if (dev_data->get_read_p < dev_data->get_size)
    {
      *cp = dev_data->get_buf[dev_data->get_read_p];
      dev_data->get_read_p++;
      
      return fts_Success;
    }
  else
    {
      fd_set check;
      struct timeval timeout;

      timeout.tv_sec = 0;
      timeout.tv_usec = 0;

      FD_ZERO(&check);
      FD_SET(dev_data->socket, &check);
      select(64, &check, (fd_set *)0, (fd_set *)0, &timeout);

      if (FD_ISSET(dev_data->socket, &check))
	{
	  dev_data->get_size = recvfrom(dev_data->socket, dev_data->get_buf, UDP_PACKET_SIZE, 0, 0, 0);
                                   
	  if (dev_data->get_size < 0)
	    {
	      if (errno == EAGAIN)
		return &fts_data_not_ready;
	      else
		return &fts_dev_io_error; /* error: File IO error */
	    }
	  else if (dev_data->get_size == 0)
	    return &fts_data_not_ready;
	  
	  dev_data->get_read_p = 0;
      
	  /* read ok */

	  *cp = dev_data->get_buf[dev_data->get_read_p];
	  dev_data->get_read_p++;
	  
	  return fts_Success;
	}
      else
	return &fts_data_not_ready;
    }
}

/* output buffering */

static fts_status_t udp_dev_flush(fts_dev_t *dev);

static fts_status_t
udp_dev_put(fts_dev_t *dev, unsigned char c)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  dev_data->put_buf[dev_data->put_buf_fill++] = c;

  if (dev_data->put_buf_fill >= UDP_PACKET_SIZE)
    {
      udp_dev_flush(dev);
    }
  
  return fts_Success;
}

static fts_status_t
udp_dev_flush(fts_dev_t *dev)
{
  udp_dev_data_t *dev_data = (udp_dev_data_t *) fts_dev_get_device_data(dev);

  if (dev_data->put_buf_fill > 1)
    {
      int r;

      /* set the sequence number and send */

      dev_data->put_buf[0] = dev_data->sequence;
      
      r = sendto(dev_data->socket, dev_data->put_buf, dev_data->put_buf_fill, 0,
		 &(dev_data->client_addr), sizeof(dev_data->client_addr));

      dev_data->sequence = (dev_data->sequence + 1) % 128;

      if (r != dev_data->put_buf_fill)
	{
	  dev_data->put_buf_fill = 1; /* leave place for the sequence number */
	  return &fts_dev_io_error; /* error: File IO error */
	}
      else
	dev_data->put_buf_fill = 1;/* leave place for the sequence number */
    }
  
  return fts_Success;
}


/* Forward declaration of host device/class functions */

static fts_status_t open_udp_client(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t close_udp_client(fts_dev_t *dev);
static fts_status_t udp_dev_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t udp_dev_put(fts_dev_t *dev, unsigned char c);
static fts_status_t udp_dev_flush(fts_dev_t *dev);

static void
init_udp_client(void)
{
  fts_dev_class_t *udp_client_class;

  udp_client_class = fts_dev_class_new(fts_char_dev);

  set_open_fun(udp_client_class, open_udp_client);
  set_close_fun(udp_client_class, close_udp_client);
  set_char_dev_get_fun(udp_client_class, udp_dev_get);
  set_char_dev_put_fun(udp_client_class, udp_dev_put);
  set_char_dev_flush_fun(udp_client_class, udp_dev_flush);

  /* Installing the class */

  fts_dev_class_register(fts_new_symbol("udp"), udp_client_class);
}


static fts_status_t
open_udp_client(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  struct sockaddr_in my_addr;
  char *address;
  unsigned short port;
  int sock;
  udp_dev_data_t *p;

  p = make_udp_data();

  fts_socket_parse(fts_get_symbol_arg(nargs, args, 0, 0), &address, &port);

  /* */

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

  bzero((char *)&(p->client_addr), sizeof(p->client_addr));
  p->client_addr.sin_family = AF_INET;
  p->client_addr.sin_addr.s_addr = inet_addr(address);
  p->client_addr.sin_port = htons(port);

  p->socket = sock;

  fts_dev_set_device_data(dev, (void *) p);

  /* Send an init package: empty content, just the packet */

  {
    char *init = "init";
    
    sendto(sock, init, 4, 0, &(p->client_addr), sizeof(p->client_addr));
  }

  return fts_Success;
}


static fts_status_t
close_udp_client(fts_dev_t *dev)
{
  udp_dev_data_t *p = fts_dev_get_device_data(dev);

  close(p->socket);/* only one needed, in and out are the same fd */

  free_udp_data(fts_dev_get_device_data(dev));

  return fts_Success;
}








