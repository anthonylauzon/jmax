#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "runtime.h"	/* @@@@ what should we do ?? */

/*
 * An ftl_data_t object implementing a vector of integer.  Supported
 * by a corresponding Java class.  Used for the moment by the old
 * table object; for now, directly implement a number of operations on
 * integer vector used by the other objects.
 */


static fts_data_class_t *fts_intvec_data_class = 0;

struct fts_intvec
{
  fts_data_t dataobj;
  int *vec;
  int size;	
};

/* Remote call codes */

#define INTEGER_VECTOR_SET    1
#define INTEGER_VECTOR_UPDATE 2

/********************************************************************/
/*                                                                  */
/*            SET functions on integer vectors                      */
/*                                                                  */
/********************************************************************/

void
fts_intvec_set_value(fts_intvec_t *this, int n1, int n2)
{
  if (n1 < 0)
    n1 = 0;
  else if (n1 >= this->size)
    n1 = this->size-1;

  (this->vec)[n1] = n2;
}


void
fts_intvec_set_const(fts_intvec_t *this, int n)
{
  int i;

  for (i = 0; i < this->size; i++)
    this->vec[i] = n;
}


void
fts_intvec_set(fts_intvec_t *this, int onset, int ac, const fts_atom_t *at)
{
  int i;

  for (i = onset; i < ac && i < this->size; i++)
    if (fts_is_int(&at[i]))
      this->vec[i] = fts_get_int(&at[i]);
}


/* set the size of the vector */

void
fts_intvec_set_size(fts_intvec_t *this, int n)
{
  int i;

  if (n < 1)
    n = 1;

  if (n == this->size)
    return;

  this->vec = (int *)fts_realloc((void *)this->vec, n * sizeof(int));

  if (! this->vec)
    return;

  for (i = this->size; i < n ; i++)
    this->vec[i] = 0;

  this->size = n;
}


/********************************************************************/
/*                                                                  */
/*            GET functions on integer vectors                      */
/*                                                                  */
/********************************************************************/


/* Get the fts_intvec size */

int
fts_intvec_get_size(fts_intvec_t *this)
{
  return this->size;
}


int
fts_intvec_get_sum(fts_intvec_t *this, int min,  int max)
{
  int i;
  int sum = 0;
  int *l;
  
  if (min < 0)
    min = 0;

  if (max > this->size || max == -1)
    max = this->size;

  for (i = min; i < max; i++)
    sum += this->vec[i];

  return sum;
}


int
fts_intvec_get_value(fts_intvec_t *this, int idx)
{
  return (this->vec)[idx];
}


int
fts_intvec_get_min_value(fts_intvec_t *this)
{
  int i, min;

  min = this->vec[0];

  for (i = 1; i < this->size; i++)
    if (this->vec[i] < min)
      min = this->vec[i];

  return min;
}


int
fts_intvec_get_max_value(fts_intvec_t *this)
{
  int i, max;

  max = this->vec[0];

  for (i = 1; i < this->size; i++)
    if (this->vec[i] > max)
      max = this->vec[i];

  return max;
}


int
fts_intvec_get_quantile(fts_intvec_t *this, int n)
{
  int v, index, i;

  v = 0;
  for (i = 0; i < this->size; i++)
    v += this->vec[i];

  if (! v)
    return 0;

  index = ((n * v) >> 15) + 1;

  for (i = 0; i < this->size; i++)
    {
      index -= this->vec[i];
      if (index <= 0)
	break;
    }
  
  if (i >= this->size)
    i--;

  return i;
}

/* Assuming  that the vector contain a monotone function,
   compute the inverse of the function for value n;
   slow, coud use a binary search
*/

int
fts_intvec_get_inv(fts_intvec_t *this, int n)
{
  int i;

  for (i = 0; i < this->size; i++)
    if (this->vec[i] > n)
      return (i > 0) ? i - 1 : i;
  
  return this->size;
}

/********************************************************************/
/*                                                                  */
/*            NEW/FREE functions on integer vectors                 */
/*                                                                  */
/********************************************************************/

fts_intvec_t *fts_intvec_new(int size)
{
  fts_intvec_t *this = (fts_intvec_t *)fts_malloc(sizeof(fts_intvec_t));
  int i;

  this->vec = (int *) fts_malloc(size * sizeof(int));
  this->size = size;

  for (i = 0; i < size; i++)
    this->vec[i] = 0;

  fts_data_init((fts_data_t *) this, fts_intvec_data_class);

  return this;
}

void
fts_intvec_delete(fts_intvec_t *this)
{
  fts_data_delete((fts_data_t *) this);
  fts_free((void *)this->vec);
  fts_free((void *)this);
}


/********************************************************************/
/*                                                                  */
/*            BMAX Releated functions on integer vectors            */
/*                                                                  */
/********************************************************************/

/* This is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a vector !!*/

/* Save bmax */


void fts_intvec_save_bmax(fts_intvec_t *this, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for  (i = 0; i < this->size; i++)
    {
      fts_set_int(&av[ac], this->vec[i]);

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the values */

	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */

	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 values plus offset */

	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the values  */

	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the values */

      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */

      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the values left */
      
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the values  */

      fts_bmax_code_pop_args(f, ac);
    }
}


/********************************************************************/
/*                                                                  */
/*            FTS_DATA functions on integer vectors                 */
/*                                                                  */
/********************************************************************/

/*
 * The export function
 */


static void fts_intvec_export_fun(fts_data_t *d)
{
  fts_intvec_t *this = (fts_intvec_t *)d;
  int i;

  fts_data_export(d);

  fts_data_start_remote_call(d, INTEGER_VECTOR_SET, 0, 0);
  fts_client_mess_add_int(this->size);

  for (i = 0; i < this->size; i++)
    fts_client_mess_add_int(this->vec[i]);

  fts_data_end_remote_call();
}

/*
 * The remote set function.
 * No checks here, correctness by design.
 */

static void fts_intvec_remote_set( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_intvec_t *this = (fts_intvec_t *)d;
  int offset;
  int nvalues;
  int i;

  /*
   * Arguments: the offset
   *            the number of values
   *            the values
   */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i = 0; i < nvalues; i++)
    this->vec[i + offset] = fts_get_int(&at[i + 2]);
}


static void fts_intvec_remote_update( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_intvec_t *this = (fts_intvec_t *)d;
  int i;

  fts_data_start_remote_call(d, INTEGER_VECTOR_SET, 0, 0);
  fts_client_mess_add_int(this->size);

  for (i = 0; i < this->size; i++)
    fts_client_mess_add_int(this->vec[i]);

  fts_data_end_remote_call();
}

/********************************************************************/
/*                                                                  */
/*            INIT_DATA functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void fts_intvec_config(void)
{
  fts_intvec_data_class = fts_data_class_new( fts_new_symbol( "intvec_data"));
  fts_data_class_define_export_function(fts_intvec_data_class, fts_intvec_export_fun);
  fts_data_class_define_function(fts_intvec_data_class, INTEGER_VECTOR_SET, fts_intvec_remote_set);
  fts_data_class_define_function(fts_intvec_data_class, INTEGER_VECTOR_UPDATE, fts_intvec_remote_update);
}







