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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include "fts.h"
#include <string.h>

/*****************************************************************************
 *
 *  catch~ object
 *
 */

static fts_hash_table_t catch_table;

static fts_symbol_t sigcatch_function = 0;
static fts_symbol_t sigcatch_64_function = 0;

typedef struct _sigcatch_
{
  fts_object_t _o;
  fts_symbol_t name;
  float *buf;
  int n_tick;
} sigcatch_t;

static void
sigcatch_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_atom_t a;

  if(name && fts_symbol_name(name) != "" && fts_hash_table_lookup(&catch_table, name, &a))
    {
      post("catch~: duplicated name: %s (last ignored)\n", fts_symbol_name(name));
      this->name = 0;
    }
  else
    {
      fts_set_ptr(&a, this);
      fts_hash_table_insert(&catch_table, name, &a);

      this->name = name;
      dsp_list_insert(o);
    }

  this->buf = 0;
  this->n_tick = 0;
}

static void
sigcatch_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_atom_t a;

  if(this->buf)
    fts_free(this->buf);

  if(this->name && fts_hash_table_lookup(&catch_table, this->name, &a))
    {
      fts_hash_table_remove(&catch_table, this->name); 
      dsp_list_remove(o);
    }
}

/*****************************************************************************
 *
 *  catch~ dsp
 *
 */

static void
sigcatch_64_dsp_fun(fts_word_t *argv)
{
  float * restrict out = (float *)fts_word_get_ptr(argv);
  float * restrict buf = (float *)fts_word_get_ptr(argv+1);
  int i;

  for (i = 0; i < 64; i++)
    {
      out[i] = buf[i];
      buf[i] = 0;
    }
}


static void
sigcatch_dsp_fun(fts_word_t *argv)
{
  float * restrict out = (float *)fts_word_get_ptr(argv);
  float * restrict buf = (float *)fts_word_get_ptr(argv+1);
  int n = fts_word_get_int(argv+2);
  int i;

  for (i = 0; i < n; i++)
    {
      out[i] = buf[i];
      buf[i] = 0;
    }
}

static void
sigcatch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
 
  if(this->n_tick == 0)
    {
      this->buf = (float *)fts_malloc(sizeof(float) * n_tick);
      this->n_tick = n_tick;
    }
  else if(this->n_tick != n_tick)
    {
      post("catch~ %s: tick size doesn't match\n", fts_symbol_name(this->name));
      return;
    }
  
  fts_vecx_fzero(this->buf, n_tick);

  if (n_tick == 64)
    {
      fts_set_symbol(argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr(argv+1, this->buf);

      dsp_add_funcall(sigcatch_64_function, 2, argv);
    }
  else
    {
      fts_set_symbol(argv, fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr(argv+1, this->buf);
      fts_set_int(argv+2, n_tick);

      dsp_add_funcall(sigcatch_function, 3, argv);
    }
}

/*****************************************************************************
 *
 *  catch~ class
 *
 */

static fts_status_t
sigcatch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigcatch_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigcatch_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigcatch_delete, 0, a);

  a[0] = fts_s_ptr; 
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigcatch_put, 1, a);
 
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
 
  sigcatch_function = fts_new_symbol("sigcatch");
  dsp_declare_function(sigcatch_function, sigcatch_dsp_fun);

  sigcatch_64_function = fts_new_symbol("sigcatch_64");
  dsp_declare_function(sigcatch_64_function, sigcatch_64_dsp_fun);
 
  return fts_Success;
}

void
sigcatch_config(void)
{
  fts_hash_table_init(&catch_table);

  fts_class_install(fts_new_symbol("catch~"), sigcatch_instantiate);
}


/*****************************************************************************
 *
 *  throw~ object
 *
 */

static fts_symbol_t sigthrow_function = 0;
static fts_symbol_t sigthrow_64_function = 0;

typedef struct _sigthrow_
{
  fts_object_t _o;
  fts_symbol_t name;
  ftl_data_t bufp;
  int n_tick;
} sigthrow_t;

