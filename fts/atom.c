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


#include <string.h>
#include <assert.h>
#include <math.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/message.h>

static fts_atom_t __fts_null;
const fts_atom_t *fts_null = &__fts_null;

static fts_atom_t __fts_nix;
fts_atom_t *fts_nix = &__fts_nix;

void fts_kernel_atom_init(void);

int fts_atom_identical( const fts_atom_t *first, const fts_atom_t *second)
{
  if ( !fts_atom_same_type( first, second))
    return 0;

  switch( fts_class_get_typeid( fts_get_class( first)) ) {
  case FTS_TYPEID_VOID:
    return fts_is_void( second);
  case FTS_TYPEID_INT:
    return fts_get_int( first) == fts_get_int( second);
  case FTS_TYPEID_FLOAT:
    return fts_get_float( first) == fts_get_float( second);
  case FTS_TYPEID_SYMBOL:
    return fts_get_symbol( first) == fts_get_symbol( second);
  case FTS_TYPEID_POINTER:
    return fts_get_pointer( first) == fts_get_pointer( second);
  case FTS_TYPEID_STRING :
    return ! strcmp( fts_get_string( first), fts_get_string( second));
  default:
    return fts_get_object( first) == fts_get_object( second);
  }

  return 0;
}

int
fts_atom_equals( const fts_atom_t *first, const fts_atom_t *second)
{
  if(fts_is_int(first))
  {
    if(fts_is_int(second))
      return fts_get_int( first) == fts_get_int( second);
    else if(fts_is_float(second))
      return (double)fts_get_int( first) == fts_get_float( second);
  }
  else if(fts_is_float(first) && fts_is_number(second))
    return fabs(fts_get_float( first) - fts_get_number_float( second)) < 1.0e-7;
  else if (fts_atom_same_type( first, second))
  {
    if ( fts_is_symbol( first))
      return fts_get_symbol( first) == fts_get_symbol( second);
    else if ( fts_is_object( first))
    {
      fts_object_t *obj_first = fts_get_object( first);
      fts_object_t *obj_second = fts_get_object( second);
      
      if(obj_first == obj_second)
        return 1;
      else
      {
        fts_class_t *class_first = fts_object_get_class(obj_first);
        fts_class_t *class_second = fts_object_get_class(obj_second);
        
        if(class_first == class_second)
        {
          fts_class_equals_function_t equals = fts_class_get_equals_function(class_first);
        
          return (*equals)(obj_first, obj_second);
        }
      }
    }
  }
  
  return 0;
}


static int
fts_atom_compare_classes (const fts_atom_t *a, const fts_atom_t *b)
{
    if (fts_is_number(a)  &&  fts_is_number(b))
        /* special case: numbers are comparable -> distance zero */
        return 0;
    else                
        /* return arbitrary class distance (is typeid better?) */
        return (int) fts_get_class(a) - (int) fts_get_class(b);
}


int
fts_atom_compare (const fts_atom_t *a, const fts_atom_t *b)
{
    int res = fts_atom_compare_classes(a, b);

    if (res != 0)
        /* different classes, return arbitrary class distance */
        return res;

    /* comparable classes */
    switch (fts_class_get_typeid(fts_get_class(a))) 
    {
    case FTS_TYPEID_VOID:
        return 0;             /* two voids are always equal */

    case FTS_TYPEID_INT:
    case FTS_TYPEID_FLOAT:
        return fts_get_number_float(a) > fts_get_number_float(b)  ?   1  :
               fts_get_number_float(a) < fts_get_number_float(b)  ?  -1  :  0;

    case FTS_TYPEID_SYMBOL:
        return strcmp(fts_symbol_name(fts_get_symbol(a)),
                      fts_symbol_name(fts_get_symbol(b)));

    case FTS_TYPEID_STRING :
        return strcmp(fts_get_string(a), fts_get_string(b));

    case FTS_TYPEID_POINTER:
        return (int) fts_get_pointer(a) - (int) fts_get_pointer(b);

    default:    /* for objects, return arbitrary pointer distance.
		   TODO: call object comparison function */
        return (int) fts_get_object(a) - (int) fts_get_object(b);
    }
}


/* copy atom value or object (not just reference) from one atom to another 
   (target atom has to be initialized) */
void
fts_atom_copy (const fts_atom_t *from_atom, fts_atom_t *to_atom)
{
  fts_atom_release(to_atom); /* free if target contained an object */
  
  if (fts_is_object(from_atom))
  {
    fts_object_t             *from_obj   = fts_get_object(from_atom);
    fts_class_t              *from_class = fts_object_get_class(from_obj);
    fts_object_t             *to_obj     = fts_object_create(from_class, 0, 0);
    fts_class_copy_function_t copyfunc   = fts_class_get_copy_function(from_class);
    
    if (copyfunc != NULL)
      (*copyfunc)(from_obj, to_obj);
    /* else: class can not be copied: leave empty object */
    /*       alternative: fts_set_void(to); ? */
    
    fts_object_refer(to_obj);  /* lock new object */
    fts_set_object(to_atom, to_obj);
  }
  else /* binary atom copy for non-objects */
    *to_atom = *from_atom;
}




/***********************************************************************
 *
 * Initialization
 *
 */

#define FTS_PRIMITIVE_CLASS(VAR, TYPEID)                        \
static fts_class_t VAR =                                        \
{                                                               \
  /* class object header */                                     \
  {                                                             \
    0,         /* class */                                      \
    {0, 0, 0}, /* flags */                                      \
    0,         /* reference counter */                          \
    0          /* patcher data */                               \
  },                                                            \
  /* class structure */                                         \
  0, /* name */                                                 \
  TYPEID, /* type_id */                                         \
  0, /* super class */						\
  0, /* hash_function */                                        \
  0, /* equals_function */                                      \
  0, /* description_function */                                 \
  0, /* copy_function */                                        \
  0, /* array_function */                                       \
  { 0, 0, 0, 0 }, /* fts_hashtable_t import_handlers */		\
  { 0, 0, 0, 0 }, /* fts_hashtable_t export_handlers */         \
  0, /* instantiate_fun */                                      \
  0, /* constructor */                                          \
  0, /* deconstructor */                                        \
  0, /* package */                                              \
  0, /* methods */                                              \
  0, /* ninlets */                                              \
  0, /* input_handler */                                        \
  0, /* noutlets */                                             \
  0, /* out_alloc */                                            \
  0, /* outlets */                                              \
  0, /* size */                                                 \
  0, /* heap */                                                 \
  0, /* doc */                                                  \
};                                                              \
fts_class_t *fts_##VAR = &VAR;

FTS_PRIMITIVE_CLASS(void_class,    FTS_TYPEID_VOID)
FTS_PRIMITIVE_CLASS(int_class,     FTS_TYPEID_INT)
FTS_PRIMITIVE_CLASS(float_class,   FTS_TYPEID_FLOAT)
FTS_PRIMITIVE_CLASS(symbol_class,  FTS_TYPEID_SYMBOL)
FTS_PRIMITIVE_CLASS(pointer_class, FTS_TYPEID_POINTER)
FTS_PRIMITIVE_CLASS(string_class,  FTS_TYPEID_STRING)


void fts_kernel_atom_init( void)
{
  fts_class_set_name( &int_class, fts_s_int);
  fts_class_set_name( &float_class, fts_s_float);
  fts_class_set_name( &symbol_class, fts_s_symbol);
  fts_class_set_name( &pointer_class, fts_s_pointer);
  fts_class_set_name( &string_class, fts_s_string);

  fts_set_void( &__fts_null);
  fts_set_void( &__fts_nix);
}
