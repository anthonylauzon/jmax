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
 * The message class.
 * A message object can be created empty or with a symbol as the first argument.
 *
 * @code
 *   fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, NULL, ac, at);
 * @endcode
 *
 * @defgroup message message
 * @ingroup fts_obj_class_method
 */

/** 
 * @var fts_class_t *fts_message_class
 * @brief the message class ... 
 * @ingroup message 
 */
FTS_API fts_class_t *fts_message_class;

/**
 * @typedef struct fts_message fts_message_t
 * @brief fts message
 * @ingroup message
 */
/**
 * @struct fts_message
 * @brief fts message struct
 * @ingroup message
 */
typedef struct fts_message
{
  fts_object_t o;/**< o ....*/
  fts_array_t args;/**< args ....*/
  fts_symbol_t s;/**< message selector */
} fts_message_t;

/**
 * @def fts_message_get_selector(m) 
 * @brief returns message selector \n --> fts_symbol_t fts_message_get_selector(fts_message_t *m)
 * @param m fts message
 * @return message selector
 * @ingroup message
 */
#define fts_message_get_selector(m) ((m)->s)
/**
 * @def fts_message_get_args(m) 
 * @brief returns message args \n --> fts_array_t *fts_message_get_args(fts_message_t *m)
 * @param m fts message
 * @return message args
 * @ingroup message
 */
#define fts_message_get_args(m) (&(m)->args)
/**
 * @def fts_message_get_ac(m) 
 * @brief returns message arguments count \n --> int fts_message_get_ac(fts_message_t *m)
 * @param m fts message
 * @return arguments count
 * @ingroup message
 */
#define fts_message_get_ac(m) (fts_array_get_size(&(m)->args))
/**
 * @def fts_message_get_at(m) 
 * @brief returns message args as atoms array \n --> fts_atom_t * fts_message_get_at(fts_message_t *m)
 * @param m fts message
 * @return args as atoms array
 * @ingroup message
 */
#define fts_message_get_at(m) (fts_array_get_atoms(&(m)->args))

/**
 * Clear message (reset selector and arguments).
 *
 * @fn void fts_message_clear(fts_message_t *mess)
 * @brief clear message
 * @param mess the message
 * @ingroup message
 */
FTS_API void fts_message_clear(fts_message_t *mess);

/**
 * Set message.
 *
 * @fn void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @brief set message
 * @param mess the message
 * @param s message symbol (selector)
 * @param ac argument count
 * @param at argument values
 * @ingroup message
 */
FTS_API void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Set message from array (first element must be a symbol).
 *
 * @fn void fts_message_set_from_atoms(fts_message_t *mess, int ac, const fts_atom_t *at)
 * @brief set message from array
 * @param mess the message
 * @param ac array size
 * @param at array values
 * @ingroup message
 */
FTS_API void fts_message_set_from_atoms(fts_message_t *mess, int ac, const fts_atom_t *at);


#ifdef AVOID_MACROS
/**
 * Append arguments to a message.
 *
 * @fn void fts_message_append(fts_message_t *mess, int ac, const fts_atom_t *at)
 * @brief append arguments to message
 * @param mess the message
 * @param ac argument count
 * @param at argument values
 * @ingroup message
 */
FTS_API void fts_message_append(fts_message_t *mess, int ac, fts_atom_t *at);
/**
 * @fn void fts_message_append_int(fts_message_t *mess, int val)
 * @brief append int argument to message
 * @param mess the message
 * @param val int argument
 * @ingroup message
 */
FTS_API void fts_message_append_int(fts_message_t *mess, int val);
/**
 * @fn void fts_message_append_float(fts_message_t *mess, float val)
 * @brief append float argument to message
 * @param mess the message
 * @param val float argument
 * @ingroup message
 */
