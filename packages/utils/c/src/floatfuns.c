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


#include <fts/packages/utils/utils.h>
#include <math.h>
#include <assert.h>

static fts_hashtable_t the_fts_ffun_hashtable;

/* float function tables */
typedef struct _fts_fftab
{
  float *values;
  int size; /* table size must be a power of two (for fast lookups) */
  float min;
  float max;
  float scale; /* size. / (max - min) */
  struct _fts_fftab *next_in_list; /* for list of tables for one float function */
} fts_fftab_t;

/***************************************************************************************
 *
 *  float functions
 *
 */

typedef struct _fts_ffun
{
  float (*function) (float);
  fts_fftab_t *tables; /* list of lookup tables registered for the function */
} fts_ffun_t;

/* local */
static fts_ffun_t *ffun_get_by_name( fts_symbol_t name)
{
  fts_atom_t a, n;

  fts_set_symbol( &n, name);
  if ( fts_hashtable_get( &the_fts_ffun_hashtable, &n, &a))
    return fts_get_pointer( &a);
  else
    return 0;
}

/* insert to table list ( append to end) */
static void ffun_insert_tab( fts_ffun_t *ffun, fts_fftab_t *fftab) 
{
  if (ffun->tables)
    fftab->next_in_list = ffun->tables;

  ffun->tables = fftab;
}

/* remove from table list */
static void ffun_remove_tab( fts_ffun_t *ffun, fts_fftab_t *fftab)
{
  if (ffun->tables == fftab)
    ffun->tables = fftab->next_in_list;
  else
    {
      fts_fftab_t *ptr = ffun->tables;
      
      while ( ptr->next_in_list)
	{
	  if ( ptr->next_in_list == fftab)
	    {
	      ptr->next_in_list = fftab->next_in_list;
	      break;
	    }
	  ptr = ptr->next_in_list;
	}
    }
}

int fts_ffun_exists( fts_symbol_t name)
{
  fts_atom_t a;
  fts_atom_t n;

  fts_set_symbol( &n, name);
  return (fts_hashtable_get( &the_fts_ffun_hashtable, &n, &a));
}

void fts_ffun_new( fts_symbol_t name, float (*function)(float))
{
  fts_atom_t a, n;
  fts_ffun_t *ffun = ffun_get_by_name( name);
  
  if ( ffun)
    return;

  ffun = fts_malloc( sizeof( fts_ffun_t));
  ffun->function = function;
  ffun->tables = 0;
      
  fts_set_symbol( &n, name);
  fts_set_pointer( &a, ffun);
  fts_hashtable_put( &the_fts_ffun_hashtable, &n, &a);
}

float fts_ffun_eval( fts_symbol_t name, float f)
{
  fts_ffun_t *ffun = ffun_get_by_name( name);

  assert( ffun != 0);

  return ffun->function( f);
}

void fts_ffun_fill( fts_symbol_t name, float *out, int size, float min, float max)
{
  int i;
  fts_ffun_t *ffun = ffun_get_by_name( name);
  float step = (max - min) / size;

  assert( ffun != 0);

  if ( ffun)
    {
      for( i=0; i<=size; i++)
	out[i] = ffun->function( min + (float)i * step);
    }
}

void fts_ffun_apply( fts_symbol_t name, float* in, float* out, int size)
{
  int i;
  fts_ffun_t *ffun = ffun_get_by_name( name);

  assert( ffun != 0);

  if ( ffun)
    {
      for( i=0; i<size; i++)
	out[i] = ffun->function( in[i]);
    }
}

fts_float_function_t fts_ffun_get_ptr( fts_symbol_t name)
{
  fts_ffun_t *ffun = ffun_get_by_name( name);

  if ( ffun)
    return ffun->function;
  else
    return 0;
}

static float hanning( float f)
{
  return (0.5 - 0.5 * cos(f));
}

#ifndef HAVE_SINF
static float sinf( float f)
{
  return (float)sin( f);
}
#endif

#ifndef HAVE_COSF
static float cosf( float f)
{
  return (float)cos( f);
}
#endif

void
fts_ffuns_init( void)
{
  fts_hashtable_init( &the_fts_ffun_hashtable, FTS_HASHTABLE_SMALL);

  fts_ffun_new( fts_new_symbol( "sin"), sinf);
  fts_ffun_new( fts_new_symbol( "cos"), cosf);

#if HAVE_TANF
  fts_ffun_new( fts_new_symbol( "tan"), tanf);
#endif

#if HAVE_ASINF
  fts_ffun_new( fts_new_symbol( "asin"), asinf);
#endif

#if HAVE_ACOSF
  fts_ffun_new( fts_new_symbol( "acos"), acosf);
#endif

#if HAVE_ATANF
  fts_ffun_new( fts_new_symbol( "atan"), atanf);
#endif

#if HAVE_EXPF
  fts_ffun_new( fts_new_symbol( "exp"), expf);
#endif

#if HAVE_LOGF
  fts_ffun_new( fts_new_symbol( "log"), logf);
#endif

#if HAVE_LOG10F
  fts_ffun_new( fts_new_symbol( "log10"), log10f);
#endif

  fts_ffun_new( fts_new_symbol( "hanning"), hanning);
}


