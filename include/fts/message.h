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


#ifndef _FTS_MESSAGE_H_
#define _FTS_MESSAGE_H_

/************************************************
 *
 *  message object
 *
 */

typedef struct
{
  fts_array_t args;
  fts_symbol_t s;
} fts_message_t;

FTS_API fts_class_t *fts_message_class;

#define fts_message_get_selector(m) ((m)->s)
#define fts_message_get_args(m) (&(m)->args)
#define fts_message_get_ac(m) (fts_array_get_size(&(m)->args))
#define fts_message_get_at(m) (fts_array_get_atoms(&(m)->args))

#define fts_message_append(m, n, a) (fts_array_append(&(m)->args, (n), (a)))
#define fts_message_append_int(m, x) (fts_array_append_int(&(m)->args, (x)))
#define fts_message_append_float(m, x) (fts_array_append_float(&(m)->args, (x)))
#define fts_message_append_symbol(m, x) (fts_array_append_symbol(&(m)->args, (x)))

FTS_API void fts_message_clear(fts_message_t *mess);
FTS_API void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);

#define fts_message_output(o, i, m) do { \
    fts_object_refer((fts_object_t *)(m)); \
    fts_outlet_send((o), (i), fts_message_get_selector(m), fts_message_get_ac(m), fts_message_get_at(m)); \
    fts_object_release((fts_object_t *)(m)); \
  } while(0);


/************************************************
 *
 *  message dumper
 *
 */

typedef struct fts_dumper
{
  fts_object_t head;
  fts_message_t *message;
  fts_method_t send;
} fts_dumper_t;

#define fts_dumper_get_message(d) ((d)->message)

FTS_API void fts_dumper_init(fts_dumper_t *dumper, fts_method_t send);
FTS_API void fts_dumper_destroy(fts_dumper_t *dumper);

FTS_API fts_message_t *fts_dumper_message_new(fts_dumper_t *dumper, fts_symbol_t selector);
FTS_API void fts_dumper_message_send(fts_dumper_t *dumper, fts_message_t *message);
FTS_API void fts_dumper_send(fts_dumper_t *dumper, fts_symbol_t s, int ac, const fts_atom_t *at);


/************************************************
 *
 *  message handling
 *
 */

/* Return status values */
FTS_API fts_status_description_t fts_MethodNotFound;
FTS_API fts_status_description_t fts_ArgumentMissing;
FTS_API fts_status_description_t fts_ArgumentTypeMismatch;
FTS_API fts_status_description_t fts_ExtraArguments;
FTS_API fts_status_description_t fts_InvalidMessage;

/* init function */
FTS_API void fts_messages_init(void);

/* The object stack; used for fpe handling, debug and who know what else in the future */

#define DO_OBJECT_STACK

#ifdef DO_OBJECT_STACK
FTS_API int fts_objstack_top; /* Next free slot; can overflow, must be checked */
FTS_API fts_object_t *fts_objstack[];

#define FTS_OBJSTACK_SIZE  8*1024

#define FTS_OBJSTACK_PUSH(obj)   { if (fts_objstack_top < FTS_OBJSTACK_SIZE)  \
                                      fts_objstack[fts_objstack_top++] = (obj); \
				   else \
				      fts_objstack_top++; \
				 }

#define FTS_OBJSTACK_POP(obj)    (fts_objstack_top--)

#define fts_get_current_object() (((fts_objstack_top > 0) && (fts_objstack_top <= FTS_OBJSTACK_SIZE)) ? \
				  fts_objstack[fts_objstack_top - 1] : \
				  (fts_object_t *)0)
#else

#define FTS_OBJSTACK_PUSH(obj)
#define FTS_OBJSTACK_POP(obj)
#define fts_get_current_object() (0)

#endif

/* Messaging */
FTS_API fts_status_t fts_send_message(fts_object_t *, int winlet, fts_symbol_t , int,  const fts_atom_t *);

/* NOt to be used by users, but called by the optimized macros ... */
FTS_API fts_status_t fts_send_message_cache(fts_object_t *o, int winlet, fts_symbol_t s,
					   int ac, const fts_atom_t *at, fts_symbol_t *symb_cache, fts_method_t *mth_cache);


FTS_API fts_status_t fts_outlet_send(fts_object_t *, int woutlet, fts_symbol_t , int, const fts_atom_t *);


/* argument macros and functions */
#define fts_get_symbol_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_symbol(&(AT)[N]) : (DEF))
#define fts_get_string_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_string(&(AT)[N]) : (DEF))
#define fts_get_ptr_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_ptr(&(AT)[N]) : (DEF))
#define fts_get_object_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_object(&(AT)[N]) : (DEF))

#define fts_get_int_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ? (int) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_float_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? (float) fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ?  fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_double_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? (double) fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ? (double) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

/* inlined macros for message sending (active only if compiled optimized */
#ifdef OPTIMIZE

#define fts_send(CONN, S, AC, AT) \
\
while((CONN)) \
  { \
    if (((CONN)->symb == (S)) || (!(CONN)->symb && (CONN)->mth)) \
       {							  \
           FTS_OBJSTACK_PUSH((CONN)->dst);                        \
           (*(CONN)->mth)((CONN)->dst, (CONN)->winlet, (S), (AC), (AT)); \
           FTS_OBJSTACK_POP((CONN)->dst);                         \
       } \
    else \
       fts_send_message_cache((CONN)->dst, (CONN)->winlet, (S), (AC), (AT), &((CONN)->symb), &((CONN)->mth)); \
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
  fts_set_int(&__a, (N)); \
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

FTS_API void fts_outlet_bang(fts_object_t *o, int woutlet);
FTS_API void fts_outlet_int(fts_object_t *o, int woutlet, int n);
FTS_API void fts_outlet_float(fts_object_t *o, int woutlet, float f);
FTS_API void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s);
FTS_API void fts_outlet_list(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at);

#endif

#endif
