/*
 * This code include support for fpe exceptions handling and reporting;
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "lang/datalib.h"
#include "fpe.h"

static fts_object_set_t *fpe_objects = 0;

/* Current version ignore exception type and count */


static void fpe_handler(int which)
{
  fts_object_t *obj = dsp_get_current_object();

  if (obj)
    fts_object_set_add(fpe_objects, obj);
  else
    {
      obj = fts_get_current_object();

      if (obj)
	fts_object_set_add(fpe_objects, obj);
    }
}

void fts_fpe_empty_collection()
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



void fts_fpe_stop_collect()
{
  fts_reset_fpe_handler();
  fpe_objects = 0;
}




