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
   The clipboard; it have no methods, no inlets, not outlets;
   keep a reference to an unlinked file storing the clipboard content.
   Only one file is created during the session, never closed until
   the quit.

   It is manipulated directly by the user interface by means of messages
   to the object itself.
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PROCESS_H
#include <process.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/saver.h>

typedef struct {
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

  this->file = fopen(buf, "w+b");

  /* Note that according to the UNIX file semantic, the file
     will not be removed by the next call, just its name
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


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_clipboard_init(void)
{
  fts_class_install(fts_new_symbol("__clipboard"), clipboard_instantiate);
}

