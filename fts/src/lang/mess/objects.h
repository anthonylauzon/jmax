/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.4 $ IRCAM $Date: 1998/04/23 16:07:21 $
 *
 *  Eric Viara for Ircam, January 1995
 *
 */

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

/* Object functions and macros */

/* Return status values */

extern fts_status_description_t fts_MethodNotFound;
extern fts_status_description_t fts_ObjectCannotConnect;
extern fts_status_description_t fts_NoSuchConnection;
extern fts_status_description_t fts_ArgumentMissing;
extern fts_status_description_t fts_ArgumentTypeMismatch;
extern fts_status_description_t fts_ExtraArguments;
extern fts_status_description_t fts_InvalidMessage;

/* init function */

extern void fts_objects_init(void);

/* Object creation, deleting and replacing  */

#define FTS_NO_ID  -1

extern fts_object_t *fts_object_new(fts_patcher_t *patcher, long id, int ac, const fts_atom_t *at);
extern void          fts_object_delete(fts_object_t *);
extern void          fts_object_send_properties(fts_object_t *obj);

/* .pat support *only*, don't use for new stuff */

extern void fts_object_change_description(fts_object_t *obj, int argc, const fts_atom_t *argv);

/* Connections */

extern fts_status_t fts_object_connect(fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern fts_status_t fts_object_disconnect(fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern fts_status_t fts_object_connect_perform(fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern fts_status_t fts_object_connect_outlet(fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern fts_status_t fts_object_disconnect_perform(fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern fts_status_t fts_object_disconnect_outlet(fts_object_t *, int woutlet, fts_object_t *, int winlet);

/* Messaging */

extern fts_status_t fts_message_send(fts_object_t *, int winlet, fts_symbol_t , int,  const fts_atom_t *);

/* NOt to be used by users, but called by the optimized macros ... */

extern fts_status_t fts_message_send_cache(fts_object_t *o, int winlet, fts_symbol_t s,
					   int ac, const fts_atom_t *at, fts_symbol_t *symb_cache, fts_method_t *mth_cache);


extern fts_status_t fts_outlet_send(fts_object_t *, int woutlet, fts_symbol_t , int, const fts_atom_t *);




/* argument macros and functions */

#define fts_get_long_arg(AC, AT, N, DEF)   ((N) < (AC) ? fts_get_long(&(AT)[N]) : (DEF))
#define fts_get_float_arg(AC, AT, N, DEF)  ((N) < (AC) ? fts_get_float(&(AT)[N]) : (DEF))
#define fts_get_symbol_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_symbol(&(AT)[N]) : (DEF))
#define fts_get_string_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_string(&(AT)[N]) : (DEF))
#define fts_get_ptr_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_ptr(&(AT)[N]) : (DEF))

#define fts_get_number_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_long(&(AT)[N]) ? fts_get_long(&(AT)[N]) : \
	       (fts_is_float(&(AT)[N]) ? fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_double_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_long(&(AT)[N]) ? (double) fts_get_long(&(AT)[N]) : \
	       (fts_is_float(&(AT)[N]) ? (double) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))


#define fts_get_long_by_name(AC, AR, NAME, DEF)   fts_get_int_by_name(AC, AR, NAME, DEF) 
extern long fts_get_int_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, int def);
extern float fts_get_float_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, float def);
extern fts_symbol_t fts_get_symbol_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, fts_symbol_t def);
extern long fts_get_boolean_by_name(int argc, const fts_atom_t *at,  fts_symbol_t name, int def);

/* inlined macros for message sending (active only if compiled optimized */

#ifdef OPTIMIZE

#define fts_send(CONN, S, AC, AT) \
\
while((CONN)) \
  { \
    if (((CONN)->symb == (S)) || (!(CONN)->symb && (CONN)->mth)) \
       (*(CONN)->mth)((CONN)->dst, (CONN)->winlet, (S), (AC), (AT)); \
    else \
       fts_message_send_cache((CONN)->dst, (CONN)->winlet, (S), (AC), (AT), &((CONN)->symb), &((CONN)->mth)); \
 \
    (CONN) = (CONN)->next_same_src; \
  }


#define fts_outlet_send(O, WOUTLET, PS, AC, AT) \
do { \
  fts_connection_t *__conn; \
  fts_symbol_t __s = (PS); \
 \
  __conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, __s, (AC), (AT)); \
} while (0)


#define fts_outlet_int(O, WOUTLET, N) \
do { \
  fts_connection_t *__conn; \
  fts_atom_t __a; \
 \
  fts_set_long(&__a, (N)); \
  \
  __conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, fts_s_int, 1, &__a); \
} while (0)


#define fts_outlet_float(O, WOUTLET, F) \
do { \
  fts_connection_t *__conn; \
  fts_atom_t __a; \
 \
  fts_set_float(&__a, (F)); \
  \
  __conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, fts_s_float, 1, &__a); \
} while (0)


#define fts_outlet_symbol(O, WOUTLET, S) \
do { \
  fts_connection_t *__conn; \
  fts_atom_t __a; \
 \
  fts_set_symbol(&__a, (S)); \
  \
  __conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, fts_s_symbol, 1, &__a); \
} while(0)


#define fts_outlet_bang(O, WOUTLET) \
do { \
  fts_connection_t *__conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, fts_s_bang, 0, 0); \
} while(0)


#define fts_outlet_list(O, WOUTLET, AC, AT) \
do { \
  fts_connection_t *__conn; \
                           \
  __conn = (O)->out_conn[(WOUTLET)]; \
 \
  fts_send(__conn, fts_s_list, (AC), (AT)); \
} while(0)

#else
/* Prototypes of the functions equivalent to the macros; actually
   implemented in messutil.c, but they have to be prototyped here
   with the macros !!!  
*/

extern void fts_outlet_bang(fts_object_t *o, int woutlet);
extern void fts_outlet_int(fts_object_t *o, int woutlet, int n);
extern void fts_outlet_float(fts_object_t *o, int woutlet, float f);
extern void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s);
extern void fts_outlet_list(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at);
#endif

/* Object Access */

#define fts_object_get_outlet_type(O, WOUTLET) (((fts_object_t *)(O))->cl->outlets[(WOUTLET)].tmess.symb)
#define fts_object_get_outlets_number(O)       (((fts_object_t *)(O))->cl->noutlets)
#define fts_object_get_inlets_number(O)        (((fts_object_t *)(O))->cl->ninlets)
#define fts_object_get_patcher(O)              (((fts_object_t *)(O))->patcher)

extern int fts_object_handle_message(fts_object_t *o, int winlet, fts_symbol_t s);
extern fts_symbol_t fts_object_get_class_name(fts_object_t *obj);






#define fts_object_is_outlet(o)  ((o)->cl->mcl == outlet_metaclass)
#define fts_object_is_inlet(o)   ((o)->cl->mcl == inlet_metaclass)

#define fts_object_get_id(o)     ((o)->id)




#endif
