/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <stdarg.h>

#include <fts/fts.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/errobj.h>

/*******************************************************************
*
*  error object
*
*/
fts_class_t *fts_error_object_class = NULL;

static void
error_object_input_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* should we signal an error here? */
}

static void
error_object_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_object_t *this = (fts_error_object_t *)o;

  if(this->name != NULL && this->name != fts_s_empty_string)
  {
    fts_atom_t a;

    fts_set_symbol(&a, this->name);
    fts_client_send_message(o, fts_s_name, 1, &a);
  }
}

static void
error_object_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_object_t *this = (fts_error_object_t *)o;

  if(this->name != NULL && this->name != fts_s_empty_string)
  {
    fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
    fts_atom_t a;
    
    fts_set_symbol(&a, this->name);
    fts_dumper_send(dumper, fts_s_name, 1, &a);
  }
}

static void
error_object_set_name(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_object_t *this = (fts_error_object_t *)o;

  if(ac > 0 && fts_is_symbol(at))
  {
    this->name = fts_get_symbol(at);

    if(fts_object_has_id(o))
    {
      fts_atom_t a;

      fts_set_symbol(&a, this->name);
      fts_client_send_message(o, fts_s_name, 1, &a);
    }
  }
}

static void
error_object_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_object_t *this = (fts_error_object_t *)o;

  this->name = fts_s_empty_string;
  this->description = fts_get_symbol(at);

  fts_object_set_inlets_number(o, 0);
  fts_object_set_outlets_number(o, 0);
}

static void
error_object_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_error_object_t), error_object_init, NULL);

  fts_class_message_varargs(cl, fts_s_name, error_object_set_name);
  fts_class_message_varargs(cl, fts_s_update_gui, error_object_update_gui);
  fts_class_message_varargs(cl, fts_s_dump, error_object_dump);

  fts_class_input_handler(cl, error_object_input_handler);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);

  fts_dsp_declare_inlet(cl, 0);
}

fts_class_t *
fts_error_object_get_class(fts_error_object_t *obj)
{
  fts_object_t *o = (fts_object_t *)obj;
  fts_symbol_t class_name = NULL;
  fts_class_t *class = NULL;
  fts_package_t *package = NULL;
  int argc = fts_object_get_description_size(o);
  fts_atom_t *argv = fts_object_get_description_atoms(o);

  if(argc > 1 && fts_is_symbol(argv) && fts_get_symbol(argv) == fts_s_colon && fts_is_symbol(argv + 1))
    class_name = fts_get_symbol(argv + 1);
  else if(argc > 2 && 
          fts_is_symbol(argv) &&
          fts_is_symbol(argv + 1) && 
          fts_is_symbol(argv + 2) && 
          fts_get_symbol(argv + 1) == fts_s_colon)
  {
    fts_symbol_t package_name = fts_get_symbol(argv);
    package = fts_package_get(package_name);
    class_name = fts_get_symbol(argv + 2);
  }
  else if(argc > 0 && fts_is_symbol(argv))
    class_name = fts_get_symbol(argv);

  if(class_name != NULL)
  {
    if(package != NULL)
      class = fts_package_get_class(package, class_name);
    else
      class = fts_class_get_by_name(class_name);
  }
  
  if(class != NULL)
    return class;
  else
    return fts_error_object_class;
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

/*******************************************************************
 *
 *  unique global error symbol
 *
 */
static fts_symbol_t the_error = NULL;

void
fts_set_error(const char *format, ...)
{
  va_list ap;
  char buf[1024];

  /* make up the errdesc property  */
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  the_error = fts_new_symbol(buf);
}

fts_symbol_t
fts_get_error(void)
{
  return the_error;
}

/************************************************************
 *
 *  runtime error proxy
 *
 */
static fts_object_t *runtime_error_proxy = NULL;
static fts_symbol_t sym_runtime_error_post = NULL;

void 
fts_runtime_error_proxy_set(fts_object_t *obj)
{
  runtime_error_proxy = obj;
  sym_runtime_error_post = fts_new_symbol("postError");
}

void 
fts_runtime_error_proxy_remove(fts_object_t *obj)
{
  runtime_error_proxy = 0;
}

static void
fts_object_runtime_error(fts_object_t *obj, fts_symbol_t error)
{
  if(fts_object_get_patcher(obj) != NULL)
  {
    if(runtime_error_proxy)
    {
      fts_atom_t a[2];

      fts_set_object(a + 0, obj);
      fts_set_symbol(a + 1, error);

      fts_client_send_message(runtime_error_proxy, sym_runtime_error_post, 2, a);
    }
    else
    {
      fts_symbol_t class = fts_class_get_name(fts_object_get_class(obj));

      fts_post("error in %s: %s\n", class, error);
    }
  }
}

/************************************************************
*
*  object error (creation or runtime)
*
*/
void
fts_object_error(fts_object_t *obj, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_symbol_t error;

  /* make up the errdesc property  */
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  error = fts_new_symbol(buf);

  if (fts_object_get_status(obj) == FTS_OBJECT_STATUS_CREATE 
      || fts_object_get_status(obj) == FTS_OBJECT_STATUS_INVALID)
    {
      fts_object_set_status(obj, FTS_OBJECT_STATUS_INVALID);
      the_error = error;
    }
  else
    fts_object_runtime_error(obj, error);
}


/***********************************************************************
*
* Initialization
*
*/

void 
fts_kernel_error_object_init(void)
{
  fts_error_object_class = fts_class_install(NULL, error_object_instantiate);
}

