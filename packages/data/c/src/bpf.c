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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include "fts.h"

/************************************************************
 *
 *  break points
 *
 */

static fts_heap_t *bp_heap;

typedef struct _bp_
{
  double time;
  double value;
  struct _bp_ *next; /* list of break points */
} bp_t;

#define bp_get_time(b) ((b)->time)
#define bp_get_value(b) ((b)->value)
#define bp_get_next(b) ((b)->next)
#define bpf_set_editor_open(s) ((s)->open = 1)
#define bpf_set_editor_close(s) ((s)->open = 0)

static bp_t *
bp_new(double time, double value)
{
  bp_t *bp = fts_heap_alloc(bp_heap);
  
  bp->time = time;
  bp->value = value;
  bp->next = 0;

  return bp;
}

static void
bp_delete(bp_t *bp)
{
  fts_heap_free(bp, bp_heap);
}

/************************************************************
 *
 *  bpf
 *
 */

typedef struct _bpf_
{
  fts_object_t o;
  fts_ramp_t ramp;
  bp_t *first; /* first break point of function */
  bp_t *last; /* last break point of function */
  bp_t *next; /* next break point to be triggered */
  int open; /* flag: is 1 if bpf editor is open */
  double duration;
  double sr;
} bpf_t;

static fts_symbol_t sym_bpf = 0;

#define bpf_advance(b) ((b)->next = (b)->next->next)

static void
bpf_clear(bpf_t *bpf)
{
  bp_t *bp = bpf->first;

  while(bp)
    {
      bp_t *next = bp_get_next(bp);

      bp_delete(bp);
      bp = next;
    }
  
  bpf->first = 0;
  bpf->last = 0;
  bpf->next = 0;  

  bpf->duration = 0.0;
}

static void
bpf_append(bpf_t *bpf, bp_t *bp)
{
  if(bpf->first == 0)
    bpf->first = bpf->last = bp;
  else
    {
      bpf->last->next = bp;
      bpf->last = bp;
    }

  bpf->duration += bp->time;
}

/************************************************************
 *
 *  methods
 *
 */

static void
bpf_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  if(this->first)
    {
      fts_ramp_set_target(&this->ramp, bp_get_value(this->first), bp_get_time(this->first), this->sr);

      this->next = this->first->next;
    }
}

static void
bpf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  fts_ramp_freeze(&this->ramp);

  this->next = 0;
}

static void
bpf_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_clear(this);
  
  if(ac)
    {
      int i = ac & 1;
      
      if(i)
	{
	  if(fts_is_number(at))
	    {
	      double value = fts_get_number_float(at);

	      bpf_append(this, bp_new(0.0, value));
	    }
	}

      for(; i<ac; i+=2)
	{
	  if(fts_is_number(at + i) && fts_is_number(at + i + 1))
	    {
	      double time = fts_get_number_float(at + i);
	      double value = fts_get_number_float(at + i + 1);
	      
	      bpf_append(this, bp_new(time, value));
	    }
	}    
    }
}

static void
bpf_set_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  bpf_set(o, 0, 0, ac, at);
  bpf_go(o, 0, 0, ac, at);
}

/* create new event and upload by client request */
void
bpf_add_point_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  if(ac)
    {
      if(fts_is_number(at) && fts_is_number(at + 1) && fts_is_number(at + 2))
	{
	  int index = fts_get_number_int(at);
	  double time = fts_get_number_float(at + 1);
	  double value = fts_get_number_float(at + 2);
	      
	  bpf_append(this, bp_new(time, value));

	  fts_client_send_message((fts_object_t *)this, fts_new_symbol("addPoint"), ac, at);
	}    
    }
}

void
bpf_remove_points_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  /*  remove event objects from client */
  fts_client_send_message(o, fts_new_symbol("removePoints"), ac, at);
}

void
bpf_set_point_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  fts_client_send_message(o, fts_new_symbol("setPoint"), ac, at);
}

/************************************************************
 *
 *  dsp
 *
 */

static void
bpf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  this->sr = sr;

  bpf_stop(o, 0, 0, 0, 0);

  fts_set_ptr(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  dsp_add_funcall(sym_bpf, 3, a);
}

static void
bpf_ftl(fts_word_t *argv)
{
  bpf_t *this = (bpf_t *) fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  fts_ramp_t *ramp = &this->ramp;
  int n_tick = fts_word_get_int(argv + 2);
  int i = 0;

  while(i < n_tick)
    {
      if(fts_ramp_running(ramp))
	{
	  double value = fts_ramp_get_value(ramp);
	  double incr = fts_ramp_get_incr(ramp);
	  int n_target = i + fts_ramp_get_steps(ramp);
	  int n_left = n_tick - i;

	  if(n_target > n_tick)
	    n_target = n_tick;
	  
	  for(; i<n_target; i++)
	    {
	      out[i] = value;
	      value += incr;
	    }
	  
	  fts_ramp_incr_by(ramp, n_left);
	}
      else if(this->next)
	{
	  fts_ramp_set_target(ramp, bp_get_value(this->next), bp_get_time(this->next), this->sr);

	  bpf_advance(this);
	}
      else
	{
	  double value = fts_ramp_get_value(ramp);

	  for(; i<n_tick; i++)
	    out[i] = value;
	}
    }
}


static void
bpf_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_set_editor_open(this);
  fts_client_send_message(o, /*seqsym_createEditor*/fts_new_symbol("openEditor"), 0, 0);
  /*sequence_upload(o, 0, 0, 0, 0);*/

}

static void
bpf_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_set_editor_close(this);
  /* here we could as well un-upload the objects (and the client would have to destroy the proxies) */
}

/************************************************************
 *
 *  class
 *
 */

static void
bpf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  dsp_list_insert(o);

  fts_ramp_init(&this->ramp, 0.0);

  /* init break points */
  this->first = 0;
  this->last = 0;
  this->next = 0;
  this->duration = 0.0;

  this->sr = 1.0;

  bpf_set(o, 0, 0, ac - 1, at + 1);
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  dsp_list_remove(o);

  bpf_clear(this);
}

static fts_status_t
bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  for(i=1; i<ac; i++)
    if(!fts_is_number(at + i))
      return &fts_CannotInstantiate;

  fts_class_init(cl, sizeof(bpf_t), 2, 1, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bpf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bpf_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, bpf_put);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, bpf_go);

  fts_method_define_varargs(cl, 0, fts_s_int, bpf_set_and_go);
  fts_method_define_varargs(cl, 0, fts_s_float, bpf_set_and_go);
  fts_method_define_varargs(cl, 0, fts_s_list, bpf_set_and_go);
  
  fts_method_define_varargs(cl, 1, fts_s_int, bpf_set);
  fts_method_define_varargs(cl, 1, fts_s_float, bpf_set);
  fts_method_define_varargs(cl, 1, fts_s_list, bpf_set);

  /* graphical editor */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), bpf_open_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), bpf_close_editor);
  fts_method_define_varargs(cl, 0, fts_new_symbol("open"), bpf_open_editor);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_point"), bpf_add_point_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_points"), bpf_remove_points_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_point"), bpf_set_point_by_client_request);

  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
signal_bpf_config(void)
{
  bp_heap = fts_heap_new(sizeof(bp_t));

  fts_metaclass_install(fts_new_symbol("bpf~"), bpf_instantiate, fts_arg_type_equiv);

  sym_bpf = fts_new_symbol("bpf");
  dsp_declare_function(sym_bpf, bpf_ftl);
}
