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

static void fpe_handler( int which)
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
  fts_set_fpe_handler(fpe_handler);
}



void fts_fpe_stop_collect(void)
{
  fts_reset_fpe_handler();
  fpe_objects = 0;
}


