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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <fts/fts.h>
#include <ftsconfig.h>
#include "Rsend.h"

typedef struct 
{
  fts_object_t _ob;
  fts_array_t output;
} Rsend_t;

/**********************************************************
 *
 *  Rsend is a historical object to send messages between two patches via MIDI.
 *  An Rreceive object has to be used to receive them (see Rreceive.c).
 *  The message selector is interpreted (by Rreceive) as a target.
 *  Rsend has to be connected to a sysexout object.
 *
 *  message to sysex block encoding:
 *
 *  head:
 *
 *    0xF0 (added by sysexout)
 *    0x7F (sysex realtime id)
 *    <bytes of receive name as string> 
 *    NULL 
 *
 *  for each atom of the message:
 *
 *    <type id>
 *    <bytes of atom as string>
 *    NULL
 *
 *  end:
 *
 *    0xF7 (added by sysexout)
 *
 */

static void
Rsend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rsend_t *this = (Rsend_t *)o;
  const char *name = fts_symbol_name(s);
  char string[STRLENMAX];
  fts_atom_t a;
  int i, j;

  fts_array_set_size(&this->output, 0);

  /* write sysex real-time message id 0x7F */
  fts_set_int(&a, 0x7F);
  fts_array_append(&this->output, 1, &a);

  /* write selector name */
  for(j=0; j<=strlen(name); j++)
    {
      fts_set_int(&a, name[j]);
      fts_array_append(&this->output, 1, &a);
    }

  for(i=0; i<ac; i++)
    {
      if (fts_is_int(at + i))
	{
	  string[0] = (char)RSEND_LONG;
	  snprintf(string + 1, STRLENMAX, "%d", fts_get_int(at + i));
	}
      else if (fts_is_float(at + i))
	{
	  string[0] = (char)RSEND_FLOAT;
	  snprintf(string + 1, STRLENMAX, "%#f", fts_get_float(at + i));
	}
      else if (fts_is_symbol(at + i))
	{
	  string[0] = (char)RSEND_SYM;
	  snprintf(string + 1, STRLENMAX, "%s", fts_symbol_name(fts_get_symbol(at + i)));
	}
      else
	{
	  string[0] = (char)RSEND_SYM;
	  snprintf(string + 1, STRLENMAX, "?");
	}

      for(j=0; j<=strlen(string); j++)
	{
	  fts_set_int(&a, string[j]);
	  fts_array_append(&this->output, 1, &a);
	}
    }

  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->output), fts_array_get_atoms(&this->output));
}

static void
Rsend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rsend_t *this = (Rsend_t *)o;
  
  fts_array_init(&this->output, 0, 0);
}

static void
Rsend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rsend_t *this = (Rsend_t *)o;

  fts_array_destroy(&this->output);
}

static fts_status_t
Rsend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(Rsend_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, Rsend_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, Rsend_delete);

  fts_method_define_varargs(cl, 0, fts_s_anything, Rsend_anything);

  return fts_Success;
}


void
Rsend_config(void)
{
  fts_class_install(fts_new_symbol("Rsend"), Rsend_instantiate);
}
