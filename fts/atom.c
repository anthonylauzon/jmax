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


void fts_kernel_atom_init (void);


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
          fts_equals_function_t equals = fts_class_get_equals_function(class_first);
        
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
	/* special case: numbers are comparable */
	return 1;
    else		
	/* return arbitrary class distance (is type_id better?) */
	return ((int) fts_get_class(a) - (int) fts_get_class(b));
}


int
fts_atom_compare (const fts_atom_t *a, const fts_atom_t *b)
{
    int res = fts_atom_compare_classes(a, b);

    if (res != 0)
	/* return arbitrary class distance */
	return res;
    else
    {    /* comparable classes */
	switch (fts_class_get_typeid(fts_get_class(a))) 
	{
	case FTS_TYPEID_VOID:
	    return (1);

	case FTS_TYPEID_INT:
	case FTS_TYPEID_FLOAT:
	    return (fts_get_number_float(a) - fts_get_number_float(b));

	case FTS_TYPEID_SYMBOL:
	    return (strcmp(fts_symbol_name(fts_get_symbol(a)),
			   fts_symbol_name(fts_get_symbol(b))));

	case FTS_TYPEID_POINTER:
	    return ((char *) fts_get_pointer(a) - (char *) fts_get_pointer(b));

	case FTS_TYPEID_STRING :
	    return (strcmp(fts_get_string(a), fts_get_string(b)));

	default:	/* TODO: call object comparison function */
	    return fts_get_object(a) - fts_get_object(b);
	}
    }
}


void
fts_atom_copy( const fts_atom_t *from, fts_atom_t *to)
{
  if(fts_is_object(from) && fts_is_object(to))
  {
    fts_object_t *obj_from = fts_get_object(from);
    fts_object_t *obj_to = fts_get_object(to);
    fts_class_t *class_from = fts_object_get_class(obj_from);
    fts_class_t *class_to = fts_object_get_class(obj_to);
    
    if(class_from == class_to)
    {
      fts_copy_function_t copy = fts_class_get_copy_function(class_from);
      
      if(copy != NULL)
        (*copy)(obj_from, obj_to);
    }
  }

  *to = *from;
}

/***********************************************************************
 *
 * Initialization
 *
 */

static fts_class_t void_class = 
{
  /* class object header */
  {
    0, /* class */
    {0,0,0}, /* flags */
    0, /* reference counter */
    0 /* patcher data */
  },
  /* class structure */
  0, /*  */
  FTS_TYPEID_VOID, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0, /*  */
  0 /*  */
};

fts_class_t *fts_void_class = &void_class;

static fts_class_t int_class = { 
  {
    0,
  {0,0,0},
    0,
    0
  },
  0,
  FTS_TYPEID_INT,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
fts_class_t *fts_int_class = &int_class;

static fts_class_t float_class = { 
  {
    0,
  {0,0,0},
    0,
    0
  },
  0,
  FTS_TYPEID_FLOAT,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
fts_class_t *fts_float_class = &float_class;

static fts_class_t symbol_class = { 
  {
    0,
  {0,0,0},
    0,
    0
  },
  0,
  FTS_TYPEID_SYMBOL,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
fts_class_t *fts_symbol_class = &symbol_class;

static fts_class_t pointer_class = { 
  {
    0,
  {0,0,0},
    0,
    0
  },
  0,
  FTS_TYPEID_POINTER,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
fts_class_t *fts_pointer_class = &pointer_class;

static fts_class_t string_class = { 
  {
    0,
  {0,0,0},
    0,
    0
  },
  0,
  FTS_TYPEID_STRING,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
fts_class_t *fts_string_class = &string_class;

void fts_kernel_atom_init( void)
{
  fts_class_set_name( &int_class, fts_s_int);
  fts_class_set_name( &float_class, fts_s_float);
  fts_class_set_name( &symbol_class, fts_s_symbol);
  fts_class_set_name( &pointer_class, fts_s_pointer);
  fts_class_set_name( &string_class, fts_s_string);

  fts_set_void( &__fts_null);
}