FTS_API void fts_message_append_float(fts_message_t *mess, float val);
/**
 * @fn void fts_message_append_symbol(fts_message_t *mess, fts_symbol_t val)
 * @brief append symbol argument to message
 * @param mess the message
 * @param val symbol argument
 * @ingroup message
 */
FTS_API void fts_message_append_symbol(fts_message_t *mess, fts_symbol_t val);
/**
 * @fn void fts_message_append_object(fts_message_t *mess, fts_object_t *val)
 * @brief append fts_object argument to message
 * @param mess the message
 * @param val fts_object argument
 * @ingroup message
 */
FTS_API void fts_message_append_object(fts_message_t *mess, fts_object_t *val);
/**
 * @fn int fts_is_message(fts_atom_t *a)
 * @brief tells if atom content is a message
 * @param a the atom
 * @return 1 if true, 0 if false
 * @ingroup message
 */
FTS_API int fts_is_message(fts_atom_t *a);
/**
 * @fn void fts_message_output(fts_object _t *o, int idx, fts_message_t *m)
 * @brief send the message on the given outlet
 * @param o the object
 * @param idx outlet index
 * @param m the message
 * @ingroup message
 */
FTS_API void fts_message_output(fts_object _t *o, int idx, fts_message_t *m);
#else
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
#endif
/************************************************
 *
 *  message handling
 *
 */

/* argument macros and functions */
#ifdef AVOID_MACROS
/**
 * @fn fts_symbol_t fts_get_symbol_arg(int ac, const fts_atom_t *at, int n, fts_symbol_t def)
 * @brief get symbol argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return symbol arg
 * @ingroup message
 */
FTS_API fts_symbol_t fts_get_symbol_arg(int ac, const fts_atom_t *at, int n, fts_symbol_t def);
/**
 * @fn char *fts_get_string_arg(int ac, const fts_atom_t *at, int n, char *def)
 * @brief get string argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return string arg
 * @ingroup message
 */
FTS_API char *fts_get_string_arg(int ac, const fts_atom_t *at, int n, char *def);
/**
 * @fn void *fts_get_pointer_arg(int ac, const fts_atom_t *at, int n, void *def)
 * @brief get pointer argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return pointer arg
 * @ingroup message
 */
FTS_API void *fts_get_pointer_arg(int ac, const fts_atom_t *at, int n, void *def);
/**
 * @fn fts_object_t *fts_get_object_arg(int ac, const fts_atom_t *at, int n, fts_object_t *def)
 * @brief get fts_object argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return fts_object arg
 * @ingroup message
 */
FTS_API fts_object_t *fts_get_object_arg(int ac, const fts_atom_t *at, int n, fts_object_t *def);
/**
 * @fn int fts_get_int_arg(int ac, const fts_atom_t *at, int n, int def)
 * @brief get int argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return int arg
 * @ingroup message
 */
FTS_API int fts_get_int_arg(int ac, const fts_atom_t *at, int n, int def);
/**
 * @fn FTS_API float fts_get_float_arg(int ac, const fts_atom_t *at, int n, float def)
 * @brief get float argument 
 * @param ac args count
 * @param at args 
 * @param n arg index
 * @param def default value
 * @return float arg
 * @ingroup message
 */
FTS_API float fts_get_float_arg(int ac, const fts_atom_t *at, int n, float def);
#else
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
#endif

/* message cache */
/**
 * @typedef struct _fts_message_cache_ fts_message_cache_t
 * @brief fts message cache
 * @ingroup message
 */
/**
 * @struct _fts_message_cache_
 * @brief fts message cache struct
 * @ingroup message
 */
typedef struct _fts_message_cache_
{
  fts_symbol_t selector;/**< selector ... */
  fts_class_t *type;/**< type ... */
  fts_method_t method;/**< method ... */
} fts_message_cache_t;

/**
 * @fn void fts_message_cache_init(fts_message_cache_t *cache)
 * @brief massage cache init function 
 * @param cache the massage cache
 * @ingroup message
 */
