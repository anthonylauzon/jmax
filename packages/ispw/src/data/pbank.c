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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

#include "fts.h"

static fts_hash_table_t pbank_data_table;

#define DEFAULT_N_COLS 8
#define DEFAULT_N_ROWS 64

/******************************************************************
 *
 *  pbank data
 *
 */

typedef struct
{
  fts_symbol_t name; /* data name */ 
  fts_atom_t **matrix; /* matrix  pointer */
  fts_atom_t *buffer; /* one row of buffer */
  int set; /* flag 1, if buffer set and not yet stored to matrix */
  int m; /* memory dimension */
  int n; /* memory dimension */
  int refcount; /* number of objects pointing to "matrix" */
} pbank_data_t;

/* Two functions:

   1- pbank_data_get, return the pdata with the right name and size,
      if it exists, otherwise create a new one, and return the pdata.
      refcount the pbank_data object.

   2- pbank_data_release, dereference the refcount, and if zero free the
      structure

   pbank_data can be unnamed, in such case they are not put in the table.
*/


/* read and write pbank files */

static int 
pbank_data_read_file(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_file_t *f;
  int ret;
  fts_atom_t a, *av;
  char c;
  int j, i;

  f = fts_atom_file_open(fts_symbol_name(file_name), "r");

  if(!f)
    {
      post("pbank: can't open file to read: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* read the pbank keyword */

  ret = fts_atom_file_read(f, &a, &c);

  if ((! ret) || (! fts_is_symbol(&a)) || (fts_get_symbol(&a) != fts_new_symbol("pbank")))
    {
      post("pbank: file hasn't pbank format: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* skip the column, row and comma in the file (we rebuild the matrix size
     from the data itself, and we trust the method) */
  fts_atom_file_read(f, &a, &c);
  fts_atom_file_read(f, &a, &c);
  fts_atom_file_read(f, &a, &c);

  /* note: the above line is intentionally repeated three times */
  i = 0;
  j = 0;

  av = data->matrix[i];

  while(fts_atom_file_read(f, &a, &c))
    {
      if (fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
	{
	  i++;
	  j = 0;

	  /* in case of row overflow, break and skip the other data */
	  if (i >= data->m)
	    break;

	  av = data->matrix[i];
	}
      else if (j < data->n)
	{
	  av[j] = a;
	  j++;
	}
    }

  fts_atom_file_close(f);
  return i;
}

static int 
pbank_data_write_file(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_t  a, *ap;
  fts_atom_file_t *f;
  int i, j;

  f = fts_atom_file_open(fts_symbol_name(file_name), "w");
  if(!f)
    {
      post("pbank: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* first, write the header: a line with "pbank <m> <n>" where m and n are ints */
  fts_set_symbol(&a, fts_new_symbol("pbank"));
  fts_atom_file_write(f, &a, ' ');

  fts_set_int(&a, data->n);
  fts_atom_file_write(f, &a, ' ');

  fts_set_int(&a, data->m);
  fts_atom_file_write(f, &a, ' ');

  fts_set_symbol(&a, fts_new_symbol(","));
  fts_atom_file_write(f, &a, '\n');

  /* write the content of the matrix */
  for(i=0; i<data->m; i++)     
    {
      ap = data->matrix[i];

      for (j=0; j<data->n; j++, ap++)	
	fts_atom_file_write(f, ap, ' ');

      fts_set_symbol(&a, fts_new_symbol(","));
      fts_atom_file_write(f, &a, '\n');

    }

  fts_atom_file_close(f);

  return(1);
}

static int 
pbank_data_export_ascii(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_t  a, *ap;
  fts_atom_file_t *f;
  int i, j;

  f = fts_atom_file_open(fts_symbol_name(file_name), "w");
  if(!f)
    {
      post("pbank: can't open file to write: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* write the content of the matrix */
  for(i=0; i<data->m; i++)     
    {
      ap = data->matrix[i];

      for(j=0; j<data->n; j++, ap++)	
	fts_atom_file_write(f, ap, ' ');

      fts_set_symbol(&a, fts_new_symbol(""));
      fts_atom_file_write(f, &a, '\n');
    }

  fts_atom_file_close(f);

  return(1);
}

/* allocate/free pbank data */

static pbank_data_t *
pbank_data_get(fts_symbol_t name, int n, int m)
{
  fts_atom_t atom;

  if(n < 1)
    n = DEFAULT_N_COLS;

  if(m < 1)
    m = DEFAULT_N_ROWS;

  if (name && fts_hash_table_lookup(&pbank_data_table, name, &atom))
    {
      /* data found by name , check its dimension and reference it */
      pbank_data_t *data = (pbank_data_t *) fts_get_ptr(&atom);

      if(data->n != n || data->m != m)
	{
	  post("pbank: %s %d %d: dimensions don't match\n", fts_symbol_name(name), data->n, data->m);
	  return 0;
	}
      else
	{
	  /* add reference to data */
	  data->refcount++;
	  return data;
	}
    }
  else
    {
      /* create new data */
      pbank_data_t *data;
      int i, j;

      data = (pbank_data_t *) fts_malloc(sizeof(pbank_data_t));
      data->refcount = 1;
      data->m = m;
      data->n = n;

      /* allocate the matrix */
      data->matrix = (fts_atom_t **) fts_malloc(sizeof(fts_atom_t *) * m);
      for(i=0; i<m; i++) 
	data->matrix[i] = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * n);

      /* zero matrix */
      for(i=0; i<m; i++)	
	{
	  fts_atom_t *row;

	  row = data->matrix[i];
	  for(j=0; j<n; j++)
	    fts_set_int(row + j, 0);
	}

      /* allocate preset buffer */
      data->buffer = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * n);

      /* zero buffer */
      for (j=0; j<n; j++)
	fts_set_int(data->buffer + j, 0);

       if(name)
	{
	  data->name = name;

	  /* record in name table */
	  fts_set_ptr(&atom, data);
	  fts_hash_table_insert(&pbank_data_table, name, &atom);

	  /* read in data from file (at least try it) */
	  pbank_data_read_file(data, name);
	}
       else
	 data->name = 0;

      return data;
    }
}

static void
pbank_data_release(pbank_data_t *data)
{
  /* dereference it */
  data->refcount--;
  
  if(!data->refcount)
    {
      int i;
      
      /* no more referenced, take it away from the table if named and free the memory */
      if(data->name)
	fts_hash_table_remove(&pbank_data_table, data->name);

      for(i=0; i<data->m; i++) 
	fts_free((void *)data->matrix[i]);

      fts_free((void *)data->buffer);
      fts_free((void *)data->matrix);
    }
}


/******************************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t ob;	 
  pbank_data_t *data; /* the pbank_data_t object including the param matrix */
  fts_atom_t *out_list; /* list for output on int method */
  fts_symbol_t *receives; /* used for back door messaging */
} pbank_t;

static void
pbank_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int n = fts_get_int_arg(ac, at, 1, 0);
  int m = fts_get_int_arg(ac, at, 2, 0);
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 3, 0);
  fts_symbol_t receive = fts_get_symbol_arg(ac, at, 4, 0);
  
  if (n <= 0)
    {
      n = DEFAULT_N_COLS;
      post("pbank: # of columns argument out of range, setting to %d\n", n);
    }
       
  if (m <= 0)
    {
      m = DEFAULT_N_ROWS;
      post("pbank: # of rows argument out of range, setting to %d\n",m);
    }

  /* name skip arg is "" */
  if(name == fts_new_symbol("\"\""))
    name = 0;
	
  if(receive)	
    {
      int j;

      /* create array of receive names */
      this->receives = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t ) * n);

      /* compose receive names for */
      for(j=0; j<n; j++) 
	{
	  char buf[256];

	  sprintf(buf, "%d-%s", j, fts_symbol_name(receive));
	  this->receives[j] = fts_new_symbol_copy(buf);
	}
    }

  /* tempory buffer for list output */
  this->out_list = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * n);

  /* the matrix object */
  this->data = pbank_data_get(name, n, m);
}

