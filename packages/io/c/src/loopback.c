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

#include <fts/fts.h>

typedef struct _loopback_
{
  fts_bytestream_t head;
  int lock;
} loopback_t;

/************************************************************************
 *
 *  fts bytestream interface functions
 *
 */ 
static void
loopback_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  loopback_t *this = (loopback_t *)stream;
  int i;

  if(!this->lock)
    {
      this->lock = 1;
      fts_bytestream_input(stream, n, c);
      this->lock = 0;
    }
  else
    post("loopback: loop detected!\n");

  for(i=0; i<n; i++)
    fts_outlet_int((fts_object_t *)stream, 0, (int)c[i]);
}

static void
loopback_output_char(fts_bytestream_t *stream, unsigned char c)
{
  loopback_t *this = (loopback_t *)stream;

  if(!this->lock)
    {
      this->lock = 1;
      fts_bytestream_input(stream, 1, &c);
      this->lock = 0;
    }
  else
    post("loopback locked!\n");

  fts_outlet_int((fts_object_t *)stream, 0, (int)c);
}

/************************************************************
 *
 *  methods
 *
 */
static void
loopback_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_bytestream_t *stream = (fts_bytestream_t *)o;
  unsigned char c = fts_get_int(at);

  loopback_output_char(stream, c);
}

static void
loopback_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_bytestream_t *stream = (fts_bytestream_t *)o;
  unsigned char c = fts_get_float(at);

  loopback_output_char(stream, c);
}

#define LIST_SIZE 128

static void
loopback_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_bytestream_t *stream = (fts_bytestream_t *)o;

  while(ac > 0)
    {
      unsigned char buf[LIST_SIZE];
      int i, n;

      if(ac > LIST_SIZE)
	n = LIST_SIZE;
      else
	n = ac;
      
      for(i=0; i<n; i++)
	if(fts_is_number(at + i))
	   buf[i] = (unsigned char)fts_get_number_int(at + i);
      
      loopback_output(stream, n, buf);
      
      at += n;
      ac -= n;
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
loopback_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  loopback_t *this = (loopback_t *)o;
  fts_bytestream_t *stream = (fts_bytestream_t *)o;

  this->lock = 0;

  fts_bytestream_init(stream);
  fts_bytestream_set_input(stream);
  fts_bytestream_set_output(stream, loopback_output, loopback_output_char, 0);
}

static void 
loopback_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_bytestream_t *stream = (fts_bytestream_t *)o;
}

/************************************************************
 *
 *  get bytestream variable
 *
 */
static void
loopback_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, o);
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
loopback_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(loopback_t), 1, 1, 0);

  fts_bytestream_class_init(cl);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, loopback_init);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, loopback_get_state);
  
  fts_method_define_varargs(cl, 0, fts_s_int, loopback_int);
  fts_method_define_varargs(cl, 0, fts_s_float, loopback_float);
  fts_method_define_varargs(cl, 0, fts_s_list, loopback_list);
  
  return fts_ok;
}

void
loopback_config(void)
{
  fts_class_install(fts_new_symbol("loopback"), loopback_instantiate);
}
