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
 * Smooth Overlap Granular Synthesis class
 * 2001 by Norbert Schnell
 */
#include <stdlib.h>
#include <math.h>
#include <fts/fts.h>
#include <fts/packages/data/data.h>

#define WINDOW_BITS 12
#define WINDOW_SIZE (1 << WINDOW_BITS)

#define random_get(min, max) ((min) + (((max) - (min)) * rand() / RAND_MAX))
#define cent_to_linear(x) (exp(0.00057762265047 * (x)))

static fts_symbol_t sym_sogs = 0;
static fts_symbol_t sym_begin = 0;
static fts_symbol_t sym_end = 0;

static float * restrict sogs_window = 0;

typedef struct _sogs_params_
{
  /* control parameters */
  double onset;
  double onset_var;
  double pitch;
  double pitch_var;
  double duration;

  /* running */
  double target;
  double speed;

  /* misc */
  int n_overlap;
  float scale; /* 2 / n_overlap */
  fvec_t *fvec;

  /* state */
  fts_intphase_t wind_phi; /* window phase */
  fts_intphase_t wind_incr; /* window phase increment */

  /* const */
  double sr;

  fts_object_t *o;
} sogs_params_t;

static double
clip_to_vector(double value, fvec_t *fvec)
{
  double size = fvec_get_size(fvec);

  if(value < 0.0)
    value = 0.0;
  else if(value > size)
    value = size;

  return value;
}

static void
sogs_params_reset(sogs_params_t *params, double sr, int n_tick)
{
  sr *= 0.001; /* convert to samples per msec */

  if(params->sr == 1.0)
  {
    params->target *= sr;
    params->onset_var *= sr;
    params->duration *= sr;
  }

  params->onset = params->target;
  params->onset = clip_to_vector(params->onset, params->fvec);

  if(params->duration < 4.0)
    params->duration = 4.0;

  params->wind_phi = 0;
  params->wind_incr = (double)FTS_INTPHASE_RANGE / params->duration;

  params->sr = sr;
}

/************************************************
*
*    utils
*
*/

typedef struct _sogs_segment_
{
  fts_idefix_t index; /* fractional sample index */
  fts_idefix_t incr; /* fractional sample incr */
  fts_intphase_t wind; /* window phase */
  double phase;
} sogs_segment_t;

static void
sogs_segment_init(sogs_segment_t *seg, int i)
{
  fts_idefix_set_zero(&seg->index);
  fts_idefix_set_zero(&seg->incr);

  {
    int wind = 0;
    int w = FTS_INTPHASE_RANGE / 2;
    double phase = 0.0;
    double p = 0.5;

    while(i)
    {
      if(i & 1)
      {
        wind += w;
        phase += p;
      }

      i /= 2;
      w /= 2;
      p *= 0.5;
    }

    seg->wind = wind;
    seg->phase = phase;
  }
}

static void
sogs_segment_set(sogs_segment_t *seg, sogs_params_t *params, int i)
{
  double seg_transposition = cent_to_linear(params->pitch + random_get(-params->pitch_var, params->pitch_var));
  double seg_onset = params->onset + params->speed * i - 0.5 * seg_transposition * params->duration + random_get(-params->onset_var, params->onset_var);

  fts_idefix_set_float(&seg->index, seg_onset);
  fts_idefix_set_float(&seg->incr, seg_transposition);
}

/************************************************
*
*    sogs
*
*/

typedef struct
{
  fts_dsp_object_t o;
  ftl_data_t segs;
  ftl_data_t params;
  int max_overlap;
} sogs_t;

static void
sogs_bang_at_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 1);
}

/************************************************
*
*    dsp
*
*/

static void
sogs_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  sogs_segment_t *segs = ftl_data_get_ptr(this->segs);
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  fts_atom_t a[6];
  int i;

  sogs_params_reset(params, sr, n_tick);

  for(i=0; i<this->max_overlap; i++)
    sogs_segment_set(segs + i, params, 0);

  fts_set_symbol(a + 0, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(a + 1, this->segs);
  fts_set_ftl_data(a + 2, this->params);
  fts_set_int(a + 3, n_tick);
  fts_dsp_add_function(sym_sogs, 4, a);
}

