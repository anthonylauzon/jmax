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


#ifndef _FTS_ATOMS_H_
#define _FTS_ATOMS_H_

/*
 * Atoms are type-tagged words; i.e. an atom include a type tag,
 * and a fts_word_t value.
 * They are used everywhere in FTS as self described data items, like
 * in object messages and so on.
 * See mess_types.h for the typedefs.
 * 
 * The types tag is a symbols; to each basic type correspond 
 * to a symbol predefined in symbol.h; basic types are supported by
 * means of a group of dedicated get/set macros, but the atoms are not limited
 * to basic types; the symbol can be set freely; the C type of the value part
 * should be considered as the representational type; it make sense for example
 * to have a type like "socket" that use for its representation the int C type.
 *
 * The special  symbol fts_s_void means a void value.
 * 
 * The special symbol fts_s_error means an error value, i.e. a value
 * that should generate an error each time somebody try to use it.
 *
 * fts_s_true and fts_s_false are used to build boolean atoms.
*/

/* Macros for dealing with atoms: note the use of the 

   do {<macro-body} while (0) 

   trick, to be able to put the macro in a single statement "then"
   branch of an if ... tricky hack, shitty language.
   
 */

/* init function */
FTS_API void fts_atoms_init(void);

/* An initializer for empty atoms */
#define FTS_NULL {(fts_symbol_t)0, {0}}

/* A variable initialized to an empty atom */
FTS_API const fts_atom_t fts_null;

FTS_API void fts_atom_type_register(fts_symbol_t name, fts_class_t *cl);
FTS_API int fts_atom_type_lookup(fts_symbol_t name, fts_class_t **cl);

/* Macros to deal with any type */
#define fts_set_type(ap, t) (((ap)->type) = (t))
#define fts_set_object_type(ap, t) (((ap)->type) = ((fts_symbol_t)((unsigned int)(t) | 1)))

#define fts_get_type(ap) ((fts_type_t)((unsigned int)((ap)->type) & ~1))
#define fts_is_a(ap, t) (fts_get_type(ap) == (t))

#define fts_get_selector(ap) (fts_type_get_selector(fts_get_type(ap)))

/* Return a pointer; i.e. you can write "*(fts_atom_value(a)).fts_symbol = fts_s_int;" for example */
#define fts_atom_value(ap) (&((ap)->value))

/* set object of any type */
#define fts_set_object_with_type(ap, x, t) \
     do {fts_set_object_type(ap, t); fts_word_set_object(fts_atom_value(ap), ((fts_object_t *)x));} while (0)

/* set atoms of predefined types */
#define fts_set_symbol(ap, x) \
        do {fts_set_type(ap, fts_s_symbol); fts_word_set_symbol(fts_atom_value(ap), (x));} while (0)
#define fts_set_string(ap, x) \
        do {fts_set_type(ap, fts_s_string); fts_word_set_string(fts_atom_value(ap), (x));} while (0)
#define fts_set_ptr(ap, x) \
        do {fts_set_type(ap, fts_s_ptr); fts_word_set_ptr(fts_atom_value(ap), (x));} while (0)
#define fts_set_fun(ap, x) \
        do {fts_set_type(ap, fts_s_fun); fts_word_set_fun(fts_atom_value(ap), (x));} while (0)
#define fts_set_int(ap, x) \
     do {fts_set_type(ap, fts_s_int); fts_word_set_int(fts_atom_value(ap), (x));} while (0)
#define fts_set_long(ap, x) fts_set_int(ap, x)
#define fts_set_float(ap, x) \
     do {fts_set_type(ap, fts_s_float); fts_word_set_float(fts_atom_value(ap), (x));} while (0)
#define fts_set_connection(ap, x)       \
     do {fts_set_type(ap, fts_s_connection); fts_word_set_connection(fts_atom_value(ap), (x));} while (0)
#define fts_set_data(ap, x) \
     do {fts_set_type(ap, fts_s_data); fts_word_set_data( fts_atom_value(ap), (x));} while (0)
#define fts_set_list(ap, x) \
     do {fts_set_type(ap, fts_s_list); fts_word_set_data(fts_atom_value(ap), ((fts_data_t *)x));} while (0)
#define fts_set_object(ap, x) \
     do {fts_set_object_type(ap, fts_s_object); fts_word_set_object(fts_atom_value(ap), (x));} while (0)

#define fts_set_void(ap) (fts_set_type(ap, fts_s_void))
#define fts_set_error(ap) (fts_set_type(ap, fts_s_error))
#define fts_set_true(ap) (fts_set_type(ap, fts_s_true))
#define fts_set_false(ap) (fts_set_type(ap, fts_s_false))

