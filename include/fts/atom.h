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


#include <stdio.h>

/**
 * Atom accessors and helper functions
 *
 * @defgroup atom atom
 */

/*
 * Definition of types and predefined values
 *
 * The type of an atom is a pointer to a class.
 *
 */

/**
 * Get the class of the atom
 * 
 * @fn fts_class_t *fts_get_class( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the type of the atom as a fts_class_t *
 * @ingroup atom
 */
#define fts_get_class(p) (p)->type
#define fts_get_class_id(p) ((p)->type->type_id)

/**
 * Get the name associated with the type of the atom.
 * 
 * @fn fts_symbol_t fts_get_class_name( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the selector
 * @ingroup atom
 */
#define fts_get_class_name(p) fts_class_get_name( fts_get_class(p))

/** 
 * Check type of an atom
 *
 * @fn int fts_is_a(fts_atom_t* p, fts_class_t* cl)
 * @param p pointer to the the atom
 * @param c the type of the atom as a fts_class_t*
 * @return 1 if atom is of the given type
 * @ingroup atom
 */
#define fts_is_a(p,c) ((p)->type == (c))

/**
 * Class representing the 'void' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_void_class;

/**
 * Class representing the 'int' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_int_class;

/**
 * Class representing the 'float' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_float_class;

/**
 * Class representing the 'symbol' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_symbol_class;

/**
 * Class representing the 'pointer' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_pointer_class;

/**
 * Class representing the 'string' primitive type
 * @ingroup atom
 */
FTS_API fts_class_t *fts_string_class;

/*
 * fts_word_t accessors
 */

#define fts_word_set_int(p, v)         ((p)->fts_int = (v))
#define fts_word_set_float(p, v)      ((p)->fts_float = (v))
#define fts_word_set_symbol(p, v)      ((p)->fts_symbol = (v))
#define fts_word_set_object(p, v)      ((p)->fts_object = (v))
#define fts_word_set_pointer(p, v)     ((p)->fts_pointer = (v))
#define fts_word_set_string(p, v)      ((p)->fts_string = (v))

#define fts_word_get_int(p)            ((p)->fts_int)
#define fts_word_get_float(p)         ((p)->fts_float)
#define fts_word_get_symbol(p)         ((p)->fts_symbol)
#define fts_word_get_object(p)         ((p)->fts_object)
#define fts_word_get_pointer(p)        ((p)->fts_pointer)
#define fts_word_get_string(p)         ((p)->fts_string)


/**
 * Set the atom to void
 * 
 * @fn fts_set_void( const fts_atom_t *p)
 * @param p pointer to the atom
 * @ingroup atom
 */
#define fts_set_void(p) ((p)->type = fts_void_class, fts_word_set_int( &(p)->value, 0))

/**
 * Set the integer value
 * 
 * @fn fts_set_int( const fts_atom_t *p, int v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_int(p, v) ((p)->type = fts_int_class, fts_word_set_int( &(p)->value, (v)))

/**
 * Set the float value
 * 
 * @fn fts_set_float( const fts_atom_t *p, float v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_float(p, v) ((p)->type = fts_float_class, fts_word_set_float( &(p)->value, (v)))

/**
 * Set the symbol value
 * 
 * @fn fts_set_symbol( const fts_atom_t *p, fts_symbol_t v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_symbol(p, v) ((p)->type = fts_symbol_class, fts_word_set_symbol( &(p)->value, (v)))

/**
 * Set the object value
 * 
 * @fn fts_set_object( const fts_atom_t *p, fts_object_t *v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_object(p, v) ((p)->type = fts_object_get_class((fts_object_t *)(v)), fts_word_set_object( &(p)->value, (fts_object_t *)(v)))

/**
 * Set the pointer value
 * 
 * @fn fts_set_pointer( const fts_atom_t *p, void *v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_pointer(p, v) ((p)->type = fts_pointer_class, fts_word_set_pointer( &(p)->value, (v)))

/**
 * Set the string value
 * 
 * @fn fts_set_string( const fts_atom_t *p, const char *v)
 * @param p pointer to the atom
 * @param v the value
 * @ingroup atom
 */