void
ftl_sogs(fts_word_t *a)
{
  float * restrict out = (float *)fts_word_get_pointer(a + 0);
  sogs_segment_t *segs = fts_word_get_pointer(a + 1);
  sogs_params_t *params = fts_word_get_pointer(a + 2);
  int n_tick = fts_word_get_int(a + 3);
  fts_intphase_t wind_phi = params->wind_phi;
  fts_intphase_t wind_incr = params->wind_incr;
  float * restrict samples = fvec_get_ptr(params->fvec);
  float scale = params->scale;
  int size = fvec_get_size(params->fvec);
  int i, j;

  for(i=0; i<n_tick; i++)
    out[i] = 0.0;

  if(size >= 4)
  {
    for(i=0; i<params->n_overlap; i++)
    {
      sogs_segment_t * restrict seg = segs + i;
      fts_idefix_t index = seg->index;
      fts_idefix_t incr = seg->incr;
      fts_intphase_t phi = wind_phi - seg->wind;

      for(j=0; j<n_tick; j++)
      {
        phi = fts_intphase_wrap(phi);

        if(phi < wind_incr)
        {
          sogs_segment_set(seg, params, j);

          index = seg->index;
          incr = seg->incr;
        }

        if(fts_idefix_get_index(index) >= 0 && fts_idefix_get_index(index) < size)
        {
          float f;

          fts_cubic_idefix_interpolate(samples, index, &f);
          out[j] += f * sogs_window[fts_intphase_get_index(phi, WINDOW_BITS)] * scale;
        }

        fts_idefix_incr(&index, incr);

        phi += wind_incr;
      }

      seg->index = index;

      if(i == 0)
        params->wind_phi = phi;
    }

    if(params->speed != 0.0)
    {
      params->onset += params->speed * n_tick;

      if((params->speed > 0.0 && params->onset >= params->target) ||
         (params->speed < 0.0 && params->onset <= params->target))
      {
        params->onset = params->target;
        params->speed = 0.0;

        fts_timebase_add_call(fts_get_timebase(), params->o, sogs_bang_at_end, 0, 0.0);
      }
    }
  }
}

/************************************************
*
*  user methods
*
*/

static void
sogs_set_position(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double position = fts_get_number_float(at);

  params->onset = params->target = clip_to_vector(position * params->sr, params->fvec);
  params->speed = 0.0;
}

static void
sogs_set_position_variation(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double f = fts_get_number_float(at);

  params->onset_var = f * params->sr;
}

static void
sogs_set_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double f = fts_get_number_float(at);

  params->pitch = f;
}

static void
sogs_set_pitch_variation(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double f = fts_get_number_float(at);

  params->pitch_var = f;
}

static void
sogs_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double duration = fts_get_number_float(at) * params->sr;

  if(params->sr != 1.0 && duration < 4.0)
    duration = 4.0;

  params->duration = duration;
  params->wind_incr = (double)FTS_INTPHASE_RANGE / duration;
}

static void
sogs_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  fvec_t *fvec = (fvec_t *)fts_get_object(at);
  int size = fvec_get_size(fvec);

  if(params->onset > size)
    params->onset = size;

  if(params->target == fvec_get_size(params->fvec) || params->target > size)
    params->target = size;

  fts_object_release((fts_object_t *)params->fvec);
  params->fvec = fvec;
  fts_object_refer((fts_object_t *)fvec);
}

