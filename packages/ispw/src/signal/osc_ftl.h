/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _OSC_FTL_H_
#define _OSC_FTL_H_

typedef struct _osc_ftl osc_ftl_t;

extern ftl_data_t osc_ftl_data_new(void);
extern void osc_ftl_data_set_phase(ftl_data_t ftl_data, float phase);
extern void osc_ftl_data_set_table(ftl_data_t ftl_data, void *table);
extern void osc_ftl_data_init(ftl_data_t ftl_data, float sr);

extern void osc_ftl_dsp_put(fts_dsp_descr_t *dsp, ftl_data_t ftl_data);
extern void osc_ftl_declare_functions(void);

#endif




