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
 *
 * Authors: W. Ritsch, François Déchelle
 */
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fts.h"

#define SERIAL_PACKET_SIZE 512

typedef struct {
  fts_object_t o;
  int fd;
  struct termios termios_saved, termios_current;
  char buffer[SERIAL_PACKET_SIZE];
} serial_t;

static struct baud_speed {
  int baud;
  speed_t speed;
} baud_speed_conversion_table[] = {
  { 50, B50},
  { 75, B75},
  { 110, B110},
  { 134, B134},
  { 150, B150},
  { 200, B200},
  { 300, B300},
  { 600, B600},
  { 1200, B1200},
  { 1800, B1800},
  { 2400, B2400},
  { 4800, B4800},
  { 9600, B9600},
  { 19200, B19200},
  { 38400, B38400},
  { 57600, B57600},
  { 115200, B115200},
#ifdef B230400
  { 230400, B230400}
#endif
};
  
static speed_t baud_to_speed( int baud)
{
  unsigned int i;

  for ( i = 0; i < sizeof( baud_speed_conversion_table) / sizeof( struct baud_speed); i++)
    if ( baud_speed_conversion_table[i].baud == baud)
      return baud_speed_conversion_table[i].speed;

  post( "Invalid baud rate: %d\n", baud);

  return B0;
}

static void send_chars( int fd, char *buffer, int size)
{
  if ( write( fd, buffer, size) == size)
    {
#if 0
      /* Can't find the TCFLSH definition */
      /* anyway, the tcflush() man page seems to indicate that this is not
	 the right function to call, because it *discards* the output queue bytes
      */
      ioctl( fd, TCFLSH, TIOCFLUSH);  /* flush pending I/O chars */
#endif
    }
  else
    {
      post( "Error writing chars (%s)\n", strerror( errno));
    }
}

static void serial_int( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  char c;

  c = (char) (fts_get_int( at) & 0xff);

  send_chars( this->fd, &c, 1);
}

static void serial_list( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  int i;

  for ( i = 0; i < ac; i++)
    this->buffer[i] = (char)(fts_get_int( at+i) & 0xff);

  send_chars( this->fd, this->buffer, ac);
}

/* 
 * Here only for compatibility 
 */
static void serial_bang( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post( "serial_bang: compatibility method (you should remove the bang message, it is now useless)\n");
}

/*
 * This method will be called by the scheduler to inform the object that there are bytes available.
 */
static void serial_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  int size;

  size = read( this->fd, this->buffer, SERIAL_PACKET_SIZE);
  if ( size > 0)
    {
      int i;

      for ( i = 0; i < size; i++)
	fts_outlet_int( (fts_object_t *)this, 0, this->buffer[i]);
    }
}

static void serial_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  const char *dev_name;
  int baud;
  speed_t speed;

  dev_name = fts_symbol_name( fts_get_symbol_arg( ac, at, 1, 0) );
  baud = fts_get_int_arg( ac, at, 2, 19200);

  this->fd = -1;

  this->fd = open( dev_name, O_RDWR);

  if ( this->fd == -1)
    {
      post( "Cannot open serial line \"%s\" (%s)\n", dev_name, strerror( errno));
      return;
    }

  if ( tcgetattr( this->fd, &this->termios_saved) < 0)
    {
      post( "Cannot save serial configuration (%s)\n", strerror( errno));

      close( this->fd);
      return;
    }

  speed = baud_to_speed( baud);

  cfsetospeed( &this->termios_current, speed);
  cfsetispeed( &this->termios_current, 0);

  this->termios_current.c_iflag = IXOFF;
  this->termios_current.c_oflag = 0;
  this->termios_current.c_cflag = CS8 | CLOCAL | CREAD; /* Default */
  this->termios_current.c_lflag = 0;
  this->termios_current.c_cc[VMIN] = 0;     /* setup to return after 0 seconds */
  this->termios_current.c_cc[VTIME] = 0;   /* ..if no characters are received */

  if (tcsetattr( this->fd, TCSANOW, &this->termios_current) < 0)
    {
      post( "Cannot set serial configuration (%s)\n", strerror( errno));

      close( this->fd);
      return;
    }

  fts_sched_add_fd( fts_sched_get_current(), this->fd, 1, serial_receive, (fts_object_t *)this);
}

static void serial_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  if ( this->fd >= 0)
    {
      if (tcsetattr( this->fd, TCSANOW, &this->termios_saved) < 0)
	{
	  post( "Cannot restore serial configuration (%s)\n", strerror( errno));
	}

      fts_sched_remove_fd( fts_sched_get_current(), this->fd);

      close( this->fd);
    }
}

static fts_status_t serial_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[3];

  fts_class_init( cl, sizeof( serial_t), 1, 1, 0);

  t[0] = fts_t_symbol;
  t[1] = fts_t_symbol;  /* the device to use: "/dev/ttyd1" for instance */
  t[2] = fts_t_int;  /* the baud rate: 2400, 19200, 38400, 57600, 115200, etc */
  fts_method_define(cl, fts_SystemInlet, fts_s_init, serial_init, 3, t);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, serial_delete, 0, 0);

  t[0] = fts_t_int;
  fts_method_define( cl, 0, fts_type_get_selector(fts_t_int), serial_int, 1, t);

  fts_method_define( cl, 0, fts_s_bang, serial_bang, 0, 0);

  fts_method_define_varargs( cl, 0, fts_s_list, serial_list);

  t[0] = fts_t_int;
  fts_outlet_type_define( cl, 0, fts_type_get_selector( fts_t_int), 1, t);

  return fts_Success;
}

void serial_config( void)
{
  fts_class_install( fts_new_symbol("serial"), serial_instantiate);
}


