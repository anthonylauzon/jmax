/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/* 
   The clipboard; it have no methods, no inlets, not outlets;
   it is created by the patparser when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).

   It should not be an object !!!!
*/

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/clipboard.h"

typedef struct 
{
  fts_object_t ob;

  FILE *file;

} fts_clipboard_t;


static void
clipboard_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const char *tmp;
  const char *name;
  char buf[1024];
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (ac == 1)
    name = "clipboard";
  else
    name = fts_symbol_name(fts_get_symbol(at));

  tmp = getenv ("JMAX_TMP");

  if (tmp)
    sprintf(buf, "%s/jmax.%s.%d", tmp, name, getpid());
  else
    sprintf(buf, "/tmp/jmax.%s.%d", name, getpid());

  this->file = fopen(buf, "w+");

  /* Note that according to the UNIX file semantic, the file
     will not be removed by the next command, just its name
     is removed; the file will be automatically destroyed after
     the fclose, at the object destruction, or in case of FTS crash/quit.
     Note that this will not work on Windows :-> (but who care, anyway) */
     
  unlink(buf);
}


static void
clipboard_paste_in(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (this->file)
    fts_binary_filedesc_load(this->file, fts_get_object(at), 0, 0, 0);
}



static void
clipboard_copy_selection(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (this->file)
    fts_save_selection_as_bmax(this->file, fts_get_object(at));
}


static void
clipboard_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  fclose(this->file);
}

static fts_status_t
clipboard_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(fts_clipboard_t), 0, 0, 0); 

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init,  clipboard_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete,  clipboard_delete, 0, 0);

  a[0] = fts_s_object;
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("paste"),  clipboard_paste_in, 1, a);

  a[0] = fts_s_object;
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("copy"),  clipboard_copy_selection, 1, a);

  return fts_Success;
}


void fts_clipboard_config(void)
{
  fts_class_install(fts_new_symbol("__clipboard"), clipboard_instantiate);
}

