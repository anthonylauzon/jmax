/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "floatfuns.h"
#include <math.h>

static fts_hash_table_t the_fts_ffun_hashtable;

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
static fts_ffun_t *
ffun_get_by_name(fts_symbol_t name)
{
  fts_atom_t a;

  if(fts_hash_table_lookup(&the_fts_ffun_hashtable, name, &a))
    return fts_get_ptr(&a);
  else
    return 0;
}

/* insert to table list (append to end) */
static void
ffun_insert_tab(fts_ffun_t *ffun, fts_fftab_t *fftab) 
{
  if(ffun->tables)
    fftab->next_in_list = ffun->tables;

  ffun->tables = fftab;
}

/* remove from table list */
static void
ffun_remove_tab(fts_ffun_t *ffun, fts_fftab_t *fftab)
{
  if(ffun->tables == fftab)
    ffun->tables = fftab->next_in_list;
  else
    {
      fts_fftab_t *ptr = ffun->tables;
      
      while(ptr->next_in_list)
	{
	  if(ptr->next_in_list == fftab)
	    {
	      ptr->next_in_list = fftab->next_in_list;
	      break;
	    }
	  ptr = ptr->next_in_list;
	}
    }
}

int
fts_ffun_exists(fts_symbol_t name)
{
  fts_atom_t a;

  return (fts_hash_table_lookup(&the_fts_ffun_hashtable, name, &a));
}

int
fts_ffun_new(fts_symbol_t name, float (*function)(float))
{
  fts_atom_t a;
  fts_ffun_t *ffun = ffun_get_by_name(name);
  
  if(ffun)
    return (ffun->function == function);
  else
    {
      ffun = fts_malloc(sizeof(fts_ffun_t));
      ffun->function = function;
      ffun->tables = 0;
      
      fts_set_ptr(&a, ffun);
      return fts_hash_table_insert(&the_fts_ffun_hashtable, name, &a);
    }
}

float
fts_ffun_eval(fts_symbol_t name, float f)
{
  fts_ffun_t *ffun = ffun_get_by_name(name);
  
  return ffun->function(f);
}

void
fts_ffun_fill(fts_symbol_t name, float *out, int size, float min, float max)
{
  int i;
  fts_ffun_t *ffun = ffun_get_by_name(name);
  float step = (max - min) / size;

  if(ffun)
    {
      for(i=0; i<=size; i++)
	out[i] = ffun->function(min + (float)i * step);
    }
}

void
fts_ffun_apply(fts_symbol_t name, float* in, float* out, int size)
{
  int i;
  fts_ffun_t *ffun = ffun_get_by_name(name);

  if(ffun)
    {
      for(i=0; i<size; i++)
	out[i] = ffun->function(in[i]);
    }
}

fts_float_function_t
fts_ffun_get_ptr(fts_symbol_t name)
{
  fts_ffun_t *ffun = ffun_get_by_name(name);

  if(ffun)
    return ffun->function;
  else
    return 0;
}

void fts_ffuns_init(void)
{
  fts_hash_table_init(&the_fts_ffun_hashtable);

#ifdef SGI
  fts_ffun_new(fts_new_symbol("sin"), sinf);
  fts_ffun_new(fts_new_symbol("cos"), cosf);
  fts_ffun_new(fts_new_symbol("tan"), tanf);
  fts_ffun_new(fts_new_symbol("asin"), asinf);
  fts_ffun_new(fts_new_symbol("acos"), acosf);
  fts_ffun_new(fts_new_symbol("atan"), atanf);
  fts_ffun_new(fts_new_symbol("exp"), expf);
  fts_ffun_new(fts_new_symbol("log"), logf);
  fts_ffun_new(fts_new_symbol("log10"), log10f);
#endif
}


/***************************************************************************************
 *
 *  float function tables
 *
 */

