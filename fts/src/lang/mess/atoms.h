#ifndef _ATOMS_H_
#define _ATOMS_H_

/*
 * Atoms are type-tagged words; i.e. an atom include a type tag,
 * and a fts_word_t value.
 * They are used everywhere in FTS as self described datums, like
 * in object messages and so on.
 * See mess_types.h for the typedefs.
 * 
 * The types tag is a symbols; to each basic type correspond 
 * a library symbol
 *
 * The special
 * symbol fts_s_void means a void value, while fts_s_anything
 * means any type (but cannot be used as type tag, is just used
 * in type checking).
 *
 * fts_s_true and fts_s_false are used to build boolean atoms.
*/

/* Macros for dealing with atoms: note the use of the 

   do {<macro-body} while (0) 

   trick, to be able to put the macro in a single statement "then"
   branch of an if ...
 */


/* Macros to deal with any type */

#define fts_set_type(ap, ttype)    (((ap)->type) = (ttype))
#define fts_get_type(ap)           ((ap)->type)
#define fts_is_a(ap, ttype)        (((ap)->type) == (ttype))
#define fts_get_value(ap)          (&((ap)->value))

/* specialized macros for predefined symbols */

#define fts_set_symbol(ap, x)       \
        do {fts_set_type(ap, fts_s_symbol); fts_word_set_symbol(fts_get_value(ap), (x));} while (0)

#define fts_set_string(ap, x)       \
        do {fts_set_type(ap, fts_s_string); fts_word_set_string(fts_get_value(ap), (x));} while (0)

#define fts_set_ptr(ap, x)         \
        do {fts_set_type(ap, fts_s_ptr); fts_word_set_ptr(fts_get_value(ap), (x));} while (0)

#define fts_set_int(ap, x)         \
     do {fts_set_type(ap, fts_s_int); fts_word_set_int(fts_get_value(ap), (x));} while (0)

#define fts_set_long(ap, x)        fts_set_int(ap, x)

#define fts_set_float(ap, x)       \
     do {fts_set_type(ap, fts_s_float); fts_word_set_float(fts_get_value(ap), (x));} while (0)

#define fts_set_object(ap, x)       \
     do {fts_set_type(ap, fts_s_object); fts_word_set_object(fts_get_value(ap), (x));} while (0)

#define fts_set_connection(ap, x)       \
     do {fts_set_type(ap, fts_s_connection); fts_word_set_connection(fts_get_value(ap), (x));} while (0)

#define fts_set_data(ap, x)       \
     do {fts_set_type(ap, fts_s_data); fts_word_set_data( fts_get_value(ap), (x));} while (0)

#define fts_set_void(ap)           (fts_set_type(ap, fts_s_void))

#define fts_set_true(ap)           (fts_set_type(ap, fts_s_true))
#define fts_set_false(ap)          (fts_set_type(ap, fts_s_false))


#define fts_get_symbol(ap)         (fts_word_get_symbol(fts_get_value(ap)))
#define fts_get_string(ap)         (fts_word_get_string(fts_get_value(ap)))
#define fts_get_ptr(ap)            (fts_word_get_ptr(fts_get_value(ap)))
#define fts_get_int(ap)            (fts_word_get_int(fts_get_value(ap)))
#define fts_get_long(ap)           fts_get_int(ap)
#define fts_get_float(ap)          (fts_word_get_float(fts_get_value(ap)))
#define fts_get_number(ap)         (fts_is_a(ap, fts_s_float) ? fts_get_float(ap) : fts_get_int(ap))


#define fts_get_double(ap)         (fts_is_a(ap, fts_s_float) ? (double) fts_get_float(ap) : (double)fts_get_int(ap))

#define fts_get_object(ap)         (fts_word_get_object(fts_get_value(ap)))
#define fts_get_connection(ap)     (fts_word_get_connection(fts_get_value(ap)))
#define fts_get_data(ap)         (fts_word_get_data(fts_get_value(ap)))

#define fts_is_void(ap)            fts_is_a(ap, fts_s_void)
#define fts_is_symbol(ap)          fts_is_a(ap, fts_s_symbol)
#define fts_is_string(ap)          fts_is_a(ap, fts_s_string)
#define fts_is_ptr(ap)             fts_is_a(ap, fts_s_ptr)
#define fts_is_int(ap)             fts_is_a(ap, fts_s_int)
#define fts_is_long(ap)            fts_is_int(ap)
#define fts_is_float(ap)           fts_is_a(ap, fts_s_float)
#define fts_is_number(ap)          fts_is_a(ap, fts_s_int) || fts_is_a(ap, fts_s_float)
#define fts_is_object(ap)          fts_is_a(ap, fts_s_object)
#define fts_is_connection(ap)      fts_is_a(ap, fts_s_connection)
#define fts_is_true(ap)            fts_is_a(ap, fts_s_true)
#define fts_is_false(ap)           fts_is_a(ap, fts_s_false)
#define fts_is_data(ap)            fts_is_a(ap, fts_s_data)

/* Convenience macros to deal with type and atom algebra */

#define fts_same_types(ap1, ap2)    (((ap1)->type) == (ap2)->type)

/* equality test between two atoms */

extern int fts_atom_equal(fts_atom_t *a1, fts_atom_t *a2);

/* null test: a null content can be a null pointer or a zero value */

extern int fts_atom_is_null(fts_atom_t *a);

/* Convenience macro for symbols */

#define fts_is_operator(a)        (fts_is_symbol((a)) && fts_symbol_is_operator(fts_get_symbol(a)))
#define fts_get_operator(a)        (fts_symbol_get_operator(fts_get_symbol(a)))

#endif





