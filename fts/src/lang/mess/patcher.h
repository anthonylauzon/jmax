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
#ifndef _PATCHER_H_
#define _PATCHER_H_

extern fts_metaclass_t *patcher_metaclass;
extern fts_metaclass_t *inlet_metaclass;
extern fts_metaclass_t *outlet_metaclass;

#define fts_object_is_patcher(o) ((o)->cl->mcl == patcher_metaclass)



#define fts_patcher_set_standard(p)      ((p)->type = fts_p_standard)
#define fts_patcher_set_abstraction(p)   ((p)->type = fts_p_abstraction)
#define fts_patcher_set_error(p)         ((p)->type = fts_p_error)
extern void fts_patcher_set_template(fts_patcher_t *patcher, fts_template_t *template);

#define fts_patcher_is_standard(p)       ((p)->type == fts_p_standard)
#define fts_patcher_is_abstraction(p)    ((p)->type == fts_p_abstraction)
#define fts_patcher_is_error(p)          ((p)->type == fts_p_error)
#define fts_patcher_is_template(p)       ((p)->type == fts_p_template)

#define fts_patcher_get_env(p)           (&((p)->env))

#define fts_object_is_abstraction(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_abstraction((fts_patcher_t *) (o)))

#define fts_object_is_standard_patcher(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_standard((fts_patcher_t *) (o)))

#define fts_object_is_error(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_error((fts_patcher_t *) (o)))

#define fts_object_is_template(o) (fts_object_is_patcher((o)) &&  \
				      fts_patcher_is_template((fts_patcher_t *) (o)))

#define fts_object_is_object(o) ((! fts_object_is_patcher((o))) || fts_object_is_error((o)))

#define fts_patcher_is_open(p)    ((p)->open)
#define fts_object_patcher_is_open(o)    ((fts_object_get_patcher(o))->open)

extern void fts_inlet_reposition(fts_object_t *o, int pos);
extern void fts_outlet_reposition(fts_object_t *o, int pos);

extern void fts_patcher_redefine_number_of_inlets(fts_patcher_t *this, int new_ninlets);
extern void fts_patcher_redefine_number_of_outlets(fts_patcher_t *this, int new_noutlets);


extern fts_patcher_t *fts_patcher_redefine(fts_patcher_t *this, int aoc, const fts_atom_t *aot);

extern fts_object_t *fts_patcher_get_inlet(fts_object_t *patcher, int inlet);
extern fts_object_t *fts_patcher_get_outlet(fts_object_t *patcher, int outlet);

extern fts_object_t *fts_outlet_get_next_inlet(fts_object_t *outlet);
extern fts_object_t *fts_inlet_get_next_inlet(fts_object_t *inlet);

extern fts_patcher_t *fts_get_root_patcher(void);

#endif








