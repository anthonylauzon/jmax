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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_DSP_H_
#define _FTS_DSP_H_

#include <fts/lang/ftl.h>

extern fts_module_t fts_dsp_module;

typedef struct 
{
  int ninputs;
  int noutputs;
  fts_dsp_signal_t **in;
  fts_dsp_signal_t **out;
} fts_dsp_descr_t;

/* Macro to access the input and output characteristics
   from a dsp descriptor
   This macros are official part of the object DSP API, and their definition
   depend on the actual dsp structure.
*/

#define fts_dsp_get_signal_name(s) ((s)->name)

/* get input properties */
#define fts_dsp_get_input_name(DESC, IN) ((DESC)->in[(IN)]->name)
#define fts_dsp_get_input_size(DESC, IN) ((DESC)->in[(IN)]->length)
#define fts_dsp_get_input_srate(DESC, IN) ((DESC)->in[(IN)]->srate)

/* test for the null input special case */
#define fts_dsp_is_input_null(DESC, IN) ((DESC)->in[(IN)]->id == 0)
#define fts_dsp_is_output_null(DESC, IN) ((DESC)->out[(IN)]->id == 0)

/* test if inlet is signal inlet */
extern int fts_dsp_is_sig_inlet(fts_object_t *o, int num);

/* get output properties */
#define fts_dsp_get_output_name(DESC, OUT) ((DESC)->out[(OUT)]->name)
#define fts_dsp_get_output_size(DESC, OUT) ((DESC)->out[(OUT)]->length)
#define fts_dsp_get_output_srate(DESC, OUT) ((DESC)->out[(OUT)]->srate)

extern void fts_dsp_set_output(fts_dsp_descr_t *descr, int out, fts_dsp_signal_t *sig);

extern fts_dsp_signal_t *fts_dsp_get_privat_signal(int vs);
extern void fts_dsp_release_privat_signal(fts_dsp_signal_t *signal);

/* object declarations */
extern void fts_dsp_add_object(fts_object_t *o);
extern void fts_dsp_add_object_to_prolog(fts_object_t *o);
extern void fts_dsp_remove_object(fts_object_t *o);
extern void fts_dsp_remove_object_from_prolog(fts_object_t *o);

extern void fts_dsp_declare_inlet(fts_class_t *cl, int num);
extern void fts_dsp_declare_outlet(fts_class_t *cl, int num);

extern void fts_dsp_declare_function(fts_symbol_t name, ftl_wrapper_t fun);
extern void fts_dsp_add_function(fts_symbol_t name, int ac, fts_atom_t *at);

/* old names of user API */
#define dsp_list_insert(o) fts_dsp_add_object(o)
#define dsp_list_remove(o) fts_dsp_remove_object(o)
#define dsp_sig_inlet(c, i) fts_dsp_declare_inlet((c), (i))
#define dsp_sig_outlet(c, i) fts_dsp_declare_outlet((c), (i))
#define dsp_declare_function(n, f) fts_dsp_declare_function((n), (f))
#define dsp_add_funcall(s, n, a) fts_dsp_add_function((s), (n), (a))

extern void fts_dsp_auto_stop(void);
extern void fts_dsp_auto_restart(void);
extern void fts_dsp_auto_update(void);

/* internal API */
extern void dsp_add_signal(fts_symbol_t name, int vs);

extern void dsp_chain_create(int vs);
extern void dsp_chain_delete(void);

extern void dsp_chain_post(void);
extern void dsp_chain_post_signals(void);

extern void dsp_chain_fprint(FILE *f);
extern void dsp_chain_fprint_signals(FILE *f);

extern fts_object_t *dsp_get_current_object(void);

extern int dsp_is_running( void);
extern void dsp_make_dsp_off_chain(void);
extern ftl_program_t *dsp_get_current_dsp_chain( void);

extern fts_symbol_t fts_s_put;
extern fts_symbol_t fts_s_sig_zero;
extern fts_symbol_t fts_s_dsp_upsampling;
extern fts_symbol_t fts_s_dsp_downsampling;
extern fts_symbol_t fts_s_dsp_outputsize;
extern fts_symbol_t fts_s_dsp_descr;

/* the function that is called by the scheduler */
extern void fts_dsp_chain_poll( void);

#endif
