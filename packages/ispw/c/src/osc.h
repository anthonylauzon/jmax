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

#ifndef _ISPW_OSC_H_
#define _ISPW_OSC_H_

#include "ispw.h"
/************************************************************
 *
 *  osc~
 *
 */

typedef struct _osc_ftl osc_ftl_t;

ISPW_API ftl_data_t osc_ftl_data_new(void);
ISPW_API void osc_ftl_data_set_phase(ftl_data_t ftl_data, float phase);
ISPW_API void osc_ftl_data_set_table(ftl_data_t ftl_data, void *table);
ISPW_API void osc_ftl_data_init(ftl_data_t ftl_data, float sr);

ISPW_API void osc_ftl_dsp_put(fts_dsp_descr_t *dsp, ftl_data_t ftl_data);
ISPW_API void osc_ftl_declare_functions(void);

/************************************************************
 *
 *  phasor~
 *
 */

ISPW_API fts_symbol_t phasor_function;
ISPW_API fts_symbol_t phasor_inplace_function;

typedef struct _phasor_ftl phasor_ftl_t;

ISPW_API ftl_data_t phasor_ftl_data_new(void);
ISPW_API void phasor_ftl_data_init(ftl_data_t ftl_data, float sr);
ISPW_API void phasor_ftl_set_phase(ftl_data_t ftl_data, float phi);

ISPW_API void phasor_ftl(fts_word_t *argv);
ISPW_API void phasor_ftl_inplace(fts_word_t *argv);

#endif