#define fts_set_string(p, v) ((p)->type = fts_string_class, fts_word_set_string( &(p)->value, (v)))

/**
 * Tests if atom is void
 * 
 * @fn int fts_is_void( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is void
 * @ingroup atom
 */
#define fts_is_void(p) ((p)->type == fts_void_class)

/**
 * Tests if atom contains an integer
 * 
 * @fn int fts_is_int( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is integer
 * @ingroup atom
 */
#define fts_is_int(p) ((p)->type == fts_int_class)

/**
 * Tests if atom contains a float
 * 
 * @fn int fts_is_float( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is float
 * @ingroup atom
 */
#define fts_is_float(p) ((p)->type == fts_float_class) 

/**
 * Tests if atom contains a number (int or float)
 * 
 * @fn int fts_is_number( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is number (int or float)
 * @ingroup atom
 */
#define fts_is_number(p) ((p)->type == fts_int_class || (p)->type == fts_float_class) 

/**
 * Tests if atom contains a symbol
 * 
 * @fn int fts_is_symbol( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is symbol
 * @ingroup atom
 */
#define fts_is_symbol(p) ((p)->type == fts_symbol_class)

/**
 * Tests if atom contains an object
 * 
 * @fn int fts_is_object( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is object
 * @ingroup atom
 */
#define fts_is_object(p) (!fts_class_is_primitive( (p)->type))

/**
 * Tests if atom contains a pointer
 * 
 * @fn int fts_is_pointer( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is pointer
 * @ingroup atom
 */
#define fts_is_pointer(p) ((p)->type == fts_pointer_class)

/**
 * Tests if atom contains a string
 * 
 * @fn int fts_is_string( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return 1 if atom type is string
 * @ingroup atom
 */
#define fts_is_string(p) ((p)->type == fts_string_class)

/**
 * Get the integer value
 * 
 * @fn int fts_get_int( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the integer value of the atom
 * @ingroup atom
 */
#define fts_get_int(p) fts_word_get_int( &(p)->value)

/**
 * Get the float value
 * 
 * @fn float fts_get_float( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the float value of the atom
 * @ingroup atom
 */
#define fts_get_float(p) fts_word_get_float( &(p)->value)

/**
 * Get the number value as integer
 * 
 * @fn int fts_get_number_int( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the integer value of the atom if atom is integer, the float value converted to an int
 * if atom is float
 * @ingroup atom
 */
#define fts_get_number_int(p) (fts_is_int(p) ? fts_get_int(p) : (int)fts_get_float(p))

/**
 * Get the number value as float
 * 
 * @fn float fts_get_number_float( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the float value of the atom if atom is float, the integer value converted to a float
 * if atom is integer
 * @ingroup atom
 */
#define fts_get_number_float(p) (fts_is_float(p) ? fts_get_float(p) : (float)fts_get_int(p))

/**
 * Get the symbol value
 * 
 * @fn fts_symbol_t fts_get_symbol( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the symbol value of the atom
 * @ingroup atom
 */
#define fts_get_symbol(p) fts_word_get_symbol( &(p)->value)

/**
 * Get the object value
 * 
 * @fn fts_object_t *fts_get_object( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the object value of the atom
 * @ingroup atom
 */
#define fts_get_object(p) fts_word_get_object( &(p)->value)

/**
 * Get the pointer value
 * 
 * @fn void *fts_get_pointer( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the pointer value of the atom
 * @ingroup atom
 */
#define fts_get_pointer(p) fts_word_get_pointer( &(p)->value)

/**
 * Get the string value
 * 
 * @fn const char *fts_get_string( const fts_atom_t *p)
 * @param p pointer to the atom
 * @return the string value of the atom
 * @ingroup atom
 */