static void
pbank_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  if(!this->data)
    return;

  pbank_data_release(this->data);

  if(this->receives)
    fts_free((void *)this->receives);

  fts_free((void *)this->out_list);
}

/******************************************************************
 *
 *  user methods
 *
 */

static void
pbank_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int j = fts_get_int_arg(ac, at, 0, 0);
  int i = fts_get_int_arg(ac, at, 1, 0);
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *p;
  int k;

  /* skip index of row and col */
  ac -= 2;
  at += 2;

  /* force column to matrix limits */
  if(j < 0)
    j = 0;
  else if(j >= n)
    j = n-1;

  if(j + ac > n)
    ac = n - j;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if (i >= m)
    i = m - 1;

  /* copy to matrix */
  p = this->data->matrix[i] + j;
  for(k=0; k<ac; k++)
    p[k] = at[k];
}

static void
pbank_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int j = fts_get_int_arg(ac, at, 0, 0);
  int n = this->data->n;
  int k;

  /* skip first argument (index of col) */
  ac--; at++;

  /* force column to matrix limits */
  if (j < 0)
    j = 0;
  else if (j >= n)
    j = n - 1;

  if(j + ac > n)
    ac = n - j;

  /* copy list to buffer */
  for(k=0; k<ac; k++)
    this->data->buffer[k] = at[k];
}

