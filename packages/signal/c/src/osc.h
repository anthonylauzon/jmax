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
 */

#ifndef _SIGNAL_OSC_H_
#define _SIGNAL_OSC_H_

#define OSC_TABLE_BITS 9
#define OSC_TABLE_SIZE 512

#include <fts/fts.h>
#include <fts/packages/data/data.h>

struct osc_ftl_symbols
{
  fts_symbol_t control_input;
  fts_symbol_t signal_input;
  fts_symbol_t signal_input_inplace;
};

/***************************************
 *
 *  osc
 *
 */
extern struct osc_ftl_symbols osc_ftl_symbols_ptr;
extern struct osc_ftl_symbols osc_ftl_symbols_fvec;

typedef struct _osc_
{
  fts_dsp_object_t o;
  ftl_data_t data;
  int fvec;
  double freq;
  double phase;
  double sr;
} osc_t;

extern ftl_data_t osc_data_new(void);
extern void osc_data_set_factor(ftl_data_t ftl_data, double sr);
extern void osc_data_set_incr(ftl_data_t ftl_data, double freq);
extern void osc_data_set_phase(ftl_data_t ftl_data, double phase);
extern void osc_data_set_fvec(ftl_data_t ftl_data, fvec_t *fvec);
extern void osc_data_set_ptr(ftl_data_t ftl_data, float *ptr);

extern void osc_declare_functions(void);

/***************************************
 *
 *  phi
 *
 */

extern struct osc_ftl_symbols phi_ftl_symbols;

extern ftl_data_t phi_data_new(void);
extern void phi_data_set_factor(ftl_data_t ftl_data, double sr);
extern void phi_data_set_incr(ftl_data_t ftl_data, double freq);
extern void phi_data_set_phase(ftl_data_t ftl_data, double phase);

extern void phi_declare_functions(fts_word_t *argv);

#endif
