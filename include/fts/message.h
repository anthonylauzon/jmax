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

#ifndef _FTS_MESSAGE_H_
#define _FTS_MESSAGE_H_

/**
 * Messages
 *
 * @defgroup message message 
 */

/**
 * The message class.
 * A message object can be created empty or with a symbol as the first argument.
 *
 * @code
 *   fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, NULL, ac, at);
 * @endcode
 *
 * @defgroup mess_class message class
 */
FTS_API fts_class_t *fts_message_class;

typedef struct
{
  fts_object_t o;
  fts_array_t args;
  fts_symbol_t s;
} fts_message_t;

#define fts_message_get_selector(m) ((m)->s)
#define fts_message_get_args(m) (&(m)->args)
#define fts_message_get_ac(m) (fts_array_get_size(&(m)->args))
#define fts_message_get_at(m) (fts_array_get_atoms(&(m)->args))

/**
 * Clear message (reset selector and arguments).
 *
 * @fn void fts_message_clear(fts_message_t *mess)
 * @param mess the message
 * @ingroup mess_class
 */
FTS_API void fts_message_clear(fts_message_t *mess);

/**
 * Set message.
 *
 * @fn void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param mess the message
 * @param s message symbol (selector)
 * @param ac argument count
 * @param at argument values
 * @ingroup mess_class
 */
FTS_API void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
* Set message from array (first element must be a symbol).
 *
 * @fn void fts_message_set_from_atoms(fts_message_t *mess, int ac, const fts_atom_t *at)
 * @param mess the message
 * @param ac array size
 * @param at array values
 * @ingroup mess_class
 */
FTS_API void fts_message_set_from_atoms(fts_message_t *mess, int ac, const fts_atom_t *at);

/**
 * Append arguments to a message.
 *
 * @fn void fts_message_append(fts_message_t *mess, int ac, const fts_atom_t *at)
 * @param mess the message
 * @param ac argument count
 * @param at argument values
 * @ingroup mess_class
 */
#define fts_message_append(m, n, a) (fts_array_append(&(m)->args, (n), (a)))
#define fts_message_append_int(m, x) (fts_array_append_int(&(m)->args, (x)))
#define fts_message_append_float(m, x) (fts_array_append_float(&(m)->args, (x)))
#define fts_message_append_symbol(m, x) (fts_array_append_symbol(&(m)->args, (x)))
#define fts_message_append_object(m, x) (fts_array_append_object(&(m)->args, (x)))

#define fts_is_message(p) (fts_is_a(p, fts_message_class))

#define fts_message_output(o, i, m) do { \
    fts_object_refer((fts_object_t *)(m)); \
    fts_outlet_send((o), (i), fts_message_get_selector(m), fts_message_get_ac(m), fts_message_get_at(m)); \
    fts_object_release((fts_object_t *)(m)); \
  } while(0)


/**
 * The message dumper.
 * The message dumper is an abstraction permitting objects to dump their state in form of messages in any context (file saving, protocols, etc.).
 * The current dumper structure includes a message which can be re-used for dumping. 
 *
 * @defgroup mess_dumper message dumper
 */

typedef struct fts_dumper
{
  fts_object_t head;
  fts_message_t *message;
  fts_method_t send;
} fts_dumper_t;

#define fts_dumper_get_message(d) ((d)->message)

/**
 * Initialize a newly created dumper.
 *
 * @fn void fts_dumper_init(fts_dumper_t *dumper, fts_method_t send)
 * @param dumper the dumper
 * @param send the dumper method
 * @ingroup mess_dumper
 */
FTS_API void fts_dumper_init(fts_dumper_t *dumper, fts_method_t send);

/**
 * Deallocate the state of a dumper.
 *
 * @fn void fts_dumper_destroy(fts_dumper_t *dumper)
 * @param dumper the dumper
 * @ingroup mess_dumper
 */
FTS_API void fts_dumper_destroy(fts_dumper_t *dumper);

/**
 * Get an empty message from the dumper.
 *
 * @fn void fts_dumper_message_get(fts_dumper_t *dumper, fts_symbol_t selector)
 * @param dumper the dumper
 * @param selector the selector
 * @ingroup mess_dumper
 */