static void
pbank_get_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i = fts_get_int(at);
  int j;
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *row;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if(i >= m)
    i = m - 1;

  row = this->data->matrix[i];
      
  /* copy row to buffer and output */
  for(j=0; j<n; j++)
    {
      this->data->buffer[j] = row[j];
      this->out_list[j] = row[j];
    }
  
  fts_outlet_list(o, 0, n, this->out_list);
}

static void
pbank_get_row_to_receives(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i = fts_get_int(at);
  int j;
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *row;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if(i >= m)
    i = m - 1;

  row = this->data->matrix[i];
      
  /* copy matrix row to buffer and send to receives */
  for(j=n-1; j>=0; j--)
    {
      fts_atom_t *atom = row + j;

      this->data->buffer[j] = *atom;
      ispw_send_message_to_receives(this->receives[j], fts_get_selector(atom), 1, atom);
    }
}

/* outputs row as single elements lead by their col index and copies row to buffer */
static void
pbank_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i = fts_get_int_arg(ac, at, 0, 0);
  int j;
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *row;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if(i >= m)
    i = m - 1;

  row = this->data->matrix[i];

  for(j=0; j<n; j++)
    {
      fts_atom_t out_list[3];

      /* copy matrix element to buffer */
      this->data->buffer[j] = row[j];

      /* output matrix element lead by column */
      fts_set_int(out_list + 0, j);
      
      if(fts_is_symbol(row + j))
	{
	  /* buaaaarrrrrrrgg zaaaaaaaaaaaaack!!!!!!!*&(*@#&^(*^&*(%#$ */
	  fts_set_symbol((out_list + 1), fts_s_symbol);
	  out_list[2] = row[j];
	  fts_outlet_list(o, 0, 3, out_list);
	}
      else 
	{
	  out_list[1] = row[j];
	  fts_outlet_list(o, 0, 2, out_list);
	}
    }
}

/* outputs row as single elements lead by their col index and copies row to buffer */
static void
pbank_recall_to_receives(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i = fts_get_int_arg(ac, at, 0, 0);
  int j;
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *row;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if(i >= m)
    i = m - 1;

  row = this->data->matrix[i];

  /* copy matrix row to buffer and send to receives */
  for(j=n-1; j>=0; j--)
    {
      fts_atom_t *atom = row + j;

      this->data->buffer[j] = *atom;
      ispw_send_message_to_receives(this->receives[j], fts_get_selector(atom), 1, atom);
    }
}

/* copies buffer to matrix row */
static void
pbank_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i = fts_get_int_arg(ac, at, 0, 0);
  int j;
  int n = this->data->n;
  int m = this->data->m;
  fts_atom_t *row;

  /* force row to matrix limits */
  if (i < 0)
    i = 0;
  else if (i >= m)
    i = m - 1;

  row = this->data->matrix[i];

  /* copy buffer to matrix row */
  for(j=0; j<n; j++)
    row[j] = this->data->buffer[j];
}