#define fts_get_string(p) fts_word_get_string( &(p)->value)

/**
 * Increments object reference count if atom contains an object
 *
 * @fn void fts_atom_refer( const fts_atom_t *p)
 * @param p pointer to the atom
 * @ingroup atom
 */
#define fts_atom_refer(p) do {if(fts_is_object(p)) fts_object_refer(fts_get_object(p));} while(0)

/**
 * Decrements object reference count if atom contains an object
 *
 * @fn void fts_atom_release( const fts_atom_t *p)
 * @param p pointer to the atom
 * @ingroup atom
 */
#define fts_atom_release(p) do {if(fts_is_object(p)) fts_object_release(fts_get_object(p));} while(0)

/**
 * Assignment between atoms.<br> 
 * This macro takes care of dereferencing the destination if it was an
 * object and, if the source is an object, referencing it to account
 * for the new reference from dest.
 *
 * @fn void fts_atom_assign( fts_atom_t *dest, const fts_atom_t *src)
 * @param dest pointer to the destination atom
 * @param src pointer to the source atom
 * @ingroup atom
 */
#define fts_atom_assign(dest, src)		\
  do						\
    {						\
      fts_atom_release(dest);			\
      *(dest) = *(src);				\
      fts_atom_refer(src);			\
    }						\
  while(0)

/**
 * Making an atom void.<br>
 * This macro takes care of dereferencing the atom if it was an object.
 *
 * @fn void fts_atom_void( fts_atom_t *p)
 * @param dest pointer to the destination atom
 * @param src pointer to the source atom
 * @ingroup atom
 */
#define fts_atom_void(p)		        \
  do						\
    {						\
      fts_atom_release(p);			\
      fts_set_void(p);                          \
    }						\
  while(0)

/**
 * Checks if atoms are of the same type
 *
 * @fn int fts_atom_same_type( const fts_atom_t *p1, const fts_atom_t *p2))
 * @param p1 pointer to atom
 * @param p2 pointer to atom
 * @return 1 if atoms are of the same type, 0 if not
 * @ingroup atom
 */
#define fts_atom_same_type(p1, p2) ((p1)->type == (p2)->type)

/**
 * Checks if atoms are strictly equals (type and value)
 *
 * @param p1 pointer to atom
 * @param p2 pointer to atom
 * @return 1 if atoms are equals, 0 if not
 * @ingroup atom
 */
FTS_API int fts_atom_identical(const fts_atom_t *p1, const fts_atom_t *p2);

/**
 * Compares atoms of number, symbol, and objects (pointer!).
 * For objects the equals function of the class is used to compare the atoms.
 *
 * @param p1 pointer to atom
 * @param p2 pointer to atom
 * @return 1 if atoms are equal, 0 if not
 * @ingroup atom
 */
FTS_API int fts_atom_equals(const fts_atom_t *p1, const fts_atom_t *p2);

/**
 * Compares atoms of number, symbol, and objects (pointer!).
 * For objects???
 *
 * @param a pointer to atom
 * @param b pointer to atom
 * @return a value < 0, 0, > 0 if a < b, a == b, a > b respectively
 * @ingroup atom
 */
FTS_API int fts_atom_compare(const fts_atom_t *a, const fts_atom_t *b);

/**
 * Copies atom value or object (not just reference) from one atom to another.
 * For objects the copy function of the class is used.
 *
 * Precondition:  target atom has to be initialized (it will be free properly).
 *
 * Postcondition: target atom is properly referenced copy of source.
 *
 * @param from pointer to original atom
 * @param to   pointer to destination atom
 * @ingroup atom
 */
FTS_API void fts_atom_copy (const fts_atom_t *from, fts_atom_t *to);

/**
 * Constant representing a 'void' atom
 * @ingroup atom
 */
FTS_API fts_atom_t *fts_null;
FTS_API fts_atom_t *fts_nix;

