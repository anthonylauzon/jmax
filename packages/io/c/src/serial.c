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
 */
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <fts/fts.h>

#define DEFAULT_BUFFER_SIZE 128
#define DEFAULT_SERIAL_PORT_NAME "/dev/ttyS0"

static fts_symbol_t sym_5bits = 0;
static fts_symbol_t sym_6bits = 0;
static fts_symbol_t sym_7bits = 0;
static fts_symbol_t sym_8bits = 0;
static fts_symbol_t sym_2stop = 0;
static fts_symbol_t sym_odd = 0;
static fts_symbol_t sym_even = 0;
static fts_symbol_t sym_rtscts = 0;
static fts_symbol_t sym_xonoff = 0;
static fts_symbol_t sym_noread = 0;
static fts_symbol_t sym_b50 = 0;
static fts_symbol_t sym_b75 = 0;
static fts_symbol_t sym_b110 = 0;
static fts_symbol_t sym_b134 = 0;
static fts_symbol_t sym_b150 = 0;
static fts_symbol_t sym_b200 = 0;
static fts_symbol_t sym_b300 = 0;
static fts_symbol_t sym_b600 = 0;
static fts_symbol_t sym_b1200 = 0;
static fts_symbol_t sym_b1800 = 0;
static fts_symbol_t sym_b2400 = 0;
static fts_symbol_t sym_b4800 = 0;
static fts_symbol_t sym_b9600 = 0;
static fts_symbol_t sym_b19200 = 0;
static fts_symbol_t sym_b38400 = 0;
static fts_symbol_t sym_b57600 = 0;
static fts_symbol_t sym_b115200 = 0;
static fts_symbol_t sym_b230400 = 0;

typedef struct 
{
  fts_bytestream_t head;
  fts_symbol_t name;
  int fd;
  unsigned char *in_buf; /* input buffer */
  unsigned char *out_buf; /* output buffer */
  int out_fill; /* output buffer fill */
  int size; /* buffer size */
  struct termios termios; /* terminal settings */
  int noread; /* flag to desactivate input */
} serial_t;


/*********************************************************************
 *
 *  serial parameter settings
 *
 */

static void
serial_set_default_termios(serial_t *this)
{
  cfsetospeed(&this->termios, B9600);
  cfsetispeed(&this->termios, 0);

  this->termios.c_iflag = 0;
  this->termios.c_oflag = 0;
  this->termios.c_cflag = CS7 | CLOCAL | CREAD;
  this->termios.c_lflag = 0;
  this->termios.c_cc[VMIN] = 1; /* don't trigger select without at least one char ready */
  this->termios.c_cc[VTIME] = 0; /* no timer */
}

static void
serial_set_speed(serial_t *this, fts_symbol_t sym)
{
  speed_t speed;

  if(sym == sym_b50)
    speed = B50;
  else if(sym == sym_b75)
    speed = B75;
  else if(sym == sym_b110)
    speed = B110;
  else if(sym == sym_b134)
    speed = B134;
  else if(sym == sym_b150)
    speed = B150;
  else if(sym == sym_b200)
    speed = B200;
  else if(sym == sym_b300)
    speed = B300;
  else if(sym == sym_b600)
    speed = B600;
  else if(sym == sym_b1200)
    speed = B1200;
  else if(sym == sym_b1800)
    speed = B1800;
  else if(sym == sym_b2400)
    speed = B2400;
  else if(sym == sym_b4800)
    speed = B4800;
  else if(sym == sym_b9600)
    speed = B9600;
  else if(sym == sym_b19200)
    speed = B19200;
  else if(sym == sym_b38400)
    speed = B38400;
  else if(sym == sym_b57600)
    speed = B57600;
  else if(sym == sym_b115200)
    speed = B115200;
#ifdef B230400
  /* This is not defined on SGI */
  else if(sym == sym_b230400)
    speed = B230400;
#endif
  else
    {
      const char *name;

      name = sym;
      sscanf( name+1, "%d", &speed);
    }

  cfsetospeed(&this->termios, speed);
  cfsetispeed(&this->termios, 0);
}