/* get values of atoms of predefined types */
#define fts_get_int(ap) (fts_word_get_int(fts_atom_value(ap)))
#define fts_get_long(ap) fts_get_int(ap)
#define fts_get_float(ap) (fts_word_get_float(fts_atom_value(ap)))
#define fts_get_number_int(ap) (fts_is_a(ap, fts_s_float) ? (int)fts_get_float(ap) : fts_get_int(ap))
#define fts_get_number_float(ap) (fts_is_a(ap, fts_s_float) ? fts_get_float(ap) : (float)fts_get_int(ap))
#define fts_get_double(ap) (fts_is_a(ap, fts_s_float) ? (double) fts_get_float(ap) : (double)fts_get_int(ap))
#define fts_get_symbol(ap) (fts_word_get_symbol(fts_atom_value(ap)))
#define fts_get_string(ap) (fts_word_get_string(fts_atom_value(ap)))
#define fts_get_ptr(ap) (fts_word_get_ptr(fts_atom_value(ap)))
#define fts_get_fun(ap) (fts_word_get_fun(fts_atom_value(ap)))
#define fts_get_connection(ap) (fts_word_get_connection(fts_atom_value(ap)))
#define fts_get_data(ap) (fts_word_get_data(fts_atom_value(ap)))
#define fts_get_list(ap) ((fts_list_t *)fts_word_get_data(fts_atom_value(ap)))
#define fts_get_object(ap) (fts_word_get_object(fts_atom_value(ap)))

/* check atoms for predefined types */
#define fts_is_int(ap) fts_is_a(ap, fts_s_int)
#define fts_is_long(ap) fts_is_int(ap)
#define fts_is_float(ap) fts_is_a(ap, fts_s_float)
#define fts_is_number(ap) (fts_is_a(ap, fts_s_int) || fts_is_a(ap, fts_s_float))
#define fts_is_symbol(ap) fts_is_a(ap, fts_s_symbol)
#define fts_is_string(ap) fts_is_a(ap, fts_s_string)
#define fts_is_ptr(ap) fts_is_a(ap, fts_s_ptr)
#define fts_is_fun(ap) fts_is_a(ap, fts_s_fun)
#define fts_is_void(ap) fts_is_a(ap, fts_s_void)
#define fts_is_error(ap) fts_is_a(ap, fts_s_error)
#define fts_is_connection(ap) fts_is_a(ap, fts_s_connection)
#define fts_is_true(ap) fts_is_a(ap, fts_s_true)
#define fts_is_false(ap) fts_is_a(ap, fts_s_false)
#define fts_is_data(ap) (fts_is_a(ap, fts_s_data))
#define fts_is_list(ap) (fts_is_a(ap, fts_s_list))
#define fts_is_object(ap) ((unsigned int)((ap)->type) & 1)

/* atom that can be objects */
#define fts_atom_refer(ap) do {if(fts_is_object(ap)) fts_object_refer(fts_get_object(ap));} while(0)
#define fts_atom_release(ap) do {if(fts_is_object(ap)) fts_object_release(fts_get_object(ap));} while(0)

#define fts_atom_assign_object(ap, o) \
        do {if(fts_is_object(ap)) fts_atom_release(ap); \
        fts_set_object((ap), o); fts_object_refer(o);} while(0)

#define fts_atom_assign(a1p, a2p) \
        do {if(fts_is_object(a1p)) fts_atom_release(a1p); \
        *(a1p) = *(a2p); \
        if(fts_is_object(a2p)) fts_atom_refer(a2p);} while(0)

#define fts_atom_void(ap) \
        do {if(fts_is_object(ap)) fts_atom_release(ap); fts_set_void(ap);} while(0)

/* Convenience macros to deal with type and atom algebra */
#define fts_same_types(ap1, ap2) (((ap1)->type) == (ap2)->type)

/* equality test between two atoms */
FTS_API int fts_atom_are_equals(const fts_atom_t *a1, const fts_atom_t *a2);

/* null test: a null content can be a null pointer or a zero value */
FTS_API int fts_atom_is_null(const fts_atom_t *a);

/* Subsequence  testing */
FTS_API int fts_atom_is_subsequence(int sac, const fts_atom_t *sav, int ac, const fts_atom_t *av);

/* Atom printing function, usually for debug */
FTS_API void fprintf_atoms(FILE *f, int ac, const fts_atom_t *at);
FTS_API void sprintf_atoms(char *s, int ac, const fts_atom_t *at);

#endif
