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


/**
 * Messaging
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
 * @defgroup mess_class
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
 * @fn void fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at)
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
 * @defgroup mess_dumper
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
 * @fn void fts_dumper_message_new(fts_dumper_t *dumper)
 * @param dumper the dumper
 * @ingroup mess_dumper
 */
FTS_API fts_message_t *fts_dumper_message_new(fts_dumper_t *dumper, fts_symbol_t selector);

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

/* Return status values */
FTS_API fts_status_description_t fts_ArgumentMissing;
FTS_API fts_status_description_t fts_ArgumentTypeMismatch;

/* The object stack; used for fpe handling, debug and who know what else in the future */
#define DO_OBJECT_STACK

#ifdef DO_OBJECT_STACK
FTS_API int fts_objstack_top; /* Next free slot; can overflow, must be checked */
FTS_API fts_object_t *fts_objstack[];

#define FTS_OBJSTACK_SIZE  8*1024
#define FTS_CALL_DEPTH     16*1024

#define FTS_REACHED_MAX_CALL_DEPTH()     (fts_objstack_top >= FTS_CALL_DEPTH) 

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

#define FTS_OBJSTACK_FULL()     (0) 
#define FTS_OBJSTACK_PUSH(obj)
#define FTS_OBJSTACK_POP(obj)
#define fts_get_current_object() (0)

#endif


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

/**
 * Method invokation, message sending and outlet API.
 *
 * @defgroup mess_api
 */

/**
 * Invoke a method with an unfolded list of arguments (tuple is converted to varargs).
 *
 * @fn int fts_invoke_varargs(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
 * @param method the method
 * @param o the target object
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_invoke_varargs(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at);

/**
 * Invoke a method with a single argument (ac, at is converted to tuple).
 *
 * @fn int fts_invoke_atom(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
 * @param method the method
 * @param o the target object
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_invoke_atom(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at);

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
FTS_API int fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Send a varargs message to an object (invoke method).
 *
 * @fn int fts_send_message_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the target object
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 * @return non-zero if succeeded, 0 if no varargs method found
 */
FTS_API int fts_send_message_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Output bang (void) from outlet.
 *
 * @fn void fts_outlet_bang(fts_object_t *o, int woutlet)
 * @param o the object
 * @param woutlet outlet index
 */
FTS_API void fts_outlet_bang(fts_object_t *o, int woutlet);

/**
 * Output integer value from outlet.
 *
 * @fn void fts_outlet_int(fts_object_t *o, int woutlet, int n)
 * @param o the object
 * @param woutlet outlet index
 * @param n the value
 */
FTS_API void fts_outlet_int(fts_object_t *o, int woutlet, int n);

/**
 * Output (double) float value from outlet.
 *
 * @fn void fts_outlet_float(fts_object_t *o, int woutlet, double f)
 * @param o the object
 * @param woutlet outlet index
 * @param f the value
 */
FTS_API void fts_outlet_float(fts_object_t *o, int woutlet, double f);

/**
 * Output symbol from outlet.
 *
 * @fn void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
 * @param o the object
 * @param woutlet outlet index
 * @param s the symbol
 */
FTS_API void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s);

/**
* Output object value from outlet.
 *
 * @fn void fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
 * @param o the object
 * @param woutlet outlet index
 * @param obj the object value
 */
FTS_API void fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj);

/**
 * Output any atom from outlet.
 *
 * @fn void fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* at)
 * @param o the object
 * @param woutlet outlet index
 * @param at the atom
 */
FTS_API void fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* at);

/**
 * Output array of values from outlet.
 *
 * @fn void fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
 * @param o the object
 * @param woutlet outlet index
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at);

/**
 * Output message (with varargs) from outlet.
 *
 * @fn void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the object
 * @param woutlet outlet index
 * @param s the message symbol
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Output message (with varargs) or any values from outlet.
 *
 * @fn void fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the object
 * @param woutlet outlet index
 * @param s the message symbol or NULL to output values
 * @param ac argument count
 * @param at argument values
 */
FTS_API void fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * Return a value from a method.
 * The returned value is an atom that is copied by the calling code.
 *
 * @fn void fts_return( fts_atom_t *p)
 * @param p a pointer to the atom to be returned
 * @ingroup message
 */
FTS_API void fts_return( fts_atom_t *p);
FTS_API void fts_return_int(int x);
FTS_API void fts_return_float(float x);
FTS_API void fts_return_symbol(fts_symbol_t x);
FTS_API void fts_return_object(fts_object_t *x);

FTS_API fts_atom_t *fts_get_return_value( void);


