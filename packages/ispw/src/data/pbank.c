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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* pbank - zack settel 1994  */
/* Heavily modified (50% reimplemented) and ported to the new FTS by MDC */
/* ... ein bisschen Senf dazu von Nos */

#include "fts.h"

static fts_hash_table_t pbank_data_table;

#define DEFAULT_N_COLS 128
#define DEFAULT_N_ROWS 10

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
  int n_rows; /* memory dimension */
  int n_cols; /* memory dimension */
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

static int pbank_read_file(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_file_t *f;
  int ret;
  fts_atom_t a, *av;
  char c;
  int i_col, i_row;

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

  i_row = 0;
  i_col = 0;

  av = data->matrix[i_row];

  while (fts_atom_file_read(f, &a, &c))
    {
      if (fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
	{
	  i_row++;
	  i_col = 0;

	  /* in case of row overflow, break and skip the other data */

	  if (i_row >= data->n_rows)
	    break;

	  av = data->matrix[i_row];
	}
      else if (i_col < data->n_cols)
	{
	  av[i_col] = a;
	  i_col++;
	}
    }

  fts_atom_file_close(f);
  return i_row;
}

static int pbank_write_file(pbank_data_t *data, fts_symbol_t file_name)
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

  /* first, write the header: a line with "pbank <n_rows> <n_cols>" where n_rows and n_cols are ints */
  fts_set_symbol(&a, fts_new_symbol("pbank"));
  fts_atom_file_write(f, &a, ' ');

  fts_set_int(&a, data->n_cols);
  fts_atom_file_write(f, &a, ' ');

  fts_set_int(&a, data->n_rows);
  fts_atom_file_write(f, &a, ' ');

  fts_set_symbol(&a, fts_new_symbol(","));
  fts_atom_file_write(f, &a, '\n');

  /* write the content of the matrix */
  for(i=0; i<data->n_rows; i++)     
    {
      ap = data->matrix[i];

      for (j=0; j<data->n_cols; j++, ap++)	
	fts_atom_file_write(f, ap, ' ');

      fts_set_symbol(&a, fts_new_symbol(","));
      fts_atom_file_write(f, &a, '\n');

    }

  fts_atom_file_close(f);

  return(1);
}

static int pbank_export_file_ascii(pbank_data_t *data, fts_symbol_t file_name)
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
  for(i=0; i<data->n_rows; i++)     
    {
      ap = data->matrix[i];

      for(j=0; j<data->n_cols; j++, ap++)	
	fts_atom_file_write(f, ap, ' ');

      fts_set_symbol(&a, fts_new_symbol(""));
      fts_atom_file_write(f, &a, '\n');
    }

  fts_atom_file_close(f);

  return(1);
}

/* allocate/free pbank data */

