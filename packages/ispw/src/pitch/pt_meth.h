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
/* pt_meth.h */

#ifndef _PT_METH_H_
#define _PT_METH_H_

#include "fts.h"


typedef void (*analysis_t)(fts_object_t *);

extern void pt_common_instantiate(fts_class_t *cl);
extern int pt_common_init(pt_common_obj_t *x, long n_points, long n_period);
extern void pt_common_delete(pt_common_obj_t *o);
extern void pt_common_dsp_fun_put(pt_common_obj_t *o, fts_dsp_descr_t *dsp); /* to set proper sampling rate */
extern void pt_common_dsp_function(fts_word_t *a);

extern void pt_common_print_obj(pt_common_obj_t *o);
extern void pt_common_print_ctl(pt_common_obj_t *o);
extern int pt_common_debounce_time_is_up(pt_common_obj_t *o, float* time);


#endif /* _PT_METH_H_ */
