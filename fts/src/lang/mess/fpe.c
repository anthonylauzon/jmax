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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


/*
 * This code include support for fpe exceptions handling and reporting
 * in control computation; it keep an object set of objects that had
 * floating point exceptions.
 */

#include <stdio.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "lang/datalib.h"
#include "fpe.h"

static fts_object_set_t *fpe_objects = 0;

void fts_fpe_add_object( fts_object_t *object)
{
  if (fpe_objects)
    {
      fts_object_set_add( fpe_objects, object);
    }
}


/* Current version ignore exception type and count */

void fts_fpe_handler( int which)
{
  fts_object_t *obj;

  obj = dsp_get_current_object();

  if (!obj)
    obj = fts_get_current_object();

  if (obj)
    fts_fpe_add_object( obj);
}

void fts_fpe_empty_collection(void)
{
  if (fpe_objects)
    fts_object_set_remove_all(fpe_objects);
}


void fts_fpe_start_collect(fts_object_set_t *set)
{
  if (fpe_objects)
    return;

  fpe_objects = set;
  fts_enable_fpe_traps();
}



void fts_fpe_stop_collect(void)
{
  fts_disable_fpe_traps();
  fpe_objects = 0;
}