static void
sogs_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
  {
    default:
    case 5:
      if(fts_is_number(at + 4))
        sogs_set_duration(o, 0, 0, 1, at + 4);
    case 4:
      if(fts_is_number(at + 3))
        sogs_set_pitch_variation(o, 0, 0, 1, at + 3);
    case 3:
      if(fts_is_number(at + 2))
        sogs_set_pitch(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
        sogs_set_position_variation(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
        sogs_set_position(o, 0, 0, 1, at);
    case 0:
      break;
  }
}

static void
sogs_set_overlap(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;

  if(ac && fts_is_number(at))
  {
    sogs_params_t *params = ftl_data_get_ptr(this->params);
    sogs_segment_t *segs = ftl_data_get_ptr(this->segs);
    int n_overlap = 1 << fts_get_number_int(at);
    int i;

    if(n_overlap < 1)
      n_overlap = 1;
    else if(n_overlap > this->max_overlap)
      n_overlap = this->max_overlap;

    for(i=params->n_overlap; i<n_overlap; i++)
      sogs_segment_set(segs + i, params, 0);

    params->n_overlap = n_overlap;

    if(n_overlap > 2)
      params->scale = 2. / n_overlap;
    else
      params->scale = 1;
  }
}

static void
sogs_set_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);

  if(ac > 0 && fts_is_number(at))
  {
    if(ac == 1)
      sogs_set_position(o, 0, 0, 1, at);
    else if(ac == 2 && fts_is_number(at + 1))
    {
      double target = fts_get_number_float(at) * params->sr;
      double period = fts_get_number_float(at + 1) * params->sr;

      params->target = clip_to_vector(target, params->fvec);
      params->speed = (target - params->onset) / period;
    }
  }
}

static void
sogs_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double end = fvec_get_size(params->fvec);

  if(ac && fts_is_number(at))
  {
    double period = 0.001 * fts_get_number_float(at);

    params->target = end;
    params->speed = (end - params->onset) / period;
  }
  else
  {
    params->onset = params->target = end;
    params->speed = 0.0;
  }
}

static void
sogs_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);
  double end = fvec_get_size(params->fvec);
  double start = 0.0;
  double target = end;
  double speed = 1.0;
  double sr = params->sr;
  int n = (ac > 3)? 2: ac - 1;

  if(n >= 0)
  {
    /* set speed */
    if(fts_is_number(at + n))
      speed = fts_get_number_float(at + n);
    else
      return;

    n--;
  }

  if(n >= 0)
  {
    /* set target */
    if(fts_is_number(at + n))
      target = fts_get_number_float(at + n) * sr;
    else if(!fts_is_symbol(at + n) || fts_get_symbol(at + n) != sym_end)
      return;

    n--;
  }
  else
  {
    /* no target given (adjust target to speed) */
    double onset = params->onset;

    if(speed > 0 && target <= onset)
      target = end;
    else if(speed < 0 && target >= onset)
      target = 0;
    else if(speed == 0)
      target = onset;

    params->target = target;
    params->speed = speed;

    return;
  }

  if(n >= 0)
  {
    /* set start */
    if(fts_is_number(at + n))
      start = fts_get_number_float(at + n) * sr;
    else if(fts_is_symbol(at + n) && (fts_get_symbol(at + n) == sym_end))
      start = end;
    else
      return;
  }
  else
  {
    /* no start point given (start at current onset) */
    start = params->onset;
  }

  /* set onset and target */
  params->onset = start;
  params->target = target;

  /* adjust speed to start and target */
  if(target > start)
  {
    /* go forward */
    if(speed > 0.0)
      params->speed = speed;
    else if(speed < 0.0)
      params->speed = -speed;
    else
    {
      /* stop */
      params->target = params->onset;
      params->speed = 0.0;
    }
  }
  else if(target < start)
  {
    /* go backward */
    if(speed < 0.0)
      params->speed = speed;
    else if(speed > 0.0)
      params->speed = -speed;
    else
    {
      /* stop */
      params->target = params->onset;
      params->speed = 0.0;
    }
  }
  else
  {
    /* stop */
    params->onset = params->target = start;
    params->speed = 0.0;
  }
}

