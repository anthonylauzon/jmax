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


#ifndef _FTS_MESSAGES_H_
#define _FTS_MESSAGES_H_

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

#define fts_message_send  fts_send_message
FTS_API fts_status_t fts_send_message(fts_object_t *, int winlet, fts_symbol_t , int,  const fts_atom_t *);

/* NOt to be used by users, but called by the optimized macros ... */

FTS_API fts_status_t fts_send_message_cache(fts_object_t *o, int winlet, fts_symbol_t s,
					   int ac, const fts_atom_t *at, fts_symbol_t *symb_cache, fts_method_t *mth_cache);


FTS_API fts_status_t fts_outlet_send(fts_object_t *, int woutlet, fts_symbol_t , int, const fts_atom_t *);


FTS_API void fts_mess_set_run_time_check(int flag);
FTS_API int fts_mess_get_run_time_check(void);

/* argument macros and functions */

#define fts_get_symbol_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_symbol(&(AT)[N]) : (DEF))
#define fts_get_string_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_string(&(AT)[N]) : (DEF))
#define fts_get_ptr_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_ptr(&(AT)[N]) : (DEF))

#define fts_get_int_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ? (int) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_float_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? (float) fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ?  fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_double_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_long(&(AT)[N]) ? (double) fts_get_long(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ? (double) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_long_arg(AC, AT, N, DEF)   fts_get_int_arg(AC, AT, N, DEF)

#define fts_get_long_by_name(AC, AR, NAME, DEF)   fts_get_int_by_name(AC, AR, NAME, DEF) 
FTS_API long fts_get_int_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, int def);
FTS_API float fts_get_float_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, float def);
FTS_API fts_symbol_t fts_get_symbol_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, fts_symbol_t def);
FTS_API long fts_get_boolean_by_name(int argc, const fts_atom_t *at,  fts_symbol_t name, int def);

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

FTS_API void fts_outlet_bang(fts_object_t *o, int woutlet);
FTS_API void fts_outlet_int(fts_object_t *o, int woutlet, int n);
FTS_API void fts_outlet_float(fts_object_t *o, int woutlet, float f);
FTS_API void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s);
FTS_API void fts_outlet_list(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at);
#endif

#endif



