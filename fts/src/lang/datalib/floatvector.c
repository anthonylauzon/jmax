#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "runtime.h"	/* @@@@ what should we do ?? */

fts_symbol_t fts_s_float_vector = 0;
static fts_data_class_t *fts_float_vector_data_class = 0;

/* remote call codes */

#define FLOAT_VECTOR_SET    1
#define FLOAT_VECTOR_UPDATE 2

/* local */

static void
floatvec_set_size(fts_float_vector_t *vector, int size)
{
  if(size > vector->alloc)
    {
      if(vector->alloc)
	vector->values = (float *)fts_realloc((void *)vector->values, sizeof(float) * size);
      else
	vector->values = (float *)fts_malloc(sizeof(float) * size);

      vector->alloc = size;
    }

  vector->size = size;
}

/* new/delete */

fts_float_vector_t *
fts_float_vector_new(int size)
{
  fts_float_vector_t *vector;
  int i;

  vector = (fts_float_vector_t *)fts_malloc(sizeof(fts_float_vector_t));

  if(size > 0)
    {
      vector->values = (float *) fts_malloc(size * sizeof(float));
      vector->size = size;
      fts_float_vector_zero(vector);
    }
  else
    {
      vector->values = 0;
      vector->size = 0;
    }

  vector->alloc = size;

  fts_data_init((fts_data_t *) vector, fts_float_vector_data_class);

  return vector;
}

void
fts_float_vector_delete(fts_float_vector_t *vector)
{
  fts_data_delete((fts_data_t *) vector);
  fts_free(vector->values);
  fts_free(vector);
}

/* copy & zero */

void
fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out)
{
  floatvec_set_size(out, in->size);
  fts_vec_fcpy(in->values, out->values, in->size);
}

void
fts_float_vector_zero(fts_float_vector_t *vector)
{
  fts_vec_fzero(vector->values, vector->size);  
}

void
fts_float_vector_set_size(fts_float_vector_t *vector, int size)
{
  int old_size = vector->size;
  int tail = size - old_size;

  floatvec_set_size(vector, size);

  if(tail > 0)
    fts_vec_fzero(vector->values + old_size, tail);
}

void 
fts_float_vector_set(fts_float_vector_t *vector, float *ptr, int size)
{
  floatvec_set_size(vector, size);
  fts_vec_fcpy(ptr, vector->values, size);
}

void
fts_float_vector_set_from_atom_list(fts_float_vector_t *vector, int offset, int ac, const fts_atom_t *at)
{
  int i;

  for (i=offset; i<ac && i<vector->size; i++)
    if (fts_is_float(&at[i]))
      vector->values[i] = fts_get_float(&at[i]);
}


/* sum, min, max */

float
fts_float_vector_get_sum(fts_float_vector_t *vector)
{
  float sum = 0;
  int i;

  for(i=0; i<vector->size; i++)
    sum += vector->values[i];

  return sum;
}

float
fts_float_vector_get_sub_sum(fts_float_vector_t *vector, int from, int to)
{
  float sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vector->size)
    to = vector->size - 1;

  for(i=from; i<=to; i++)
    sum += vector->values[i];

  return sum;
}

float
fts_float_vector_get_min_value(fts_float_vector_t *vector)
{
  float min;
  int i;

  min = vector->values[0];

  for (i=1; i<vector->size; i++)
    if (vector->values[i] < min)
      min = vector->values[i];

  return min;
}


float
fts_float_vector_get_max_value(fts_float_vector_t *vector)
{
  float max;
  int i;

  max = vector->values[0];

  for (i=1; i<vector->size; i++)
    if (vector->values[i] > max)
      max = vector->values[i];

  return max;
}

/********************************************************************
 *
 *   FTS data functions
 *
 */

static void fts_float_vector_export_fun(fts_data_t *data)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i = 0; i < vector->size; i++)
    fts_client_mess_add_float(vector->values[i]);

  fts_data_end_remote_call();
}

static void 
fts_float_vector_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int offset;
  int nvalues;
  int i;

  /* arguments: offset, # of values, values */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i=0; i<nvalues; i++)
    vector->values[i + offset] = fts_get_float(&at[i + 2]);
}


static void 
fts_float_vector_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_float(vector->values[i]);

  fts_data_end_remote_call();
}

void fts_float_vector_config(void)
{
  fts_s_float_vector = fts_new_symbol("float_vector");
  fts_float_vector_data_class = fts_data_class_new(fts_s_float_vector);

  fts_data_class_define_export_function(fts_float_vector_data_class, fts_float_vector_export_fun);
  fts_data_class_define_function(fts_float_vector_data_class, FLOAT_VECTOR_SET, fts_float_vector_remote_set);
  fts_data_class_define_function(fts_float_vector_data_class, FLOAT_VECTOR_UPDATE, fts_float_vector_remote_update);
}