static pbank_data_t *
pbank_data_get(fts_symbol_t name, int n_cols, int n_rows)
{
  fts_atom_t atom;

  if(n_cols < 1)
    n_cols = DEFAULT_N_COLS;

  if(n_rows < 1)
    n_rows = DEFAULT_N_ROWS;

  if (name && fts_hash_table_lookup(&pbank_data_table, name, &atom))
    {
      /* data found by name , check its dimension and reference it */
      pbank_data_t *data = (pbank_data_t *) fts_get_ptr(&atom);

      if(data->n_cols != n_cols || data->n_rows != n_rows)
	{
	  post("pbank: %s %d %d: dimensions don't match\n", fts_symbol_name(name), data->n_cols, data->n_rows);
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
      data->n_cols = n_cols;
      data->n_rows = n_rows;

      /* allocate the matrix */
      data->matrix = (fts_atom_t **) fts_malloc(sizeof(fts_atom_t *) * n_rows);
      for(i=0; i<n_rows; i++) 
	data->matrix[i] = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * n_cols);

      /* zero matrix */
      for(i=0; i<n_rows; i++)	
	{
	  fts_atom_t *row;

	  row = data->matrix[i];
	  for(j=0; j<n_cols; j++)
	    fts_set_int(row + j, 0);
	}

      /* allocate preset buffer */
      data->buffer = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * n_cols);

      /* zero buffer */
      for (j=0; j<n_cols; j++)
	fts_set_int(data->buffer + j, 0);

       if(name)
	{
	  data->name = name;

	  /* record in name table */
	  fts_set_ptr(&atom, data);
	  fts_hash_table_insert(&pbank_data_table, name, &atom);

	  /* read in data from file (at least try it) */
	  pbank_read_file(data, name);
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

      for (i=0; i<data->n_rows; i++) 
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
  int n_cols = fts_get_int_arg(ac, at, 1, 0);
  int n_rows = fts_get_int_arg(ac, at, 2, 0);
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 3, 0);
  fts_symbol_t receive = fts_get_symbol_arg(ac, at, 4, 0);
  int i;
  
  if (n_cols <= 0)
    {
      n_cols = DEFAULT_N_COLS;
      post("pbank: n_cols argument out of range, setting to %d\n", n_cols);
    }
       
  if (n_rows <= 0)
    {
      n_rows = DEFAULT_N_ROWS;
      post("pbank: n_rows argument out of range, setting to %d\n",n_rows);
    }

  /* name skip arg is "" */
  if(name == fts_new_symbol("\"\""))
    name = 0;
	
  if(receive)	
    {
      /* create array of receive names */
      this->receives = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t ) * n_cols);
    
      for(i=0; i<n_cols; i++) 
	{
	  char buf[256];

	  sprintf(buf, "%d-%s", i, fts_symbol_name(receive));
	  this->receives[i] = fts_new_symbol_copy(buf);
	}
    }

  /* tempory buffer for list output */
  this->out_list = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * n_cols);

  /* the matrix object */
  this->data = pbank_data_get(name, n_cols, n_rows);
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
  int i_col = fts_get_int_arg(ac, at, 0, 0);
  int i_row = fts_get_int_arg(ac, at, 1, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *p;
  int i;

  /* skip index of row and col */
  ac -= 2;
  at += 2;

  /* force column to matrix limits */
  if(i_col < 0)
    i_col = 0;
  else if(i_col >= n_cols)
    i_col = n_cols-1;

  if(i_col + ac > n_cols)
    ac = n_cols - i_col;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if (i_row >= n_rows)
    i_row = n_rows - 1;

  /* copy to matrix */
  p = this->data->matrix[i_row] + i_col;
  for(i=0; i<ac; i++)
    p[i] = at[i];
}

static void
pbank_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_col = fts_get_int_arg(ac, at, 0, 0);
  int n_cols = this->data->n_cols;
  int i;

  /* skip first argument (index of col) */
  ac--; at++;

  /* force column to matrix limits */
  if (i_col < 0)
    i_col = 0;
  else if (i_col >= n_cols)
    i_col = n_cols - 1;

  if(i_col + ac > n_cols)
    ac = n_cols - i_col;

  /* copy list to buffer */
  for(i=0; i<ac; i++)
    this->data->buffer[i] = at[i];
}

static void
pbank_get_row(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_row = fts_get_int(at);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *row;
  int i;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if(i_row >= n_rows)
    i_row = n_rows - 1;

  row = this->data->matrix[i_row];
      
  /* copy row to buffer and output */
  for(i=0; i<n_cols; i++)
    {
      this->data->buffer[i] = row[i];
      this->out_list[i] = row[i];
    }
  
  fts_outlet_list(o, 0, n_cols, this->out_list);
}

static void
pbank_get_row_to_receives(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_row = fts_get_int(at);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *row;
  int i;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if(i_row >= n_rows)
    i_row = n_rows - 1;

  row = this->data->matrix[i_row];
      
  /* copy matrix row to buffer and send to receives */
  for(i=n_cols-1; i>=0; i--)
    {
      fts_atom_t *atom = row + i;

      this->data->buffer[i] = *atom;
      fts_send_message_to_receives(this->receives[i], fts_type_get_selector(fts_get_type(atom)), 1, atom);
    }
}

