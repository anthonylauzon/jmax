/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.12 $ IRCAM $Date: 1998/08/28 16:42:11 $
 *
 *  Eric Viara for Ircam, January 1995
 *
 */

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

/* Object functions and macros */

/* init function */

extern void fts_objects_init(void);

/* Object creation, deleting and replacing  */

#define FTS_NO_ID  -1

extern fts_object_t *fts_object_new(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern void          fts_object_set_id(fts_object_t *obj, int id);
extern void          fts_object_delete(fts_object_t *);
extern void          fts_object_send_properties(fts_object_t *obj);
extern void          fts_object_send_ui_properties(fts_object_t *obj);

/* This is only for object doctors (macros) */

fts_status_t fts_make_object(fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_object_t **ret);

/* Change the object description; more "system" oriented */

extern void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv);
extern void fts_object_set_description_and_class(fts_object_t *obj, fts_symbol_t class_name,
					  int argc, const fts_atom_t *argv);

/* Support for redefinition  */

extern fts_object_t *fts_object_recompute(fts_object_t *old);
extern fts_object_t *fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at);

/* Object Access */

#define fts_object_get_outlet_type(O, WOUTLET) (((fts_object_t *)(O))->cl->outlets[(WOUTLET)].tmess.symb)
#define fts_object_get_outlets_number(O)       (((fts_object_t *)(O))->cl->noutlets)
#define fts_object_get_inlets_number(O)        (((fts_object_t *)(O))->cl->ninlets)
#define fts_object_get_patcher(O)              (((fts_object_t *)(O))->patcher)

#define fts_object_get_variable(o)             ((o)->is_wannabe ? 0 : ((o)->varname))
#define fts_object_set_variable(o, name)       ((o)->varname = (name))

extern int fts_object_handle_message(fts_object_t *o, int winlet, fts_symbol_t s);
extern fts_symbol_t fts_object_get_class_name(fts_object_t *obj);

#define fts_object_is_outlet(o)  ((o)->cl->mcl == outlet_metaclass)
#define fts_object_is_inlet(o)   ((o)->cl->mcl == inlet_metaclass)

#define fts_object_get_id(o)     ((o)->id)


/* Test recursively if an object is inside a patcher (or its subpatchers) */

extern int  fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher);


/* Debug print */

extern void fprintf_object(FILE *f, fts_object_t *obj);
extern void post_object(fts_object_t *obj);

#endif
