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
 */

#include <fts/fts.h>
#include <ftsconfig.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>
#include "delay.h"

/********************************************************
 *
 *  dcopy
 *
 */

static fts_symbol_t sym_dcopy = 0;
static fts_symbol_t sym_dpaste = 0;

typedef struct
{
  fts_object_t o;
  delayline_t *line;
} dcopy_t;

static void 
dcopy_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dcopy_t *this = (dcopy_t *)o;
  fvec_t *fvec = (fvec_t *)fts_get_object(at);
  delayline_t *delayline = this->line;
  float *buffer = delayline->buffer;
  int delay_size = delayline->delay_size;
  int ring_size = delayline->ring_size;
  int size = fvec_get_size(fvec);
  float *ptr = fvec_get_ptr(fvec);
  int onset, tail;
  int i, k;

  if(size > delay_size)
    size = delay_size;

  onset = delayline->phase - size;
  if(onset < 0)
    onset += ring_size; /* ring buffer wrap around */

  tail = ring_size - onset;

  if(tail > size)
    tail = size;

  /* copy fvec from delay line */
  for(i=0; i<tail; i++)
    ptr[i] = buffer[onset + i];

  for(k=0; i<size; k++, i++)
    ptr[i] = buffer[k];

  fts_outlet_object(o, 0, (fts_object_t *)fvec);
}

static void 
dpaste_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dcopy_t *this = (dcopy_t *)o;
  fvec_t *fvec = (fvec_t *)fts_get_object(at);
  delayline_t *delayline = this->line;
  float *buffer = delayline->buffer;
  int phase = delayline->phase;
  int drain_size = delayline->drain_size;
  int ring_size = delayline->ring_size;
  int size = fvec_get_size(fvec);
  float *ptr = fvec_get_ptr(fvec);
  int tail;
  int i;

  if(size > drain_size)
    size = drain_size;

  if(phase >= ring_size)
    phase -= ring_size;

  tail = ring_size - phase;

  /* add fvec to delay drain */
  if(tail > size)
    {
      for(i=0; i<size; i++)
	buffer[phase + i] += ptr[i];
    }
  else
    {
      int k;

      for(i=0; i<tail; i++)
	buffer[phase + i] += ptr[i];

      for(k=0; i<size; k++, i++)
	buffer[k] += ptr[i];
    }
}

static void
dcopy_set_line(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  dcopy_t *this = (dcopy_t *)o;
  delayline_t *line = (delayline_t *)fts_get_object(at);

  if(line != this->line)
    {
      if(this->line != NULL)
	fts_object_release((fts_object_t *)this->line);

      this->line = line;
      fts_object_refer((fts_object_t *)line);
    }
}

static void
dcopy_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  dcopy_t *this = (dcopy_t *)o;

  this->line = NULL;

  if(ac > 0 && fts_is_a(at, delayline_class))
    dcopy_set_line(o, 0, 0, 1, at);
  else
    {
      fts_object_error(o, "first argument must be a delay line");
      return;
    }
}

static void
dcopy_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  dcopy_t *this = (dcopy_t *)o;

  if(this->line != NULL)
    fts_object_release((fts_object_t *)this->line);
}

static void
dcopy_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dcopy_t), dcopy_init, dcopy_delete);

  fts_class_inlet(cl, 0, fvec_type, dcopy_fvec);
  fts_class_inlet(cl, 1, delayline_class, dcopy_set_line);

  fts_class_outlet(cl, 0, fvec_type);
}

static void
dpaste_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dcopy_t), dcopy_init, dcopy_delete);

  fts_class_inlet(cl, 0, fvec_type, dpaste_fvec);
  fts_class_inlet(cl, 1, delayline_class, dcopy_set_line);
}

void
signal_dpaste_config(void)
{
  sym_dcopy = fts_new_symbol("dcopy");
  sym_dpaste = fts_new_symbol("dpaste");

  fts_class_install(sym_dcopy, dcopy_instantiate);
  fts_class_install(sym_dpaste, dpaste_instantiate);
}
