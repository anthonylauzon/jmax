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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

/* Frequency to midi conversion */

#include <fts/fts.h>
#include <math.h>

typedef struct 
{
  fts_object_t ob;
} ftom_t;



static float
ftom(float f)
{
  if (f > 0)
    return (69.0f + (1.0f/.057762265f) * log(f/440.0f));
  else
    return (-1000);		/* ???? */
}

static void
ftom_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, ftom(fts_get_float(at)));
}

static void
ftom_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, ftom((float)fts_get_int(at)));
}

static fts_status_t
ftom_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  /* initialize the class */

  fts_class_init(cl, sizeof(ftom_t), 1, 1, 0); 

  /* methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, ftom_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, ftom_float, 1, a);

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_float, 1, a);

  return fts_Success;
}

void
ftom_config(void)
{
  fts_class_install(fts_new_symbol("ftom"),ftom_instantiate);
}


