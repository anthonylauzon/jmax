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
 */


#include <stdarg.h>

#include <fts/fts.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/errobj.h>

fts_class_t *fts_error_object_class;

fts_object_t *runtime_error_handler = 0;
fts_symbol_t sym_runtime_error_post = 0;

static void
error_object_default_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* should we signal an error here? */
}

static void
error_object_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_object_t *this = (fts_error_object_t *)o;

  this->description = fts_get_symbol(at);
}

static void
error_object_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_error_object_t), error_object_init, NULL);

  fts_class_set_default_handler(cl, error_object_default_handler);
}

void
fts_error_object_fit_inlet(fts_object_t *obj, int ninlet)
{
  if (fts_object_get_inlets_number(obj) <= ninlet)
    fts_object_set_inlets_number(obj, ninlet + 1);
}


void 
fts_error_object_fit_outlet(fts_object_t *obj, int noutlet)
{
  if (fts_object_get_outlets_number(obj) <= noutlet)
    fts_object_set_outlets_number(obj, noutlet + 1);
}

/************************************************************
 *
 *  convenience function to set the error state of an object, providing 
 *  also a textual description of the error as a printf like set of arguments
 *
 */
void 
fts_object_set_error(fts_object_t *obj, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a;

  /* make up the errdesc property  */
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  fts_set_symbol(&a, fts_new_symbol(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);
}

/************************************************************
 *
 *  runtime errors
 *
 */
void 
fts_runtime_error_handler_set(fts_object_t *obj)
{
  runtime_error_handler = obj;
  
  if(!sym_runtime_error_post) 
    sym_runtime_error_post = fts_new_symbol("postError");
}

void 
fts_runtime_error_handler_remove(fts_object_t *obj)
{
  runtime_error_handler = 0;
}

void 
fts_error(fts_object_t *obj, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a[2];

  va_start(ap, format);
  vsnprintf( buf, sizeof(buf), format, ap);
  va_end(ap);

  if(runtime_error_handler)
    {
      fts_set_object(a + 0, obj);
      fts_set_symbol(a + 1, fts_new_symbol(buf));
      /* fts_set_string(a + 1, buf); ??? */
      
      fts_client_send_message(runtime_error_handler, sym_runtime_error_post, 2, a);
    }
  else
    {
      fts_symbol_t class = fts_class_get_name(fts_object_get_class(obj));

      post("error in %s: %s\n", class, buf);	
    }
}

void 
fts_object_signal_runtime_error(fts_object_t *obj, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a[2];

  va_start(ap, format);
  vsnprintf( buf, sizeof(buf), format, ap);
  va_end(ap);

  if(runtime_error_handler)
    {
      fts_set_object(a + 0, obj);
      fts_set_symbol(a + 1, fts_new_symbol(buf));
      /* fts_set_string(a + 1, buf); ??? */
      
      fts_client_send_message(runtime_error_handler, sym_runtime_error_post, 2, a);
    }
  else
    {
      fts_symbol_t class = fts_class_get_name(fts_object_get_class(obj));

      post("error in %s: %s\n", class, buf);	
    }
}

fts_symbol_t
fts_object_get_error(fts_object_t *obj)
{
  fts_atom_t error_description_prop;
  
  fts_object_get_prop(obj, fts_s_error_description, &error_description_prop);
  
  if(fts_is_symbol(&error_description_prop))
    return fts_get_symbol(&error_description_prop);

  return 0;
}

void fts_error_object_config(void)
{
  fts_error_object_class = fts_class_install(NULL, error_object_instantiate);
}

