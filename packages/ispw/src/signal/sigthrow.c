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

#include "fts.h"
#include <string.h>

/********************************
 *
 *   gensampname utility
 *
 */

static void
gensampname(char *buf, const char *base, long int n)
{
  char b2[30];
  char *s = b2+29;
  *s = 0;

  if (n < 0)
    n = 0;

  while ((*buf = *base++))
    buf++;

  if (!n)
    *(--s) = '0';
  else while (n)
    {
      *(--s) = '0' + (n%10);
      n /= 10;
    }
  while ((*buf++ = *s++))
    ;
}


/********************************
 *
 *   catch~
 *
 */

static fts_hash_table_t catch_table;

#define MAXVS 64

static fts_symbol_t sigcatch_function = 0;
static fts_symbol_t sigcatch_64_function = 0;

typedef struct sigcatch_t sigcatch_t;

struct sigcatch_t
{
  fts_object_t _o;
  fts_symbol_t sym;

  float samps[MAXVS];
};


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
  int n = fts_word_get_long(argv+2);
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

  fts_vecx_fzero(this->samps, MAXVS);

  if (fts_dsp_get_output_size(dsp, 0) == 64)
    {
      fts_set_symbol(argv,   fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr   (argv+1, this->samps);

      dsp_add_funcall(sigcatch_64_function, 2, argv);
    }
  else
    {
      fts_set_symbol(argv,   fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr   (argv+1, this->samps);
      fts_set_long  (argv+2, fts_dsp_get_output_size(dsp, 0));

      dsp_add_funcall(sigcatch_function, 3, argv);
    }
}


static void
sigcatch_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_atom_t a;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 1, 0);

  if ((s == 0) || (*(fts_symbol_name(s)) == '0'))
    s = fts_new_symbol("sigcatch~");

  this->sym = s;

  if (fts_hash_table_lookup(&catch_table, this->sym, &a))
    post("catch~: duplicated name: %s\n", fts_symbol_name(this->sym));
  else
    {
      fts_set_ptr(&a, this);
      fts_hash_table_insert(&catch_table, this->sym, &a);
    }

  dsp_list_insert(o);
}

static void
sigcatch_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_atom_t a;
  fts_symbol_t s = this->sym;

  if (fts_hash_table_lookup(&catch_table, s, &a))
    fts_hash_table_remove(&catch_table, s);

  dsp_list_remove(o);
}

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

  fts_class_install(fts_new_symbol("catch~"),sigcatch_instantiate);
}


/********************************
 *
 *   throw~
 *
 */

static fts_symbol_t sigthrow_function = 0;
static fts_symbol_t sigthrow_64_function = 0;


typedef struct
{
  fts_object_t _o;
  fts_symbol_t sym;
  ftl_data_t ftl_data;
  int rec_prot;			/* housekeeping for dead code elimination */
} sigthrow_t;


static void
sigthrow_dsp_fun(fts_word_t *argv)
{
  float * restrict in  = (float *)fts_word_get_ptr(argv);
  float * restrict p = *((float **)fts_word_get_ptr(argv+1));
  int n = fts_word_get_long(argv+2);

  if (p)
    {
      int i;

      for (i = 0; i < n; i++)
	p[i] = p[i] + in[i];
    }
}


static void
sigthrow_dsp_64_fun(fts_word_t *argv)
{
  float * restrict in  = (float *)fts_word_get_ptr(argv);
  float * restrict p = *((float **)fts_word_get_ptr(argv+1));

  if (p)
    {
      int i;

      for (i = 0; i < 64; i++)
	p[i] = p[i] + in[i];
    }
}


static void
sigthrow_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t a;

  /* look here for the corresponing catch buffer, to eliminate
     instantiation order dependency between catch~ and throw~.
   */

  if (fts_hash_table_lookup(&catch_table, this->sym, &a))
    {
      sigcatch_t *sigc = (sigcatch_t *) fts_get_ptr(&a);
      float *samps = sigc->samps;

      ftl_data_copy(float *, this->ftl_data, &samps);
    }
  else
    {
      const float *zerop = 0;

      ftl_data_copy(float *, this->ftl_data, &zerop);
    }

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data   (argv+1, this->ftl_data);

  if (fts_dsp_get_input_size(dsp, 0) == 64)
    dsp_add_funcall(sigthrow_64_function, 2, argv);
  else
    {
      fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(sigthrow_function, 3, argv);
    }
}

static void
sigthrow_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);
  sigthrow_t *this = (sigthrow_t *)o;
  fts_atom_t a;

  this->sym = s;

  if (fts_hash_table_lookup(&catch_table, s, &a))
    {
      sigcatch_t *c = (sigcatch_t *) fts_get_ptr(&a);
      float *samps = c->samps;

      ftl_data_copy(float *, this->ftl_data, &samps);
    }
  else
    {
      const float *zerop = 0;

      ftl_data_copy(float *, this->ftl_data, &zerop);
    }
}

static void
sigthrow_print(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);

  if (!s)
    s = fts_new_symbol("throw~");

  post("%s: %s\n", fts_symbol_name(s), fts_symbol_name(this->sym));
}

static void
sigthrow_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  char buf[30];
  fts_atom_t a;

  gensampname(buf, "catch", fts_get_long(at));
  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  sigthrow_set(o, winlet, s, 1, &a);
}

static void
sigthrow_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 1, 0);

  this->sym = s;
  this->rec_prot = 0;
  this->ftl_data = ftl_data_new(float *);

  dsp_list_insert(o);
}

static void
sigthrow_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;

  ftl_data_free(this->ftl_data);

  dsp_list_remove(o);
}

/* Daemon to know if throw is a dsp sink; it look for the throw, and
   ask him */

#ifdef _NOT_YET_IMPLEMENTED
static fts_atom_t *
throw_is_dsp_sink_get_daemon(fts_object_t *obj, fts_symbol_t property)
{
  sigthrow_t *this = (sigthrow_t *)obj;

  if (this->rec_prot)
    return 0;			/* recursive search protection */
  else
    {
      sigcatch_t *sigc;

      if (fts_hash_table_lookup(&catch_table, this->sym, (void **)&sigc))
	{
	  fts_atom_t *a;

	  this->rec_prot = 1;
	  a =  fts_object_get_prop((fts_object_t *) sigc, property);
	  this->rec_prot = 0;

	  return a;
	}
      else
	return 0;
    }
}
#endif

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
  fts_method_define_optargs(cl, 0, fts_s_print, sigthrow_print, 1, a, 0);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, sigthrow_set, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigthrow_int, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  /* Class DSP properties; to know if a throw is a dsp sink, we look for the
     catch, and ask him: do not work with dinamic dispatching of the throw;
     we can use it only if we know that the throw do not switch targets
     dynamically ... we can know it by testing the existance of a control input.
     */

  /* fts_class_add_get_daemon(cl, fts_s_dsp_is_sink, throw_is_dsp_sink_get_daemon); */

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





