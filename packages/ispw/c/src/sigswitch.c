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
   (Francois Dechelle, dechelle@ircam.fr)
   Hack implementation of the switch :
     * uses the FTL subroutines, which is not hack
     * but the generation of the subroutines relies on the way the dsp
     graph is walked through : it must be ordered using a depth first
     searching. There is for that a hack in dspgraph.c !!!
*/

#include <fts/fts.h>

static fts_symbol_t switch_function = 0;

static int switch_count = 1;

typedef struct {
  fts_object_t _o;
  ftl_subroutine_t *current, *previous;
  ftl_data_t switch_ftl_data;
} sigswitch_t;


static void
call_ftl_subr_cond( fts_word_t *argv)
{
  int *x = (int *)fts_word_get_pointer(argv);
  ftl_program_t *dsp_chain = (ftl_program_t *)fts_word_get_pointer(argv+1);
  ftl_subroutine_t *subr = (ftl_subroutine_t *)fts_word_get_pointer(argv+2);

  if (*x)
    ftl_program_call_subr( dsp_chain, subr);
}

static void
sigswitch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  fts_atom_t argv[3];
  char tmp[64];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  sprintf( tmp, "switch_%d", switch_count);
  switch_count++;

  this->current = ftl_program_add_subroutine( dsp_get_current_dsp_chain(), fts_new_symbol_copy(tmp));

  /* Add the call FTL subroutine conditionnally function */
  fts_set_ftl_data( argv, this->switch_ftl_data);
  fts_set_pointer( argv+1, dsp_get_current_dsp_chain());
  fts_set_pointer( argv+2, this->current);
  fts_dsp_add_function( switch_function, 3, argv);

  this->previous = ftl_program_set_current_subroutine( dsp_get_current_dsp_chain(), this->current);

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 0), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_input_size(dsp, 0));
}

static void
sigswitch_put_after(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;

  ftl_program_add_return( dsp_get_current_dsp_chain());
  ftl_program_set_current_subroutine( dsp_get_current_dsp_chain(), this->previous);
}

static void
sigswitch_switch(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  int state = fts_get_int(at);

  ftl_data_copy( int, this->switch_ftl_data, &state);
}

static void
sigswitch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;
  int state = fts_get_int_arg(ac, at, 0, 0);

  this->switch_ftl_data = ftl_data_new( int);
  ftl_data_copy( int, this->switch_ftl_data, &state);

  fts_dsp_add_object(o);
}

static void
sigswitch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigswitch_t *this = (sigswitch_t *)o;

  ftl_data_free( this->switch_ftl_data);
  fts_dsp_remove_object(o);
}

static fts_status_t
sigswitch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigswitch_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, sigswitch_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, sigswitch_delete);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, sigswitch_put);

  fts_method_define_varargs(cl, fts_system_inlet, fts_new_symbol("put_after_successors"), sigswitch_put_after);
  
  fts_method_define_varargs(cl, 0, fts_s_int, sigswitch_switch);
  fts_method_define_varargs(cl, 1, fts_s_int, sigswitch_switch);
  
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  switch_function = fts_new_symbol("switch");
  fts_dsp_declare_function( switch_function, call_ftl_subr_cond);

  return fts_ok;
}

void
sigswitch_config(void)
{
  fts_class_install(fts_new_symbol("switch~"),sigswitch_instantiate);
}