static void
serial_set_flag(serial_t *this, fts_symbol_t sym)
{
  const char *name = sym;

  if(sym == sym_5bits)
    this->termios.c_cflag |= CS5;
  else if(sym == sym_6bits)
    this->termios.c_cflag |= CS6;
  else if(sym == sym_7bits)
    this->termios.c_cflag |= CS7;
  else if(sym == sym_8bits)
    this->termios.c_cflag |= CS8;
  else if(sym == sym_2stop)
    this->termios.c_cflag |= CSTOPB;
  else if(sym == sym_odd)
    this->termios.c_cflag |= (PARENB | PARODD);
  else if(sym == sym_even)
    this->termios.c_cflag |= PARENB;
#ifdef SGI
  /* This different on SGI */
  else if(sym == sym_rtscts)
    this->termios.c_cflag |= CNEW_RTSCTS;
#else
  else if(sym == sym_rtscts)
    this->termios.c_cflag |= CRTSCTS;
#endif
  else if(sym == sym_xonoff)
    this->termios.c_iflag |= (IXON | IXOFF);
  else if(sym == sym_noread)
    this->noread = 1;
  else if(name[0] == 'b')
    serial_set_speed(this, sym);
  else
    post("serial: invalid control flag: %s (ignored)\n", sym);
}
  
/*********************************************************************
 *
 *  read callback in FTS scheduler
 *
 */

static void 
serial_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;
  int size = this->size;
  int n_read;

  do
    {
      n_read = read(this->fd, this->in_buf, size);
      fts_bytestream_input((fts_bytestream_t *)o, n_read, this->in_buf);
    }
  while(n_read == size);
}

/*********************************************************************
 *
 *  bytestream interface
 *
 */

static void
serial_default_input(fts_object_t *o, int n, const unsigned char *c)
{
  serial_t *this = (serial_t *)o;
  int i;

  for(i=0; i<n; i++)
    fts_outlet_int(o, 0, c[i]);
}

static void
serial_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  serial_t *this = (serial_t *)stream;
  int size = this->size;
  int n_write = this->out_fill + n;
  int n_over = n_write - size;
  int n_wrote = 0;
  
  if(n_over > 0)
    {
      bcopy(c, this->out_buf + this->out_fill, n - n_over);
      n_wrote = write(this->fd, this->out_buf, this->size);
      
      bcopy(c, this->out_buf, n_over);
      this->out_fill = n_over;
    }
  else if(n_over == 0)
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      n_wrote = write(this->fd, this->out_buf, this->size);
      
      this->out_fill = 0;
    }
  else
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      this->out_fill = n_write;
      
      n_wrote = n;
    }
  
  if(n_wrote != n)
    post("serial %s: write error (%s)\n", this->name, strerror(errno));
}

static void
serial_output_char(fts_bytestream_t *stream, unsigned char c)
{
  serial_t *this = (serial_t *)stream;
  int size = this->size;
  int n_wrote = 0;
  
  this->out_buf[this->out_fill++] = c;
  
  if(this->out_fill >= size)
    {
      n_wrote = write(this->fd, this->out_buf, this->size);
      
      if(n_wrote != size)
	post("serial %s: write error (%s)\n", this->name, strerror(errno));

      this->out_fill = 0;
    }
  
}

static void
serial_flush(fts_bytestream_t *stream)
{
  serial_t *this = (serial_t *)stream;
  int n = this->out_fill;
  
  if(n > 0)
    {
      int n_wrote = write(this->fd, this->out_buf, n);
      
      if(n_wrote != n)
	post("serial %s: write error (%s)\n", this->name, strerror(errno));
    }

  this->out_fill = 0;
}

/*********************************************************************
 *
 *  methods
 *
 */
static void 
serial_int( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  if(fts_bytestream_is_output(&this->head))
    serial_output_char((fts_bytestream_t *)this, (unsigned char)fts_get_int(at));
}

static void 
serial_list( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  if(fts_bytestream_is_output(&this->head))
    {
      int i;
      
      for(i=0; i<ac; i++)
	if(fts_is_int(at + i))
	  serial_output_char(&this->head, (unsigned char)fts_get_int(at + i));
    }
}

static void 
serial_bang( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  serial_flush(&this->head);
}

/************************************************************
 *
 *  get bytestream variable
 *
 */
static void
serial_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  serial_t *this = (serial_t *)o;

  if(this->fd >= 0)
    fts_set_object(value, o);
  else
    fts_set_void(value);
}

/*********************************************************************
 *
 *  class 
 *
 */
