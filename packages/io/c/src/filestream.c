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
#include <fts/fts.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

static fts_symbol_t sym_in = 0;
static fts_symbol_t sym_out = 0;

typedef struct _filestream_
{
  fts_bytestream_t head;
  fts_symbol_t name;
  int fd;
  unsigned char *in_buf; /* input buffer */
  int in_size;
  unsigned char *out_buf; /* output buffer */
  int out_size;
  int out_fill;
} filestream_t;

static void
filestream_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  filestream_t *this = (filestream_t *)o;
  int size = this->in_size;
  int n_read;

  n_read = read(this->fd, this->in_buf, size);
  fts_bytestream_input((fts_bytestream_t *)o, n_read, this->in_buf);

  while(n_read == size)
    {
      n_read = read(this->fd, this->in_buf, size);
      fts_bytestream_input((fts_bytestream_t *)o, n_read, this->in_buf);
    }
}

/************************************************************************
 *
 *  fts bytestream interface functions
 *
 */ 
static void
filestream_default_input(fts_object_t *o, int n, const unsigned char *c)
{
  filestream_t *this = (filestream_t *)o;
  int i;

  for(i=0; i<n; i++)
    fts_outlet_int(o, 0, c[i]);
}

static void
filestream_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  filestream_t *this = (filestream_t *)stream;
  int n_wrote = write(this->fd, c, n);

  if(n_wrote != n)
    fts_object_signal_runtime_error((fts_object_t *)stream, "write error for file %s", this->name);
}

static void
filestream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  filestream_t *this = (filestream_t *)stream;
  /* un-buffered output */
  int n_wrote = write(this->fd, &c, 1);
  
  if(n_wrote != 1)
    fts_object_signal_runtime_error((fts_object_t *)stream, "write error for file %s", this->name);
}

static void
filestream_output_buffered(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  filestream_t *this = (filestream_t *)stream;
  int out_size = this->out_size;
  int n_write = this->out_fill + n;
  int n_over = n_write - out_size;
  int n_wrote = 0;
  
  if(n_over > 0)
    {
      bcopy(c, this->out_buf + this->out_fill, n - n_over);
      n_wrote = write(this->fd, this->out_buf, this->out_size);
      
      bcopy(c, this->out_buf, n_over);
      this->out_fill = n_over;
    }
  else if(n_over == 0)
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      n_wrote = write(this->fd, this->out_buf, this->out_size);
      
      this->out_fill = 0;
    }
  else
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      this->out_fill = n_write;
      
      n_wrote = n;
    }
  
  if(n_wrote != n)
    fts_object_signal_runtime_error((fts_object_t *)stream, "write error for file %s", this->name);
}

static void
filestream_output_char_buffered(fts_bytestream_t *stream, unsigned char c)
{
  filestream_t *this = (filestream_t *)stream;
  int out_size = this->out_size;
  int n_wrote = 0;
  
  this->out_buf[this->out_fill++] = c;
  
  if(this->out_fill >= out_size)
    {
      n_wrote = write(this->fd, this->out_buf, this->out_size);
      
      if(n_wrote != out_size)
	fts_object_signal_runtime_error((fts_object_t *)stream, "write error for file %s", this->name);

      this->out_fill = 0;
    }
  
}

static void
filestream_flush(fts_bytestream_t *stream)
{
  filestream_t *this = (filestream_t *)stream;
  int n = this->out_fill;
  
  if(n > 0)
    {
      int n_wrote = write(this->fd, this->out_buf, n);
      
      if(n_wrote != n)
	fts_object_signal_runtime_error((fts_object_t *)stream, "write error for file %s", this->name);
    }

  this->out_fill = 0;
}

/************************************************************
 *
 *  methods
 *
 */

static void
filestream_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  filestream_t *this = (filestream_t *)o;

  if(fts_bytestream_is_output(&this->head))
    filestream_output_char(&this->head, (unsigned char)fts_get_int(at));
}

static void
filestream_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  filestream_t *this = (filestream_t *)o;

  if(fts_bytestream_is_output(&this->head))
    {
      int i;
      
      for(i=0; i<ac; i++)
	if(fts_is_int(at + i))
	  filestream_output_char(&this->head, (unsigned char)fts_get_int(at + i));
    }
}

      
static void
filestream_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  filestream_t *this = (filestream_t *)o;

  filestream_flush(&this->head);
}

