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


/* 
   The clipboard; it have no methods, no inlets, not outlets;
   keep a reference to an unlinked file storing the clipboard content.
   Only one file is created during the session, never closed until
   the quit.

   It is manipulated directly by the user interface by means of messages
   to the object itself.
*/

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PROCESS_H
#include <process.h>
#endif

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/selection.h>

typedef struct {
  fts_object_t ob;
  FILE *file;
} fts_clipboard_t;


static void
clipboard_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
#if WIN32  
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  this->file = tmpfile();
#else
  const char *name;
  char buf[1024];
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (ac == 0)
    name = "clipboard";
  else
    name = fts_get_symbol(at);

  sprintf(buf, "/tmp/jmax.clipboard.%d", getpid());

  this->file = fopen(buf, "w+b");

  /* Note that according to the UNIX file semantic, the file
     will not be removed by the next call, just its name
     is removed; the file will be automatically destroyed after
     the fclose, at the object destruction, or in case of FTS crash/quit.
     Note that this will not work on Windows :-> (but who care, anyway) */
     
  unlink(buf);
#endif
}


static void
clipboard_paste_in(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;
  fts_object_t *dummy;

  if (this->file)
    fts_bmax_filedesc_load(this->file, fts_get_object(at), 0, 0, &dummy);
}


static void
clipboard_copy_selection(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  if (this->file)
    fts_save_selection_as_bmax(this->file, fts_get_object(at));
}

static void
clipboard_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_clipboard_t *this  = (fts_clipboard_t *) o;

  fclose(this->file);
}

static void
clipboard_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_clipboard_t), clipboard_init, clipboard_delete);

  fts_class_message_varargs(cl, fts_s_paste,  clipboard_paste_in);
  fts_class_message_varargs(cl, fts_s_copy,   clipboard_copy_selection);
}


/***********************************************************************
 *
 * Initialization
 *
 */

FTS_MODULE_INIT(clipboard)
{
  fts_class_install(fts_new_symbol("__clipboard"), clipboard_instantiate);
}


