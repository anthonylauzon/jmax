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

#include <fts/fts.h>

#define CLASS_name "voxalloc"
#define MAX_size_rec_name 256

#define DEFn_vox  16L
#define DEFn_args 6L

#define HALF_TICK 

typedef struct 
{
  fts_object_t   t_ob;
  fts_symbol_t *sym_receive;
  float *time_over;
  int n_vox;
  int idx;
  fts_atom_t *list_store;
  int n_args;
  int i_dur;
  float dur;
  float last_dur;
  float half_tick;
} voxalloc_t;


/************************************************
 *
 *   object
 *
 */

static void
voxalloc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_t *this = (voxalloc_t *) o;

  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  int n_vox = fts_get_int_arg(ac, at, 2, 16);
  int n_args = fts_get_int_arg(ac, at, 3, 6);
  int i_dur = fts_get_int_arg(ac, at, 4, 0);
  int dur = fts_get_int_arg(ac, at, 5, 0);
  char rec_name[MAX_size_rec_name + 1];
  int i;

  this->sym_receive = (fts_symbol_t *) fts_malloc(sizeof(fts_symbol_t ) * n_vox);
  this->n_vox = n_vox;
  this->idx = 0;
  this->time_over = (float *) fts_malloc(sizeof(float) * n_vox);
	
  this->n_args = n_args;
  if(i_dur < 0) i_dur = -i_dur;
  if(i_dur > n_args) i_dur = n_args;
  this->i_dur = i_dur - 1;
  this->dur = dur;
  this->last_dur = 0.;
  this->list_store  = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * n_args);
	
  for(i = 0; i < n_vox; i++){
    sprintf(rec_name,"%d-%s", i, fts_symbol_name(name));
    rec_name[MAX_size_rec_name] = '\0';
    this->sym_receive[i] = fts_new_symbol_copy(rec_name);
  }
	
  for(i = 0; i < n_vox; i++)
    this->time_over[i] = (float)0;
	
  for(i = 0; i < n_args; i++)
    fts_set_int(&this->list_store[i], 0);

  this->half_tick = 500.0 * fts_dsp_get_tick_size() / fts_dsp_get_sample_rate();
}

static void
voxalloc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_t *this = (voxalloc_t *) o;

  fts_free(this->sym_receive);
  fts_free(this->time_over);
  fts_free(this->list_store);
}


/************************************************
 *
 *   user methods
 *
 */

static void
voxalloc_used(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_t *this = (voxalloc_t *) o;

  int n_used   = 0;
  float *time_over  = this->time_over;
  int idx = this->idx;
  int here = idx;
  float now = fts_get_time() - 2.0 * this->half_tick;

  do {
    if(now < time_over[idx]) 
      n_used++;

    idx++;

    if(idx == this->n_vox) 
      idx = 0;
  } while (idx != here);
  
  fts_outlet_int(o, 1, n_used);	
}

static void
voxalloc_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_t *this = (voxalloc_t *) o;

  this->dur = (float) fts_get_float_arg(ac, at, 0, 0.0f);
}

static void
voxalloc_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_t *this = (voxalloc_t *) o;
  fts_symbol_t *sym_receive = this->sym_receive;
  fts_atom_t *list_store  = this->list_store;
  int n_args = this->n_args;
  int i_dur = this->i_dur;
  float *time_over  = this->time_over;
  int idx = this->idx;
  int here  = idx;
  float now = fts_get_time();
  float dur;
	
  if (i_dur == -1)
    dur = this->dur;
  else{
    dur = fts_get_float_arg(ac, at, i_dur, this->last_dur);
    this->last_dur = dur;
  }
	
  while (time_over[idx] > now){
    idx++;
    if(idx == this->n_vox) 
      idx = 0;

    if(idx == here){
      fts_outlet_atoms(o, 0, ac, at);	
      this->idx = idx;
      return;
    }
  }
	
  time_over[idx] = now + dur - this->half_tick;

  {
    int iArg = (ac <= n_args)? ac: n_args;
    fts_label_t *label = fts_label_get(fts_object_get_patcher(o), sym_receive[idx]);

    while (iArg--) list_store[iArg] = at[iArg];
	
    if(label)
      fts_label_send(label, fts_s_list, n_args, list_store);
  }

  idx++;

  if (idx == this->n_vox) idx = 0;

  this->idx = idx;
}

static void
voxalloc_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_list(o, winlet, fts_s_list, 1, at);	
}

static void
voxalloc_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_list(o, winlet, fts_s_list, 1, at);	
}

static void
voxalloc_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  voxalloc_list(o, winlet, fts_s_list, 0, 0);	
}


/************************************************
 *
 *   class
 *
 */

static fts_status_t
voxalloc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(fts_get_int_arg(ac, at, 3, 0) == 0)
    fts_class_init(cl, sizeof(voxalloc_t), 2, 2, 0); /* no dur arg in list -> dur inlet */
  else
    fts_class_init(cl, sizeof(voxalloc_t), 1, 2, 0); /* no dur inlet */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, voxalloc_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, voxalloc_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, voxalloc_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("used"), voxalloc_used);

  fts_method_define_varargs(cl, 0, fts_s_list, voxalloc_list);
  fts_method_define_varargs(cl, 0, fts_s_int, voxalloc_int);
  fts_method_define_varargs(cl, 0, fts_s_float, voxalloc_float);

  if(fts_get_int_arg(ac, at, 3, 0) == 0)
    { 
      /* dur inlet */
      fts_method_define_varargs(cl, 1, fts_s_int, voxalloc_number_1);
      fts_method_define_varargs(cl, 1, fts_s_float, voxalloc_number_1);
    }
  
  fts_outlet_type_define_varargs(cl, 0,	fts_s_bang);
  fts_outlet_type_define_varargs(cl, 1,	fts_s_int);

  return fts_Success;
}

	
static int
voxalloc_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return(fts_get_int_arg(ac0, at0, 3, 0) == fts_get_int_arg(ac1, at1, 3, 0));
}

void
voxalloc_config(void)
{
  fts_metaclass_install(fts_new_symbol("voxalloc"), voxalloc_instantiate, voxalloc_equiv);
}