FTS_API void fts_message_cache_init(fts_message_cache_t *cache);
/**
 * @fn fts_message_cache_t *fts_message_cache_new(void)
 * @brief massage cache new function 
 * @return the massage cache
 * @ingroup message
 */
FTS_API fts_message_cache_t *fts_message_cache_new(void);
/**
 * @fn void fts_message_cache_free(fts_message_cache_t *cache)
 * @brief massage cache free function 
 * @param cache the massage cache
 * @ingroup message
 */
FTS_API void fts_message_cache_free(fts_message_cache_t *cache);

/**
 * @def fts_message_cache_get_selector(c)
 * @brief returns message cache selector \n --> fts_symbol_t fts_message_cache_get_selector(fts_message_cache_t *c)
 * @param c message cache
 * @return massage cache selector
 * @ingroup message
 */
#define fts_message_cache_get_selector(c) ((c)->selector)
/**
 * @def fts_message_cache_get_type(c)
 * @brief returns message cache type \n --> fts_class_t * fts_message_cache_get_type(fts_message_cache_t *c)
 * @param c message cache
 * @return massage cache type
 * @ingroup message
 */
#define fts_message_cache_get_type(c) ((c)->type)
/**
 * @def fts_message_cache_get_method(c)
 * @brief returns message cache type \n --> fts_method_t fts_message_cache_get_method(fts_message_cache_t *c)
 * @param c message cache
 * @return massage cache method
 * @ingroup message
 */
#define fts_message_cache_get_method(c) ((c)->method)
/**
 * @def fts_message_cache_set_selector(c, s)
 * @brief set message cache selector \n --> void fts_message_cache_set_selector(fts_message_cache_t *c,  fts_symbol_t s)
 * @param c message cache
 * @param selector
 * @ingroup message
 */
#define fts_message_cache_set_selector(c, s) ((c)->selector = (s))
/**
 * @def fts_message_cache_set_type(c, t)
 * @brief set message cache type \n --> void fts_message_cache_set_type(fts_message_cache_t *c,  fts_class_t *t)
 * @param c message cache
 * @param type
 * @ingroup message
 */
#define fts_message_cache_set_type(c, t) ((c)->type = (t))
/**
 * @def fts_message_cache_set_method(c, m)
 * @brief set message cache method \n --> void fts_message_cache_set_method(fts_message_cache_t *c,  fts_method_t t)
 * @param c message cache
 * @param method
 * @ingroup message
 */
#define fts_message_cache_set_method(c, m) ((c)->method = (m))

/**
 * Invoke a method.
 *
 * @fn void fts_invoke_method(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief invoke a method
 * @param method the method
 * @param o the target object
 * @param ac argument count
 * @param at argument values
 * @ingroup message
 */
FTS_API void fts_invoke_method(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at, fts_atom_t *ret);

/**
 * Send an arbitrary message to an object (invoke method).
 *
 * @fn fts_method_t fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief send message to an object
 * @param o the target object
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @return non-zero if succeeded, 0 if no method found for given arguments
 * @ingroup message
 */
FTS_API fts_method_t fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

/**
 * Send an arbitrary cached message to an object (invoke method).
 *
 * @fn int fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret, fts_message_cache_t *cache)
 * @brief send a cached message to an object
 * @param o the target object
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @param cache message cache
 * @return non-zero if succeeded, 0 if no method found for given arguments
 * @ingroup message
 */
FTS_API fts_method_t fts_send_message_cached(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret, fts_message_cache_t *cache);

/* deprecated */
#define fts_invoke_varargs(m, o, n, a) do { fts_atom_t _r = *fts_null; fts_invoke_method((m), (o), (n), (a), &_r); } while(0)
#define fts_send_message_varargs(o, s, n, a) do { fts_atom_t _r = *fts_null; fts_send_message((o), (s), (n), (a), &_r); } while(0)

#endif