/* store or recall single element to/from matrix */
static void
pbank_set_and_get(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_atom_t *atom;
  int j = fts_get_int_arg(ac, at, 0, 0);
  int i = fts_get_int_arg(ac, at, 1, 0);
  int n = this->data->n;
  int m = this->data->m;

  /* force column to matrix limits */
  if(j < 0)
    j = 0;
  else if (j >= n)
    j = n - 1;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if (i >= m)
    i = m - 1;

  atom = this->data->matrix[i] + j;

  if(ac == 2) 
    {
      fts_atom_t out_list[3]; 

      /* output matrix element lead by column */
      fts_set_int(out_list, j);
      
      if (fts_is_symbol(atom)) 
	{
	  /* ones more: buaaaarrrrrrrgg zaaaaaaaaaaaaack!!!!!!!*&(*@#&^(*^&*(%#$ */
	  fts_set_symbol(out_list + 1, fts_s_symbol);
	  out_list[2] = *atom;
	  fts_outlet_list(o, 0, 3, out_list);
	}
      else 
	{
	  out_list[1] = *atom;
	  fts_outlet_list(o, 0, 2, out_list);
	}
    }
  else
    {
      /* write atom to matrix */
      *atom = at[2];
    }
}

/* store or recall single element to/from matrix */
static void
pbank_set_and_get_to_receives(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_atom_t *atom;
  int j = fts_get_int_arg(ac, at, 0, 0);
  int i = fts_get_int_arg(ac, at, 1, 0);
  int n = this->data->n;
  int m = this->data->m;

  /* force column to matrix limits */
  if(j < 0)
    j = 0;
  else if (j >= n)
    j = n - 1;

  /* force row to matrix limits */
  if(i < 0)
    i = 0;
  else if (i >= m)
    i = m - 1;

  atom = this->data->matrix[i] + j;

  if(ac == 2) 
    /* read atom from matrix and send to receives */
    ispw_send_message_to_receives(this->receives[j], fts_get_selector(atom), 1, atom);
  else
    {
      /* write atom to matrix */
      *atom = at[2];
    }
}

static void
pbank_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(file_name)
    pbank_data_read_file(this->data, file_name);
}

static void
pbank_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
 
  if(file_name)
    pbank_data_write_file(this->data, file_name);
}

static void
pbank_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  
  if(file_name)
    pbank_data_export_ascii(this->data, file_name);
}

/******************************************************************
 *
 *  class
 *
 */

static fts_status_t
pbank_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  if(ac < 5)
    {
      /* pbank <# of cols> <#of rows> <name> */
      fts_class_init(cl, sizeof(pbank_t), 1, 1, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, pbank_set_and_get);

      a[0] = fts_t_int;
      fts_method_define(cl, 0, fts_s_int, pbank_get_row, 1, a);
      fts_method_define(cl, 0, fts_new_symbol("recall"), pbank_recall, 1, a);

      /* type the outlet */
      fts_outlet_type_define_varargs(cl, 0, fts_s_list);
    }
  else if(ac == 5)
    {
      /* pbank <# of cols> <#of rows> <name> <root name for receives> ... send output to receives */
      fts_class_init(cl, sizeof(pbank_t), 1, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, pbank_set_and_get_to_receives);

      a[0] = fts_t_int;
      fts_method_define(cl, 0, fts_s_int, pbank_get_row_to_receives, 1, a);
      fts_method_define(cl, 0, fts_new_symbol("recall"), pbank_recall_to_receives, 1, a);
    }
  else
    return &fts_CannotInstantiate;

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pbank_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, pbank_delete);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), pbank_set);
  fts_method_define_varargs(cl, 0, fts_new_symbol("put"), pbank_put);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("store"), pbank_store, 1, a);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("write"), pbank_write, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("read"), pbank_read, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("export"), pbank_export, 2, a);

  return fts_Success;
}

void
pbank_config(void)
{
  fts_hash_table_init(&pbank_data_table);

  fts_metaclass_install(fts_new_symbol("pbank"), pbank_instantiate, fts_narg_equiv);
}
