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
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include <fts/fts.h>

/************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_symbol_t classname;
  fts_class_t *class;
} make_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
make_args(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  make_t *this = (make_t *)o;
  fts_object_t *obj = fts_object_create(this->class, NULL, ac, at);

  if(obj)
    {
      fts_atom_t a;

      fts_object_refer(obj);
      
      fts_set_object(&a, obj);
      fts_outlet_send(o, 0, this->classname, 1, &a);
      
      fts_object_release(obj);
    }
  else
    fts_object_signal_runtime_error(o, "error in object creation");
}

static void
make_set_classname(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  make_t *this = (make_t *)o;
  fts_symbol_t classname = fts_get_symbol(at);
  fts_class_t *class = fts_class_get_by_name(NULL, classname);

  if(class)
    {
      this->classname = classname;
      this->class = class;
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
make_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  make_t *this = (make_t *)o;
  
  this->classname = 0;
  this->class = 0;

  if(ac > 0 && fts_is_symbol(at))
    make_set_classname(o, 0, 0, 1, at);
  
  if(this->class == 0)
    fts_object_set_error(o, "invalid class name");
}

static void
make_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(make_t), make_init, NULL);

  fts_class_inlet_varargs(cl, 0, make_args);
  fts_class_inlet_symbol(cl, 1, make_set_classname);

  fts_class_outlet_varargs(cl, 0);
}

void
make_config(void)
{
  fts_class_install(fts_new_symbol("make"), make_instantiate);
}