/* match by size, min and max in table list */
static float *
fftab_match(fts_fftab_t *list, int size, float min, float max, float tolerance) 
{
  fts_fftab_t *fftab = list;
  float step = (max - min) / size;
  float margin = tolerance * step; /* one percent of the step */

  while(fftab)
    {
      float fftab_step = (fftab->max - fftab->min) / fftab->size;

      if(size <= fftab->size && 
         fabs(size * step - size * fftab_step) < margin &&
         min + 0.5 * margin >= fftab->min &&
         max - 0.5 * margin <= fftab->max)
	return fftab->values + (int)(fftab->min - min + 0.5 * margin);
      
      fftab = fftab->next_in_list;
    }

  return 0;
}

static void
fftab_fill(fts_fftab_t *fftab, fts_ffun_t *ffun, int size, float min, float max)
{
  float step = (max - min) / size;
  int i;
  
  for(i=0; i<=size; i++)
    fftab->values[i] = ffun->function(min + (float)i * step);      
}

/* new / delete */

static fts_fftab_t *
fftab_new(fts_ffun_t *ffun, int size, float min, float max)
{
  fts_fftab_t *fftab;
  
  fftab = (fts_fftab_t *)fts_malloc(sizeof(fts_fftab_t));
  fftab->values = (float *)fts_malloc(sizeof(float) * (size + 1));

  if(min < max)
    {
      fftab->min = min;
      fftab->max = max;
      fftab->scale = size / (max - min);
    }
  else if(max > min)
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

static void fftab_delete(fts_fftab_t *fftab)
{
  fts_free(fftab->values);
  fts_free(fftab);
}

/* get (or create) table for existing function */
float *
fts_fftab_get(fts_symbol_t name, int size, float min, float max)
{
  fts_ffun_t *ffun = ffun_get_by_name(name);
    
  if(ffun)
    {
      float *tab = fftab_match(ffun->tables, size, min, max, 0.01f);

      if(!tab)
	{
	  float step = (max - min) / size;
	  fts_fftab_t *fftab = fftab_new(ffun, size, min, max);

	  fftab_fill(fftab, ffun, size, min, max);
	  ffun_insert_tab(ffun, fftab);
	  tab = fftab->values;
	}

      return tab;
    }
  else
    return 0;
}

float *
fts_fftab_get_sine(int size)
{
  return fts_fftab_get(fts_new_symbol("sin"), (5 * size / 4), (0.0f), (5 * 6.2831853f / 4));
}

float *
fts_fftab_get_sine_first_half(int size)
{
  return fts_fftab_get_sine(2 * size);
}

float *
fts_fftab_get_sine_second_half(int size)
{
  float *tab = fts_fftab_get_sine(2 * size);

  if(tab)
    return tab + size;
  else
    return 0;
}

float *
fts_fftab_get_sine_first_quarter(int size)
{
  return fts_fftab_get_sine(4 * size);
}

float *
fts_fftab_get_sine_second_quarter(int size)
{
  float *tab = fts_fftab_get_sine(4 * size);

  if(tab)
    return tab + size;
  else
    return 0;
}

float *
fts_fftab_get_sine_third_quarter(int size)
{
  float *tab = fts_fftab_get_sine(4 * size);

  if(tab)
    return tab + 2 * size;
  else
    return 0;
}

float *
fts_fftab_get_sine_fourth_quarter(int size)
{
  float *tab = fts_fftab_get_sine(4 * size);

  if(tab)
    return tab + 3 * size;
  else
    return 0;
}

float *
fts_fftab_get_cosine(int size)
{
  float *tab = fts_fftab_get_sine(size);

  if(tab)
    return tab + (size / 4); /* return phase shifted sine as cosine */
  else
    return 0;
}

float *
fts_fftab_get_cosine_first_half(int size)
{
  return fts_fftab_get_cosine(2 * size);
}

float *
fts_fftab_get_cosine_second_half(int size)
{
  float *tab = fts_fftab_get_cosine(2 * size);

  if(tab)
    return tab + size;
  else
    return 0;
}
