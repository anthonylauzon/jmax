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


/* 
   The error_obj; it have no methods, no inlets, not outlets;
   it is created by the pat file loader when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).
*/

#include <stdarg.h>

#include <fts/fts.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>

fts_object_t *runtime_error_handler = 0;
fts_symbol_t sym_runtime_error_post = 0;
/*
 * The error object; actually, error object are patchers, marked as errors,
 * because they can change dinamically their number of inlets and outs.
 *
 */


/* Make an error object, with a description given as an array of atoms, 
   and a textual error description given in the printf format (format string
   plus a variable number of arguments) */

fts_object_t *fts_error_object_new(fts_patcher_t *parent, int ac, const fts_atom_t *at, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a;
  fts_object_t *obj;

  /* make new patcher object */
  obj = fts_object_create( patcher_class, parent, 0, 0);

  /* flag the patcher object as error */
  fts_patcher_set_error((fts_patcher_t *)obj);

  /* Make up the errdesc property (eat symbols !!) */
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  fts_set_symbol(&a, fts_new_symbol(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);

  /* Return the object  */
  return obj;
}

void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet)
{
  if (fts_object_get_inlets_number(obj) <= ninlet)
    fts_object_set_inlets_number(obj, ninlet + 1);
}


void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet)
{
  if (fts_object_get_outlets_number(obj) <= noutlet)
    fts_object_set_outlets_number(obj, noutlet + 1);
}


/* Convenience function to set the error state of an object, providing 
   also a textual description of the error as a printf like set of arguments
   */

void fts_object_set_error(fts_object_t *obj, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a;

  /* make up the error  property  */
  fts_set_int(&a, 1);
  fts_object_put_prop(obj, fts_s_error, &a);

  /* make up the errdesc property  */
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  fts_set_symbol(&a, fts_new_symbol(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);
}

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
      fts_symbol_t variable = fts_object_get_variable(obj);

      if(variable)
	post("error in %s $%s: %s\n", class, variable, buf);
      else
	post("error in %s: %s\n", class, buf);	
    }
}

fts_symbol_t
fts_object_get_error(fts_object_t *obj)
{
  fts_atom_t error_prop;
  
  fts_object_get_prop(obj, fts_s_error, &error_prop);
  
  if(fts_is_int(&error_prop) && fts_get_int(&error_prop) == 1)
    {
      fts_atom_t error_description_prop;
      
      fts_object_get_prop(obj, fts_s_error_description, &error_description_prop);
      
      if(fts_is_symbol(&error_description_prop))
	return fts_get_symbol(&error_description_prop);
    }

  return 0;
}
