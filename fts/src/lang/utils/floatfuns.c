#include "fts.h"
#include "floatfuns.h"
#include <math.h>

static fts_hash_table_t the_fts_ffun_hashtable;

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

void
fts_ffuns_init(void)
{
  fts_hash_table_init(&the_fts_ffun_hashtable);

  fts_ffun_new(fts_new_symbol("sin"), sinf);
  fts_ffun_new(fts_new_symbol("cos"), cosf);
  fts_ffun_new(fts_new_symbol("tan"), tanf);
  fts_ffun_new(fts_new_symbol("asin"), asinf);
  fts_ffun_new(fts_new_symbol("acos"), acosf);
  fts_ffun_new(fts_new_symbol("atan"), atanf);
  fts_ffun_new(fts_new_symbol("exp"), expf);
  fts_ffun_new(fts_new_symbol("log"), logf);
  fts_ffun_new(fts_new_symbol("log10"), log10f);
}


/***************************************************************************************
 *
 *  float function tables
 *
 */

/* match by size, min and max in table list */
static fts_fftab_t *
fftab_match(fts_fftab_t *list, int size, float min, float max) 
{
  fts_fftab_t *fftab = list;
  float tolerance = 0.01 * (max - min) / size; /* one percent of the step */

  while(fftab)
    {
      if(fftab->size == size && fabs(fftab->min - min) < tolerance && fabs(fftab->max - max) < tolerance)
	break;
      
      fftab = fftab->next_in_list;
    }

  return fftab;
}

/* insert to table list (append to end) */
static void
fftab_insert(fts_ffun_t *ffun, fts_fftab_t *fftab) 
{
  if(ffun->tables)
    fftab->next_in_list = ffun->tables;

  ffun->tables = fftab;
}

/* remove from table list */
static void
fftab_remove(fts_ffun_t *ffun, fts_fftab_t *fftab)
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

/* new / delete */

static fts_fftab_t *
fftab_new(int size, float min, float max)
{
  fts_fftab_t *fftab;
  
  fftab = (fts_fftab_t *)fts_malloc(sizeof(fts_fftab_t));
  fftab->values = (float *)fts_malloc(sizeof(float) * (size + 1));

  if(min <= max)
    {
      fftab->min = min;
      fftab->max = max;
      fftab->range = max - min;
    }
  else
    {
      fftab->min = max;
      fftab->max = min;
      fftab->range = min - max;
    }
    
  fftab->size = size;
  fftab->next_in_list = 0;

  return fftab;
}

void
fftab_delete(fts_fftab_t *fftab)
{
  fts_free(fftab->values);
  fts_free(fftab);
}

/* get (or create) table for existing function */
fts_fftab_t *
fts_fftab_get(fts_symbol_t name, float min, float max, int size)
{
  fts_ffun_t *ffun = ffun_get_by_name(name);
    
  if(ffun)
    {
      fts_fftab_t *fftab = fftab_match(ffun->tables, size, min, max);

      if(!fftab)
	{
	  float step = (max - min) / size;
	  int i;

	  fftab = fftab_new(min, max, size);

	  for(i=0; i<=size; i++)
	    fftab->values[i] = ffun->function(min + (float)i * step);

	  fftab_insert(ffun, fftab);
	}

      fftab->ref++;
      
      return fftab;
    }
  else
    return 0;
}

void
fts_fftab_release(fts_symbol_t name, fts_fftab_t *fftab)
{
  fts_ffun_t *ffun = ffun_get_by_name(name);

  if(ffun)
    {
      if(--fftab->ref == 0)
	{
	  fftab_remove(ffun, fftab);
	  fftab_delete(fftab);
	}
    }
}

/***************************************************************************************
 *
 *  lookups of float function tables
 *
 */
