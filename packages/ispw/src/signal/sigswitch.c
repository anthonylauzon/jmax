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
   (Francois Dechelle, dechelle@ircam.fr)
   Hack implementation of the switch :
     * uses the FTL subroutines, which is not hack
     * but the generation of the subroutines relies on the way the dsp
     graph is walked through : it must be ordered using a depth first
     searching. There is for that a hack in dspgraph.c !!!
*/

#include "fts.h"

extern ftl_program_t *dsp_chain_on;

static fts_symbol_t switch_function = 0;

static int switch_count = 1;

typedef struct
{
  fts_object_t _o;
  ftl_subroutine_t *current, *previous;
  ftl_data_t switch_ftl_data;
} sigswitch_t;


static void
call_ftl_subr_cond( fts_word_t *argv)
{
  int *x = (int *)fts_word_get_ptr(argv);
  ftl_subroutine_t *subr = (ftl_subroutine_t *)fts_word_get_ptr(argv+1);

  if (*x)
    ftl_program_call_subr( dsp_chain_on, subr);
}

static void
sigswitch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  fts_atom_t argv[3];
  char tmp[64];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  sprintf( tmp, "switch_%d", switch_count);
  switch_count++;

  this->current = ftl_program_add_subroutine( dsp_chain_on, fts_new_symbol_copy(tmp));

  /* Add the call FTL subroutine conditionnally function */
  fts_set_ftl_data( argv, this->switch_ftl_data);
  fts_set_ptr( argv+1, this->current);
  dsp_add_funcall( switch_function, 2, argv);

  this->previous = ftl_program_set_current_subroutine( dsp_chain_on, this->current);

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
    }
}

static void
sigswitch_put_after(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;

  ftl_program_add_return( dsp_chain_on);
  ftl_program_set_current_subroutine( dsp_chain_on, this->previous);
}

static void
sigswitch_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  int state = fts_get_long(at);

  ftl_data_copy( int, this->switch_ftl_data, &state);
}

static void
sigswitch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  int state = 0;

  this->switch_ftl_data = ftl_data_new( int);
  ftl_data_copy( int, this->switch_ftl_data, &state);

  dsp_list_insert(o);
}

static void
sigswitch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;

  ftl_data_free( this->switch_ftl_data);
  dsp_list_remove(o);
}

static fts_status_t
sigswitch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigswitch_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigswitch_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigswitch_delete, 0, 0);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigswitch_put, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("put_after_successors"), sigswitch_put_after, 0, 0);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigswitch_int, 1, a);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  switch_function = fts_new_symbol( "switch");
  dsp_declare_function( switch_function, call_ftl_subr_cond);

  return fts_Success;
}

void
sigswitch_config(void)
{
  fts_class_install(fts_new_symbol("switch~"),sigswitch_instantiate);
}
