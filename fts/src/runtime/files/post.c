/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* This file include all the kernel provided post like
   (client printing) functions.
*/

#include "sys.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "lang.h"
#include "runtime/time.h"
#include "runtime/devices.h"
#include "runtime/client.h"
#include "runtime/files/post.h"


/******************************************************************************/
/*                                                                            */
/*              Basic Post Functions                                          */
/*                                                                            */
/******************************************************************************/

/*
   Due to limitation in the handling of  string type argument,
   post cannot be buffer events, must be immediate events.
*/

void
post_vector(int n, float *fp)
{
  int i;
  
  for (i = 0; i < n; i+=4)
    post("%4d: %f %f %f %f\n", i, fp[i], fp[i+1], fp[i+2], fp[i+3]);

  post("\n");
}

void
post_atoms(int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      char *ps;

      if (i == (ac-1))
	ps = "";
      else
	ps = " ";

      if (fts_is_symbol(&at[i]))
	post("%s%s", fts_symbol_name(fts_get_symbol(&at[i])), ps);
      else if (fts_is_long(&at[i]))
	post("%d%s", fts_get_long(&at[i]), ps);
      else if (fts_is_float(&at[i]))
	post("%f%s", fts_get_float(&at[i]), ps);
      else if (fts_is_ptr(&at[i]) )
	post("%lx%s", (unsigned long) fts_get_ptr( &at[i]), ps);
      else if (fts_is_void(&at[i]))
	post("(void)%s", ps);
      else if (fts_is_data(&at[i]))
	post("{<%s>}%s", fts_symbol_name(fts_data_get_class_name(fts_get_data(&at[i]))), ps);
      else
	post("<%s>%s", fts_symbol_name(fts_get_type(&at[i])), ps);
    }
}


void post( const char *format, ...)
{
  va_list ap;
  char buf[512];

  va_start( ap, format);

  vsprintf(buf, format, ap);

  va_end(ap);

  fts_client_mess_start_msg(POST_CODE);
  fts_client_mess_add_string(buf);
  fts_client_mess_send_msg();
}



/* 
 * The way to post object error message is to use this function;
 * Same syntax as post, add a first object argument; this argument
 * will be used to allow the user to retrieve the patch where the 
 * objetc is.
 *
 * Currently, the strategy is to use the error and errdesc properties;
 * it is actually a temporary hack, a special "console" should be written.
 * 
 */

void post_error(fts_object_t *obj, const char *format , ...)
{
  fts_atom_t a;
  va_list ap;
  char buf[512];

  va_start( ap, format);

  vsprintf(buf, format, ap);

  va_end(ap);

  fts_set_int(&a, 1);
  fts_object_put_prop(obj, fts_s_error, &a);

  if (obj->id != FTS_NO_ID)
    fts_object_ui_property_changed(obj, fts_s_error);

  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);

  fts_client_mess_start_msg(POST_CODE);
  fts_client_mess_add_string(buf);
  fts_client_mess_send_msg();
}