static void 
serial_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  this->fd = -1;

  if(fts_is_symbol(at))
    {
      fts_symbol_t name = fts_get_symbol(at);
      speed_t speed;
      int i = 1; /* running argument index */
      int size = 0;
      
      if(fts_is_number(at + i))
	{
	  size = fts_get_number_int(at + i);
	  i++;
	}
      
      if(size <= 0)
	size = DEFAULT_BUFFER_SIZE;
      
      serial_set_default_termios(this);
      
      for(; i<ac; i++)
	{
	  if(fts_is_symbol(at + i))
	    serial_set_flag(this, fts_get_symbol(at + i));
	  else
	    {
	      post("serial: wrong argument: ");
	      post_atoms(1, at + i);
	      post(" (ignored)\n");
	    }
	}
      
      if(this->noread)
	this->termios.c_cflag &= ~CREAD;
      
      this->fd = open(name, O_RDWR);
      
      if(this->fd < 0)
	{
	  fts_object_set_error(o, "can't open serial port \"%s\" (%s)", name, strerror( errno));
	  return;
	}
      
      if(tcsetattr(this->fd, TCSANOW, &this->termios) < 0)
	{
	  fts_object_set_error(o, "can't set termios configuration (%s)", strerror(errno));
	  
	  close(this->fd);
	  return;
	}
      
      fts_bytestream_init(&this->head);
      
      if(!this->noread)
	{
	  this->in_buf = (unsigned char *)fts_malloc(size);
	  
	  /* add fd to FTS scheduler */
	  fts_sched_add( o, FTS_SCHED_READ, this->fd);
	  
	  /* set bytestream callback functions */
	  fts_bytestream_set_input(&this->head);
	  
	  /* install default input callback */
	  fts_bytestream_add_listener(&this->head, o, serial_default_input);
	}
      
      this->out_buf = (unsigned char *)fts_malloc(size);
      this->out_fill = 0;
      
      this->size = size;
      this->name = name;
      
      fts_bytestream_set_output(&this->head, serial_output, serial_output_char, serial_flush);
    }
  else
    fts_object_set_error(o, "First argument of serial device name required");
}

static void 
serial_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  serial_t *this = (serial_t *)o;

  if(this->fd >= 0)
    {
      fts_bytestream_remove_listener(&this->head, o);
      fts_sched_remove( o);
      close( this->fd);
    }
}

static fts_status_t 
serial_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( serial_t), 1, 1, 0);
  fts_bytestream_class_init(cl);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, serial_get_state);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, serial_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, serial_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_sched_ready, serial_read);

  fts_method_define_varargs(cl, 0, fts_s_bang, serial_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, serial_int);
  fts_method_define_varargs(cl, 0, fts_s_list, serial_list);

  return fts_ok;
}

void serial_config( void)
{
  sym_5bits = fts_new_symbol("5bits");
  sym_6bits = fts_new_symbol("6bits");
  sym_7bits = fts_new_symbol("7bits");
  sym_8bits = fts_new_symbol("8bits");
  sym_2stop = fts_new_symbol("2stop");
  sym_odd = fts_new_symbol("odd");
  sym_even = fts_new_symbol("even");
  sym_rtscts = fts_new_symbol("rtscts");
  sym_xonoff = fts_new_symbol("xonoff");
  sym_noread = fts_new_symbol("noread");
  sym_b50 = fts_new_symbol("b50");
  sym_b75 = fts_new_symbol("7");
  sym_b110 = fts_new_symbol("b110");
  sym_b134 = fts_new_symbol("b134");
  sym_b150 = fts_new_symbol("b150");
  sym_b200 = fts_new_symbol("b200");
  sym_b300 = fts_new_symbol("b300");
  sym_b600 = fts_new_symbol("b600");
  sym_b1200 = fts_new_symbol("b1200");
  sym_b1800 = fts_new_symbol("b1800");
  sym_b2400 = fts_new_symbol("b2400");
  sym_b4800 = fts_new_symbol("b4800");
  sym_b9600 = fts_new_symbol("b9600");
  sym_b19200 = fts_new_symbol("b19200");
  sym_b38400 = fts_new_symbol("b38400");
  sym_b57600 = fts_new_symbol("b57600");
  sym_b115200 = fts_new_symbol("b115200");
  sym_b230400 = fts_new_symbol("b230400");

  fts_class_install( fts_new_symbol("serial"), serial_instantiate);
}