/***************************************************************************************
 *
 *  float function tables
 *
 */

/* match by size, min and max in table list */
static float *
fftab_match( fts_fftab_t *list, int size, float min, float max, float tolerance) 
{
  fts_fftab_t *fftab = list;
  float step = (max - min) / size;
  float margin = tolerance * step; /* one percent of the step */

  while (fftab)
    {
      float fftab_step = (fftab->max - fftab->min) / fftab->size;

      if (size <= fftab->size && 
         fabs( size * step - size * fftab_step) < margin &&
         min + 0.5 * margin >= fftab->min &&
         max - 0.5 * margin <= fftab->max)
	return fftab->values + (int)(fftab->min - min + 0.5 * margin);
      
      fftab = fftab->next_in_list;
    }

  return 0;
}

static void
fftab_fill( fts_fftab_t *fftab, fts_ffun_t *ffun, int size, float min, float max)
{
  float step = (max - min) / size;
  int i;
  
  for( i=0; i<=size; i++)
    fftab->values[i] = ffun->function( min + (float)i * step);      
}

/* new / delete */

static fts_fftab_t *
fftab_new( fts_ffun_t *ffun, int size, float min, float max)
{
  fts_fftab_t *fftab;
  
  fftab = (fts_fftab_t *)fts_malloc( sizeof(fts_fftab_t));
  fftab->values = (float *)fts_malloc( sizeof( float) * (size + 1));

  if (min < max)
    {
      fftab->min = min;
      fftab->max = max;
      fftab->scale = size / (max - min);
    }
  else if (max > min)
    {
      fftab->min = max;
      fftab->max = min;
      fftab->scale = size / (min - max);
    }
  else
    {
      fftab->min = fftab->max = min;
      fftab->scale = size;
    }
    
  fftab->size = size;
  fftab->next_in_list = 0;

  return fftab;
}

void
fftab_delete( fts_fftab_t *fftab)
{
  fts_free( fftab->values);
  fts_free( fftab);
}

/* get (or create) table for existing function */
float *
fts_fftab_get( fts_symbol_t name, int size, float min, float max)
{
  fts_ffun_t *ffun = ffun_get_by_name( name);
    
  assert( ffun != 0);

  if (ffun)
    {
      float *tab = fftab_match( ffun->tables, size, min, max, 0.01f);

      if (!tab)
	{
 	  fts_fftab_t *fftab = fftab_new( ffun, size, min, max);

	  fftab_fill( fftab, ffun, size, min, max);
	  ffun_insert_tab( ffun, fftab);
	  tab = fftab->values;
	}

      return tab;
    }
  else
    return 0;
}

float *
fts_fftab_get_sine( int size)
{
  return fts_fftab_get( fts_new_symbol( "sin"), (5 * size / 4), (0.0f), (5 * 6.2831853f / 4));
}

float *
fts_fftab_get_sine_first_half( int size)
{
  return fts_fftab_get_sine( 2 * size);
}

float *
fts_fftab_get_sine_second_half( int size)
{
  float *tab = fts_fftab_get_sine( 2 * size);

  if (tab)
    return tab + size;
  else
    return 0;
}

float *
fts_fftab_get_sine_first_quarter( int size)
{
  return fts_fftab_get_sine( 4 * size);
}

float *
fts_fftab_get_sine_second_quarter( int size)
{
  float *tab = fts_fftab_get_sine( 4 * size);

  if (tab)
    return tab + size;
  else
    return 0;
}

float *
fts_fftab_get_sine_third_quarter( int size)
{
  float *tab = fts_fftab_get_sine( 4 * size);

  if (tab)
    return tab + 2 * size;
  else
    return 0;
}

float *
fts_fftab_get_sine_fourth_quarter( int size)
{
  float *tab = fts_fftab_get_sine( 4 * size);

  if (tab)
    return tab + 3 * size;
  else
    return 0;
}

float *
fts_fftab_get_cosine( int size)
{
  float *tab = fts_fftab_get_sine( size);

  if ( tab)
    return tab + (size / 4); /* return phase shifted sine as cosine */
  else
    return 0;
}

float *
fts_fftab_get_cosine_first_half( int size)
{
  return fts_fftab_get_cosine( 2 * size);
}

float *
fts_fftab_get_cosine_second_half( int size)
{
  float *tab = fts_fftab_get_cosine( 2 * size);

  if (tab)
    return tab + size;
  else
    return 0;
}

float *
fts_fftab_get_hanning(int size)
{
  return fts_fftab_get(fts_new_symbol("hanning"), size, 0.0f, 6.2831853f);
}