static void
filestream_start(filestream_t *this)
{ 
  fts_bytestream_t *stream = (fts_bytestream_t *)this;

  /* open file */
  if(this->in_size > 0 && this->out_size > 0)
    this->fd = open(this->name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  else if(this->in_size > 0)
    this->fd = open(this->name, O_RDONLY);
  else if(this->out_size > 0)
    this->fd = open(this->name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  
  if(this->fd >= 0)
    {
      if(this->in_size > 0)
	{
	  /* add file descriptor for input select */
	  fts_sched_add((fts_object_t *)this, FTS_SCHED_READ, this->fd);
	  
	  /* set byte input */
	  fts_bytestream_set_input(stream);
	  
	  /* install default input callback */
	  fts_bytestream_add_listener(stream, (fts_object_t *)this, filestream_default_input);
	}
      
      if(this->out_size > 0)
	{
	  if(this->out_buf != NULL)
	    /* install buffered output functions */
	    fts_bytestream_set_output(stream, filestream_output_buffered, filestream_output_char_buffered, filestream_flush);
	  else
	    /* install un-buffered output functions */
	    fts_bytestream_set_output(stream, filestream_output, filestream_output_char, 0);
	}
    }
  else
    fts_object_signal_runtime_error((fts_object_t *)this, "cannot open file \"%s\" (%s)", this->name, strerror(errno));
}
  
static void
filestream_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    this->name = fts_get_symbol(at);
      
  if(this->name != NULL)
    {
      if(ac > 1)
	{
	  if(fts_is_number(at + 1))
	    this->in_size = fts_get_number_int(at + 1);
	  
	  if(ac > 2 && fts_is_number(at + 2))
	    this->out_size = fts_get_number_int(at + 2);
	}
      else
	this->in_size = this->out_size = 1;
      
      if(this->in_size > 0)
	this->in_buf = (unsigned char *)fts_malloc(this->in_size);
      
      if(this->out_size > 1)
	this->out_buf = (unsigned char *)fts_malloc(this->out_size);
      
      fts_bytestream_init((fts_bytestream_t *)this);
      filestream_start(this);
    }
}

static void
filestream_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;

  if(this->fd >= 0)
    {
      close(this->fd);
      fts_bytestream_remove_listener((fts_bytestream_t *)this, o);
      fts_sched_remove( (fts_object_t *)this);
    }  
}

/************************************************************
 *
 *  class
 *
 */

static void
filestream_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;

  this->name = NULL;
  this->fd = -1;
  this->in_buf = NULL;
  this->in_size = 0;
  this->out_buf = NULL;
  this->out_size = 0;
  this->out_fill = 0;

  if(ac > 0)
    {
      if(ac > 1)
	{
	  /* syntax: filestream name [<in buffer size> [<out buffer size>]] */
	  if(fts_is_number(at + 1))
	    this->in_size = fts_get_number_int(at + 1);
	  
	  if(ac > 2 && fts_is_number(at + 2))
	    this->out_size = fts_get_number_int(at + 2);
	}
      else
	this->in_size = this->out_size = 1;
      
      filestream_open(o, 0, 0, ac, at);
    }
}

static void 
filestream_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;

  filestream_close(o, 0, 0, 0, 0);
}

/************************************************************
 *
 *  get bytestream variable
 *
 */
static void
filestream_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  filestream_t *this = (filestream_t *)o;

  if(this->fd >= 0)
    fts_set_object(value, o);
}

/************************************************************
 *
 *  class
 *
 */
static void
filestream_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(filestream_t), filestream_init, filestream_delete);
  fts_bytestream_class_init(cl);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, filestream_get_state);
  fts_class_message_varargs(cl, fts_s_sched_ready, filestream_read);
  
  fts_class_message_varargs(cl, fts_s_open, filestream_open);
  fts_class_message_varargs(cl, fts_s_close, filestream_close);
  fts_class_message_varargs(cl, fts_s_bang, filestream_bang);
  fts_class_inlet_int(cl, 0, filestream_int);
  fts_class_inlet_varargs(cl, 0, filestream_list);

  fts_class_outlet_int(cl, 0);
}

void
filestream_config(void)
{
  sym_in = fts_new_symbol("in");
  sym_out = fts_new_symbol("out");

  fts_class_install(fts_new_symbol("filestream"), filestream_instantiate);
}