/* outputs row as single elements lead by their col index and copies row to buffer */
static void
pbank_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_row = fts_get_int_arg(ac, at, 0, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *row;
  int i;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if(i_row >= n_rows)
    i_row = n_rows - 1;

  row = this->data->matrix[i_row];

  for(i=0; i<n_cols; i++)
    {
      fts_atom_t out_list[3];

      /* copy matrix element to buffer */
      this->data->buffer[i] = row[i];

      /* output matrix element lead by column */
      fts_set_int(out_list + 0, i);
      
      if(fts_is_symbol(row + i))
	{
	  /* buaaaarrrrrrrgg zaaaaaaaaaaaaack!!!!!!!*&(*@#&^(*^&*(%#$ */
	  fts_set_symbol((out_list + 1), fts_s_symbol);
	  out_list[2] = row[i];
	  fts_outlet_list(o, 0, 3, out_list);
	}
      else 
	{
	  out_list[1] = row[i];
	  fts_outlet_list(o, 0, 2, out_list);
	}
    }
}

/* outputs row as single elements lead by their col index and copies row to buffer */
static void
pbank_recall_to_receives(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_row = fts_get_int_arg(ac, at, 0, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *row;
  int i;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if(i_row >= n_rows)
    i_row = n_rows - 1;

  row = this->data->matrix[i_row];

  /* copy matrix row to buffer and send to receives */
  for(i=n_cols-1; i>=0; i--)
    {
      fts_atom_t *atom = row + i;

      this->data->buffer[i] = *atom;
      fts_send_message_to_receives(this->receives[i], fts_type_get_selector(fts_get_type(atom)), 1, atom);
    }
}

/* copies buffer to matrix row */
static void
pbank_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int i_row = fts_get_int_arg(ac, at, 0, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;
  fts_atom_t *row;
  int i;

  /* force row to matrix limits */
  if (i_row < 0)
    i_row = 0;
  else if (i_row >= n_rows)
    i_row = n_rows - 1;

  row = this->data->matrix[i_row];

  /* copy buffer to matrix row */
  for(i=0; i<n_cols; i++)
    row[i] = this->data->buffer[i];
}

/* store or recall single element to/from matrix */
static void
pbank_set_and_get(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  fts_atom_t *atom;
  int i_col = fts_get_int_arg(ac, at, 0, 0);
  int i_row = fts_get_int_arg(ac, at, 1, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;

  /* force column to matrix limits */
  if(i_col < 0)
    i_col = 0;
  else if (i_col >= n_cols)
    i_col = n_cols - 1;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if (i_row >= n_rows)
    i_row = n_rows - 1;

  atom = this->data->matrix[i_row] + i_col;

  if(ac == 2) 
    {
      fts_atom_t out_list[3]; 

      /* output matrix element lead by column */
      fts_set_int(out_list, i_col);
      
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
  int i_col = fts_get_int_arg(ac, at, 0, 0);
  int i_row = fts_get_int_arg(ac, at, 1, 0);
  int n_cols = this->data->n_cols;
  int n_rows = this->data->n_rows;

  /* force column to matrix limits */
  if(i_col < 0)
    i_col = 0;
  else if (i_col >= n_cols)
    i_col = n_cols - 1;

  /* force row to matrix limits */
  if(i_row < 0)
    i_row = 0;
  else if (i_row >= n_rows)
    i_row = n_rows - 1;

  atom = this->data->matrix[i_row] + i_col;

  if(ac == 2) 
    /* read atom from matrix and send to receives */
    fts_send_message_to_receives(this->receives[i_col], fts_type_get_selector(fts_get_type(atom)), 1, atom);
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
  int read_n_rows = 0;

  if(file_name)
    read_n_rows = pbank_read_file(this->data, file_name);
}

static void
pbank_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  pbank_write_file(this->data, fts_get_symbol(at));
}

static void
pbank_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  
  if(ac > 1 && fts_is_symbol(at) && fts_is_symbol(at + 1))
    {
      fts_symbol_t format = fts_get_symbol(at);

      if(format = fts_new_symbol("ascii"))
	{
	  pbank_export_file_ascii(this->data, fts_get_symbol(at + 1));
	}
      else
	post("pbank: unknown export format: %s\n", fts_symbol_name(format));
    }
  else
    post("pbank: usage: export <format> <file name>\n");
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

  return fts_Success;
}

void
pbank_config(void)
{
  fts_hash_table_init(&pbank_data_table);

  fts_metaclass_install(fts_new_symbol("pbank"), pbank_instantiate, fts_narg_equiv);
}
