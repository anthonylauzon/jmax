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
#include <fcntl.h>
#include <errno.h>


static fts_symbol_t sym_in = 0;
static fts_symbol_t sym_out = 0;

typedef struct _filestream_
{
  fts_bytestream_t head;
  fts_symbol_t name;
  FILE* fd;
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

  do
    {
      n_read = fread(this->in_buf, 1, size, this->fd);

      fts_bytestream_input((fts_bytestream_t *)o, n_read, this->in_buf);
    }      
  while(n_read == size);
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
  int n_wrote = fwrite(&c, 1, n, this->fd);

  if(n_wrote != n)
    post("filestream %s: write error (%s)\n", this->name, strerror(errno));
}

static void
filestream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  filestream_t *this = (filestream_t *)stream;
  /* un-buffered output */
  int n_wrote = fwrite(&c, 1, 1, this->fd);
  
  if(n_wrote != 1)
    post("filestream %s: write error (%s)\n", this->name, strerror(errno));
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
      n_wrote = fwrite(this->out_buf, 1, this->out_size, this->fd);
      
      bcopy(c, this->out_buf, n_over);
      this->out_fill = n_over;
    }
  else if(n_over == 0)
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      n_wrote = fwrite(this->out_buf, 1, this->out_size, this->fd);
      
      this->out_fill = 0;
    }
  else
    {
      bcopy(c, this->out_buf + this->out_fill, n);
      this->out_fill = n_write;
      
      n_wrote = n;
    }
  
  if(n_wrote != n)
    post("filestream %s: write error (%s)\n", this->name, strerror(errno));
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
      n_wrote = fwrite(this->out_buf, 1, this->out_size, this->fd);
      
      if(n_wrote != out_size)
	post("filestream %s: write error (%s)\n", this->name, strerror(errno));

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
      int n_wrote = fwrite(this->out_buf, 1, n, this->fd);
      
      if(n_wrote != n)
	post("filestream %s: write error (%s)\n", this->name, strerror(errno));
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

/************************************************************
 *
 *  class
 *
 */

static void
filestream_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
      
  if(name)
    {
      int in = 0;
      int out = 0;
      int in_size = 0;
      int out_size = 0;
      FILE* fd = NULL;

      this->fd = NULL;
      this->in_buf = 0;
      this->in_size = 0;
      this->out_buf = 0;
      this->out_size = 0;
      this->out_fill = 0;

      if(ac == 2)
	{
	  in = 1;
	  out = 1;
	}
      else
	{
	  int i = 2;

	  /* syntax: filestream [in [<in buffer size>]] [out [<out buffer size>]] */
	  while(i < ac && fts_is_symbol(at + i))
	    {
	      fts_symbol_t sym = fts_get_symbol(at + i);
	      i++;
	      
	      if(sym == sym_in)
		{
		  in = 1;
		  
		  if(fts_is_number(at + i))
		    {
		      in_size = fts_get_number_int(at + i);
		      i++;
		    }
		}
	      else if(sym == sym_out)
		{
		  out = 1;
		  
		  if(fts_is_number(at + i))
		    {
		      out_size = fts_get_number_int(at + i);
		      i++;
		    }
		}
	      else
		return;
	    }
	  
	  /* syntax: filestream [<in buffer size> [<out buffer size>]] */
	  if(!in && !out && fts_is_number(at + i))
	    {
	      in = 1;
	      out = 1;
	      
	      in_size = fts_get_number_int(at + i);
	      i++;
	      
	      if(fts_is_number(at + i))
		out_size = fts_get_number_int(at + i);
	    }
	}

      /* allocate buffers */
      if(in_size <= 0)
	in_size = 1;

      if(in)
	{
	  this->in_buf = (unsigned char *)fts_malloc(in_size);
	  this->in_size = in_size;
	}

      if(out && out_size)
	{
	  this->out_buf = (unsigned char *)fts_malloc(out_size);
	  this->out_size = out_size;
	}

      /* open file */
      if(in && out)
	fd = fopen(name, "w+b");
      else if(in)
	fd = fopen(name, "rb");
      else if(out)
	fd = fopen(name, "wb");
      
      if(fd == NULL)
	{
	  fts_object_set_error(o, "filestream: Can't open file \"%s\" (%s)\n", name, strerror(errno));
	  return;
	}

      fts_bytestream_init(&this->head);
	  
      if(in)
	{
	  /* add file descriptor for input select */
	  fts_sched_add( o, FTS_SCHED_READ, fd);

	  /* set byte input */
	  fts_bytestream_set_input(&this->head);

	  /* install default input callback */
	  fts_bytestream_add_listener(&this->head, o, filestream_default_input);
	}

      if(out)
	{
	  if(this->out_buf)
	    {
	      /* install buffered output functions */
	      fts_bytestream_set_output(&this->head, filestream_output_buffered, filestream_output_char_buffered, filestream_flush);
	    }
	  else
	    {
	      /* install un-buffered output functions */
	      fts_bytestream_set_output(&this->head, filestream_output, filestream_output_char, 0);
	    }
	}
      
      this->fd = fd;
      this->name = name;
    }
  else
    {
      this->fd = NULL;
      this->name = 0;

      this->in_buf = 0;
      this->out_buf = 0;
   }
}

static void 
filestream_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  filestream_t *this = (filestream_t *)o;

  if(this->fd != NULL)
    {
      fclose(this->fd);
      fts_bytestream_remove_listener(&this->head, o);
      fts_sched_remove( (fts_object_t *)this);
    }
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

  if(this->fd != NULL)
    fts_set_object(value, o);
  else
    fts_set_void(value);
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
filestream_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(filestream_t), 1, 1, 0);
  fts_bytestream_class_init(cl);

  /* define variable */
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, filestream_get_state);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, filestream_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, filestream_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, filestream_read);

  fts_method_define_varargs(cl, 0, fts_s_bang, filestream_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, filestream_int);
  fts_method_define_varargs(cl, 0, fts_s_list, filestream_list);

  return fts_Success;
}

void
filestream_config(void)
{
  sym_in = fts_new_symbol("in");
  sym_out = fts_new_symbol("out");

  fts_class_install(fts_new_symbol("filestream"), filestream_instantiate);
}
