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