FTS_API fts_message_t *fts_dumper_message_get(fts_dumper_t *dumper, fts_symbol_t selector);

/**
 * Dump a message (as object).
 *
 * @fn void fts_dumper_message_send(fts_dumper_t *dumper, fts_message_t *message)
 * @param dumper the dumper
 * @param message the message
 * @ingroup mess_dumper
 */
FTS_API void fts_dumper_message_send(fts_dumper_t *dumper, fts_message_t *message);

/**
 * Dump a message.
 *
 * @fn void fts_dumper_send(fts_dumper_t *dumper, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param dumper the dumper
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @ingroup mess_dumper
 */
FTS_API void fts_dumper_send(fts_dumper_t *dumper, fts_symbol_t s, int ac, const fts_atom_t *at);


/************************************************
 *
 *  message handling
 *
 */

/* argument macros and functions */
#define fts_get_symbol_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_symbol(&(AT)[N]) : (DEF))
#define fts_get_string_arg(AC, AT, N, DEF) ((N) < (AC) ? fts_get_string(&(AT)[N]) : (DEF))
#define fts_get_pointer_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_pointer(&(AT)[N]) : (DEF))
#define fts_get_object_arg(AC, AT, N, DEF)    ((N) < (AC) ? fts_get_object(&(AT)[N]) : (DEF))

#define fts_get_int_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ? (int) fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

#define fts_get_float_arg(AC, AT, N, DEF) \
((N) < (AC) ? (fts_is_int(&(AT)[N]) ? (float) fts_get_int(&(AT)[N]) : \
	      (fts_is_float(&(AT)[N]) ?  fts_get_float(&(AT)[N]) : (DEF))) : (DEF))

/* message cache */
typedef struct
{
  fts_symbol_t selector;
  fts_class_t *type;
  fts_method_t method;
} fts_message_cache_t;

FTS_API void fts_message_cache_init(fts_message_cache_t *cache);
FTS_API fts_message_cache_t *fts_message_cache_new(void);
FTS_API void fts_message_cache_free(fts_message_cache_t *cache);

#define fts_message_cache_get_selector(c) ((c)->selector)
#define fts_message_cache_get_type(c) ((c)->type)
#define fts_message_cache_get_method(c) ((c)->method)

#define fts_message_cache_set_selector(c, s) ((c)->selector = (s))
#define fts_message_cache_set_type(c, t) ((c)->type = (t))
#define fts_message_cache_set_method(c, m) ((c)->method = (m))

/**
 * Method invokation, message sending and outlet API.
 *
 * @defgroup mess_api method invokation, message sending and outlet API
 */

/**
 * Invoke a method.
 *
 * @fn int fts_invoke_method(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
 * @param method the method
 * @param o the target object
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_invoke_method(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at);

/**
 * Send an arbitrary message to an object (invoke method).
 *
 * @fn int fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the target object
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @return non-zero if succeeded, 0 if no method found for given arguments
 */
FTS_API fts_method_t fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Send an arbitrary cached message to an object (invoke method).
 *
 * @fn int fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_message_cache_t *cache)
 * @param o the target object
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @param cache message cache
 * @return non-zero if succeeded, 0 if no method found for given arguments
 */
FTS_API fts_method_t fts_send_message_cached(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_message_cache_t *cache);

/* deprecated */
#define fts_invoke_varargs(m, o, n, a) fts_invoke_method((m), (o), (n), (a))
#define fts_send_message_varargs(o, s, n, a) fts_send_message((o), (s), (n), (a))

/**
 * Return a value from a method.
 * The returned value is an atom that is copied by the calling code.
 * (Although the fts functions are called fts_<blabla>_float, the values
 * passed and stored in fts_atom_t are double!)
 *
 * @fn void fts_return( fts_atom_t *p)
 * @param p a pointer to the atom to be returned
 * @ingroup message
 */
FTS_API void fts_return( fts_atom_t *p);
FTS_API void fts_return_int(int x);
FTS_API void fts_return_float(double x);
FTS_API void fts_return_symbol(fts_symbol_t x);
FTS_API void fts_return_object(fts_object_t *x);

FTS_API fts_atom_t *fts_get_return_value( void);

#endif