static void
sogs_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;
  sogs_params_t *params = ftl_data_get_ptr(this->params);

  params->target = params->onset;
  params->speed = 0.0;
}

/************************************************
*
*    class
*
*/

static void
sogs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;

  this->params = NULL;
  this->segs = NULL;

  if(ac > 0 && fts_is_a(at, fvec_type))
  {
    sogs_segment_t *segs;
    sogs_params_t *params;
    fvec_t *fvec;
    int i;

    /* get fvec argument */
    fvec =(fvec_t *) fts_get_object(at);
    fts_object_refer((fts_object_t *)fvec);

    /* get optional overlap argument */
    if(ac > 1 && fts_is_number(at + 1))
    {
      this->max_overlap = 1 << fts_get_number_int(at + 1);

      if(this->max_overlap < 2)
        this->max_overlap = 2;
    }
    else
      this->max_overlap = 8;

    /* allocate segments */
    this->segs = ftl_data_alloc(sizeof(sogs_segment_t) * this->max_overlap);
    segs = ftl_data_get_ptr(this->segs);

    /* init segments */
    for(i=0; i<this->max_overlap; i++)
      sogs_segment_init(segs + i, i);

    /* allocate parameters */
    this->params = ftl_data_alloc(sizeof(sogs_params_t));
    params = ftl_data_get_ptr(this->params);

    /* init parameters */
    params->onset = 0.0;
    params->onset_var = 5.0;
    params->pitch = 0.0;
    params->pitch_var = 0.0;
    params->duration = 60;

    params->target = 0.0;
    params->speed = 0.0;

    params->n_overlap = this->max_overlap;
    params->scale = 2. / this->max_overlap;

    params->fvec = fvec;

    params->wind_phi = 0;
    params->wind_incr = 0;

    params->sr = 1.0;
    params->o = o;

    /* set initial parameters */
    if(ac > 2)
      sogs_set(o, 0, 0, ac - 2, at + 2);
  }
  else
    fts_object_error(o, "first argument of fvec required");

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sogs_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sogs_t *this = (sogs_t *)o;

  if(this->params != NULL)
  {
    sogs_params_t *params = ftl_data_get_ptr(this->params);

    fts_object_release((fts_object_t *)params->fvec);
    ftl_data_free(this->params);
    ftl_data_free(this->segs);
  }

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sogs_instantiate(fts_class_t *cl)
{
  if(!sogs_window)
    sogs_window = fts_fftab_get_hanning(WINDOW_SIZE);

  fts_class_init(cl, sizeof(sogs_t), sogs_init, sogs_delete);

  fts_class_message_varargs(cl, fts_s_put, sogs_put);

  fts_class_message_varargs(cl, fts_new_symbol("overlap"), sogs_set_overlap);

  fts_class_message_varargs(cl, fts_new_symbol("go"), sogs_go);
  fts_class_message_varargs(cl, fts_new_symbol("stop"), sogs_stop);
  fts_class_message_varargs(cl, sym_end, sogs_end);

  fts_class_message_varargs(cl, fts_s_set, sogs_set);

  fts_class_inlet(cl, 0, fvec_type, sogs_set_fvec);
  fts_class_inlet_varargs(cl, 0, sogs_set_target);
  fts_class_inlet_number(cl, 0, sogs_set_position);
  fts_class_inlet_number(cl, 1, sogs_set_position_variation);
  fts_class_inlet_number(cl, 2, sogs_set_pitch);
  fts_class_inlet_number(cl, 3, sogs_set_pitch_variation);
  fts_class_inlet_number(cl, 4, sogs_set_duration);

  fts_dsp_declare_function(sym_sogs, ftl_sogs);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  fts_class_outlet_void(cl, 1);
}

void
signal_sogs_config(void)
{
  sym_sogs = fts_new_symbol("sogs~");
  sym_begin = fts_new_symbol("begin");
  sym_end = fts_new_symbol("end");

  fts_class_install(sym_sogs, sogs_instantiate);
}