static void
sigthrow_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);

  this->name = name;
  this->bufp = ftl_data_new(float *);
  this->n_tick = 0;

  dsp_list_insert(o);
}

static void
sigthrow_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;

  ftl_data_free(this->bufp);

  dsp_list_remove(o);
}

/*****************************************************************************
 *
 *  throw~ dsp
 *
 */

static void
sigthrow_dsp_fun(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv);
  float * restrict p = *((float **)fts_word_get_ptr(argv + 1));
  int n = fts_word_get_int(argv + 2);

  if(p)
    {
      int i;

      for (i = 0; i < n; i++)
	p[i] += in[i];
    }
}

static void
sigthrow_dsp_64_fun(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv);
  float * restrict p = *((float **)fts_word_get_ptr(argv + 1));

  if(p)
    {
      int i;

      for (i = 0; i < 64; i++)
	p[i] += in[i];
    }
}

static void
sigthrow_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  float **bufp = ftl_data_get_ptr(this->bufp);
  fts_atom_t a;

  this->n_tick = n_tick;

  /* look here for the corresponing catch buffer, to eliminate instantiation order dependency between catch~ and throw~. */
  if (fts_hash_table_lookup(&catch_table, this->name, &a))
    {
      sigcatch_t *sigcatch = (sigcatch_t *)fts_get_ptr(&a);

      if(sigcatch->n_tick == 0)
	{
	  sigcatch->buf = (float *)fts_malloc(sizeof(float) * n_tick);
	  sigcatch->n_tick = n_tick;
	  
	  *bufp = sigcatch->buf;
	}
      else if(sigcatch->n_tick != n_tick)
	{
	  post("throw~ %s: tick size doesn't match\n", fts_symbol_name(this->name));
	  *bufp = 0;
	}
      else
	*bufp = sigcatch->buf;
    }
  else
    *bufp = 0;
  
  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data (argv+1, this->bufp);
  
  if (fts_dsp_get_input_size(dsp, 0) == 64)
    dsp_add_funcall(sigthrow_64_function, 2, argv);
  else
    {
      fts_set_int (argv+2, n_tick);
      dsp_add_funcall(sigthrow_function, 3, argv);
    }
}

/*****************************************************************************
 *
 *  throw~ user methods
 *
 */

static void
sigthrow_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  float **bufp = ftl_data_get_ptr(this->bufp);
  fts_atom_t a;

  this->name = fts_get_symbol_arg(ac, at, 0, 0);

  if(this->name && fts_hash_table_lookup(&catch_table, this->name, &a))
    {
      sigcatch_t *sigcatch = (sigcatch_t *)fts_get_ptr(&a);

      if(sigcatch->n_tick != 0 && sigcatch->n_tick != this->n_tick)
	{
	  post("throw~ %s: tick size doesn't match\n", fts_symbol_name(this->name));
	  *bufp = 0;
	}
      else
	*bufp = sigcatch->buf;
    }
  else
    *bufp = 0;
}

static void
sigthrow_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int n = fts_get_int(at);
  char str[] = "catch999999";
  fts_atom_t a[1];

  sprintf(str + 5, "%d", n);
  fts_set_symbol(a, fts_new_symbol_copy(str));

  sigthrow_set(o, winlet, s, 1, a);
}

/*****************************************************************************
 *
 *  throw~ class
 *
 */

static fts_status_t
sigthrow_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigthrow_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigthrow_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigthrow_delete, 0, a);

  a[0] = fts_s_ptr; 
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigthrow_put, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, sigthrow_set, 1, a);
 
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigthrow_int, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  sigthrow_function = fts_new_symbol("sigthrow");
  dsp_declare_function(sigthrow_function, sigthrow_dsp_fun);

  sigthrow_64_function = fts_new_symbol("sigthrow_64");
  dsp_declare_function(sigthrow_64_function, sigthrow_dsp_64_fun);

  return fts_Success;
}

void
sigthrow_config(void)
{
  fts_class_install(fts_new_symbol("throw~"),sigthrow_instantiate);
}
