/* pbank - zack settel 1994  */
/* Heavily modified (50% reimplemented) and ported to the new FTS by MDC */

#include "fts.h"




typedef struct
{
  fts_atom_t **matrix;		/* matrix  pointer */
  fts_atom_t *edit_buffer;	/* one rows of edit buffer */
  long rows;			/* memory dimension */
  long columns;			/* memory dimension */
  int  refcount;		/* number of objects pointing to "matrix" */
  int  dirty;			/* dirty flag: modified w.r.t. the last file save*/
} pbank_data_t;

#define DEFAULT_COLUMNS 128
#define DEFAULT_ROWS 10


typedef struct
{
  fts_object_t ob;	 
  pbank_data_t *data;		/* the pbank_data_t object including the param matrix :
				   matrix[PATCH_COUNT] X *matrix+PARAMCOUNT */
  fts_atom_t *out_list;         /* list for output on int method */
  fts_symbol_t *receives;	/* used for back door messaging */
  fts_symbol_t name;		/* pbank data/file name <optional> */ 
} pbank_t;


/****************************************************************************/
/*                                                                          */
/*                      PBANK DATA HANDLING                                 */
/*                                                                          */
/****************************************************************************/

/* Two functions:

   1- pbank_data_new, return the pdata with the right name and size,
      if it exists, otherwise create a new one, and return the pdata.
      refcount the pbank_data object.

   2- pbank_data_release, dereference the refcount, and if zero free the
      structure

   pbank_data can be unnamed, in such case they are not put in the table.
*/


static fts_hash_table_t pbank_data_table;

int
pbank_read_file(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_file_t *f;
  int ret;
  fts_atom_t a, *av;
  int current_column, current_row;


  f = fts_atom_file_open(fts_symbol_name(file_name), "r");

  if(!f){
    post("pbank: can't open file to read: %s\n", fts_symbol_name(file_name));
    return(0);
  }

  /* read the pbank keyword */

  ret = fts_atom_file_read(f, &a);

  if ((! ret) || (! fts_is_symbol(&a)) || (fts_get_symbol(&a) != fts_new_symbol("pbank")))
    {
      post("pbank: file hasn't pbank format: %s\n", fts_symbol_name(file_name));
      return(0);
    }

  /* skip the column, row and comma in the file (we rebuild the matrix size
     from the data itself, and we trust the method) */

  fts_atom_file_read(f, &a);
  fts_atom_file_read(f, &a);
  fts_atom_file_read(f, &a);

  /* note: the above line is intentionally repeated three times */

  current_row = 0;
  current_column = 0;

  av = data->matrix[current_row];

  while (fts_atom_file_read(f, &a))
    {
      if (fts_is_symbol(&a) && (fts_get_symbol(&a) == fts_new_symbol(",")))
	{
	  current_row++;
	  current_column = 0;

	  /* in case of row overflow, break and skip the other data */

	  if (current_row >= data->rows)
	    break;

	  av = data->matrix[current_row];
	}
      else if (current_column < data->columns)
	{
	  av[current_column] = a;
	  current_column++;
	}
    }

  fts_atom_file_close(f);
  return(1);
}

int
pbank_write_file(pbank_data_t *data, fts_symbol_t file_name)
{
  fts_atom_t  a, *av;
  fts_atom_file_t *f;
  int i, j;


  f = fts_atom_file_open(fts_symbol_name(file_name), "w");
  if(!f){
    post("pbank: can't open file to write: %s\n", fts_symbol_name(file_name));
    return(0);
  }

  /* first, write the header: a line with "pbank rows  columns" where
     rows and columns are longs
     */

  fts_set_symbol(&a, fts_new_symbol("pbank"));
  fts_atom_file_write(f, &a, ' ');

  fts_set_long(&a, data->columns);
  fts_atom_file_write(f, &a, ' ');

  fts_set_long(&a, data->rows);
  fts_atom_file_write(f, &a, ' ');

  fts_set_symbol(&a, fts_new_symbol(","));
  fts_atom_file_write(f, &a, '\n');

  /* write the content of the matrix */

  for (i = 0; i < data->rows; i++)     
    {
      av = data->matrix[i];

      for (j = 0; j < data->columns; j++, av++)	
	fts_atom_file_write(f, av, ' ');

      fts_set_symbol(&a, fts_new_symbol(","));
      fts_atom_file_write(f, &a, '\n');

    }

  fts_atom_file_close(f);

  data->dirty = 0;
  return(1);
}

static pbank_data_t *
pbank_data_new(fts_symbol_t name, long columns, long rows)
{
  fts_atom_t data;

  if(columns < 1)
    columns = DEFAULT_COLUMNS;

  if(rows < 1)
    rows = DEFAULT_ROWS;

  if (name && fts_hash_table_lookup(&pbank_data_table, name, &data))
    {
      /* pbank_data  found, check its dimension and reference it */

      pbank_data_t *p = (pbank_data_t *) fts_get_ptr(&data);

      if (p->columns != columns || p->rows != rows)
	{
	  post("pbank: pbank %s's dimensions must be (%d x %d)\n", fts_symbol_name(name),
		p->columns, p->rows);
	  return 0;
	}
      else
	{
	  p->refcount++;
	  return p;
	}
    }
  else
    {
      int i, j;
      pbank_data_t *p;

      /* pbank_data_t not found, allocate and zero a new one */

      p = (pbank_data_t *) fts_malloc(sizeof(pbank_data_t));
      p->refcount = 1;
      p->columns  = columns;
      p->rows     = rows;

      /* allocate the real matrix */

      p->matrix = (fts_atom_t **) fts_malloc(sizeof(fts_atom_t *) * rows);

      for (i = 0; i < rows; i++) 
	p->matrix[i] = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * columns);

      p->edit_buffer = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * columns);

      /* Set the whole data matrix to 0's longs */

      for (i = 0; i < rows; i++)	
	{
	  fts_atom_t *rowp;

	  rowp = p->matrix[i];

	  for (j = 0; j < columns; j++)
	    fts_set_long(&(rowp[j]),0L);
	}

      /* set the whole edit buffer to 0's long */

      for (j = 0; j < columns; j++)
	fts_set_long(&(p->edit_buffer[j]),0L);

      /* if named, record it in the table */

      if (name)
	{
	  fts_set_ptr(&data, p);
	  fts_hash_table_insert(&pbank_data_table, name, &data);
	}

      /* finally, return it */

      return p;
    }
}



static void
pbank_data_release(pbank_data_t *p, fts_symbol_t name)
{
  /* dereference it */

  p->refcount--;
  
  if (! p->refcount)
    {
      int i;
      
      /* no more referenced, take it away from the table if named
	 and free the memory*/
	  
      if (name)
	fts_hash_table_remove(&pbank_data_table, name);

      for (i = 0; i < p->rows; i++) 
	fts_free((void *)p->matrix[i]);

      fts_free((void *)p->edit_buffer);

      fts_free((void *)p->matrix);
    }
}


/* Operation to set data in a given row, starting from the given column */

static void 
pbank_data_fill_matrix(pbank_data_t *this, long column, long row, int ac, const fts_atom_t *at)
{ 
  fts_atom_t *p;
  int i;

  /* force the row and column arguments to be within the matrix limits */

  if (column < 0)
    column = 0;
  else if (column >= this->columns)
    column = this->columns-1;

  if (row < 0)
    row = 0;
  else if (row >= this->rows)
    row = this->rows-1;

  /* do the copy */

  p = this->matrix[row];

  for (i = 0; i < this->columns - column; i ++ )
    p[i + column] = at[i];

  this->dirty = 1; /* set dirty flag */
}


/*fill the edit buffer, starting from a given colum */


static void 
pbank_data_fill_edit_buffer(pbank_data_t *this, long column, int ac, const fts_atom_t *at)
{ 
  fts_atom_t *p;
  int i;

  /* force column argument to be within the matrix limits */

  if (column < 0)
    column = 0;
  else if (column >= this->columns)
    column = this->columns-1;

  /* do the copy */

  p = this->edit_buffer;

  for (i = 0; i < ac; i ++ )
    p[i + column] = at[i];
}


/* Copy a row to the edit buffer */

static void 
pbank_data_row_to_edit_buffer(pbank_data_t *this, long row)
{ 
  fts_atom_t *p;
  int i;

  /* force the row  argument to be within the matrix limits */

  if (row < 0)
    row = 0;
  else if (row >= this->rows)
    row = this->rows - 1;

  /* do the copy */

  p = this->matrix[row];

  memcpy(this->edit_buffer, p, sizeof(fts_atom_t) * this->columns);
}


/* Copy the edit buffer to a row */

static void 
pbank_data_edit_buffer_to_row(pbank_data_t *this, long row)
{ 
  fts_atom_t *p;
  int i;

  /* force the row  argument to be within the matrix limits */

  if (row < 0)
    row = 0;
  else if (row >= this->rows)
    row = this->rows - 1;

  /* do the copy */

  p = this->matrix[row];

  memcpy(p, this->edit_buffer, sizeof(fts_atom_t) * this->columns);

  this->dirty = 1; /* set dirty flag */
}


/****************************************************************************/
/*                                                                          */
/*                      User methods                                        */
/*                                                                          */
/****************************************************************************/



/* method for message set, inlet 0 */
/* write item to matrix at given column, row address */
/* message:  column row thing1......thingN  */

static void
pbank_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  long column,row;

  if (ac < 3)
    {
      post("ac=%d\n",ac);
      post("pbank: set: takes at least 3 arguments: column row item(s)\n");
      return;
    }

  if ((! fts_is_long(at)) || (! fts_is_long(at+1)))
    {
      post("pbank: set: first arguments must of type integer (columnNo. or rowNo.)\n");
      return;
    }

  column = fts_get_long(at);
  row    = fts_get_long(at+1);

  pbank_data_fill_matrix(this->data, column, row, ac - 2, at + 2);
}


/* method for message put, inlet 0 */
/* same as set but writes to the edirt buffer:  'put' column thing1......thingN  */

static void
pbank_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  long column;

  if (ac < 2)
    {
      post("ac=%d\n",ac);
      post("pbank: put: takes at least 2 arguments: column item(s)\n");
      return;
    }

  if (! fts_is_long(at))    
    {
      post("pbank: put: first argument must of type integer (columnNo.)\n");
      return;
    }

  column = fts_get_long(at);

  pbank_data_fill_edit_buffer(this->data, column, ac - 1, at + 1);
}



/* method for message recall, inlet 0 */
/* dumps (outputs) and copies row to edbuffer */

static void
pbank_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  long row = fts_get_long(at);
  int i;
  fts_atom_t *p;

  if (row  < 0)
    row = 0L;
  else if (row >= this->data->rows)
    row = this->data->rows - 1;

  pbank_data_row_to_edit_buffer(this->data, row);

  p = this->data->matrix[row];

  if (this->receives)
    {
      /* back-door sending */



      for (i = this->data->columns - 1; i >= 0; i--)
	{
	  fts_symbol_t type;

	  if (fts_is_float(&p[i]))
	    type = fts_s_float;
	  else if (fts_is_long(&p[i]))
	    type = fts_s_int;
	  else  if (fts_is_symbol(&p[i])) 
	    type = fts_s_symbol;
	  else
	    type = fts_s_symbol; /* Default when in troubles */

	  fts_send_message_to_receives(this->receives[i], type, 1, &p[i]);
	}
    }
  else /* otherwise use outlet */
    for (i = 0; i < this->data->columns; i++)
      {
	fts_atom_t outlist[3]; 

	fts_set_long(outlist, (long)i);

	if (fts_is_symbol(&p[i])) 
	  {
	    fts_set_symbol((outlist+1), fts_s_symbol);
	    outlist[2] = p[i];

	    fts_outlet_list(o, 0, 3, outlist);
	  }
	else 
	  {
	    outlist[1] = p[i];

	    fts_outlet_list(o, 0, 2, outlist);
	  }
      }
}

/* get a row as list */
static void
pbank_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  long n_columns = this->data->columns;
  long row = fts_get_long(at);

  if(row >= 0 && row < this->data->rows)
    {
      memcpy(this->out_list, this->data->matrix[row], sizeof(fts_atom_t) * n_columns);
      fts_outlet_list(o, 0, n_columns, this->out_list);
    }
}

/* get a sub list of a row as list */
static void
pbank_get(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  long n_columns = this->data->columns;
  long row = fts_get_long_arg(ac, at, 0, 0);
  long column = fts_get_long_arg(ac, at, 1, 0);
  long length = fts_get_long_arg(ac, at, 2, n_columns);
  
  if(row >= 0 && row < this->data->rows && column >= 0 && column < this->data->columns)
    {
      if(column + length > n_columns)
	length = n_columns - column;
      
      memcpy(this->out_list, &(this->data->matrix[row][column]), sizeof(fts_atom_t) * length);
      fts_outlet_list(o, 0, length, this->out_list);
    }
}

/* method for message store, inlet 0 */
/* copies edbuffer to row */

static void
pbank_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  pbank_data_edit_buffer_to_row(this->data, fts_get_long(at));
}


/* method for message list, inlet 0 */
/* eg 'list COLUMN ROW <optional> VALUE'  - ac=2 is read, arg=3 is write */

static void
pbank_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t    *this = (pbank_t *)o;
  fts_atom_t *av;
  long        column, row;

  column = fts_get_long(at);

  if (column < 0)
    column = 0;
  else if (column >= this->data->columns)
    column = this->data->columns - 1;

  row = fts_get_long(at+1);

  if (row < 0)
    row = 0;
  else if (row >= this->data->rows)
    row = this->data->rows - 1;

  av = this->data->matrix[row] + column;

  if (ac == 2) /* read */
    {
      if (this->receives)
	{
	  fts_symbol_t type;

	  if (fts_is_float(av))
	    type = fts_s_float;
	  else if (fts_is_long(av))
	    type = fts_s_int;
	  else if (fts_is_symbol(av)) 
	    type = fts_s_symbol;
	  else
	    type = fts_s_symbol; /* Default for errors */

	  fts_send_message_to_receives(this->receives[column], type, 1, av);
	}
      else
	{
	  fts_atom_t outlist[3]; 

	  fts_set_long(outlist, column);

	  if (fts_is_symbol(av)) 
	    {
	      fts_set_symbol(outlist+1, fts_s_symbol);
	      outlist[2] = *av;
	      fts_outlet_list(o, 0, 3, outlist);
	    }
	  else 
	    {
	      outlist[1] = *av;
	      fts_outlet_list(o, 0, 2, outlist);
	    }
	}
    }
  else /* write */
    {
      *av = at[2];
      this->data->dirty = 1; 
    }
}


/* method for message read, inlet 0 */

static void
pbank_read(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  pbank_read_file(this->data, fts_get_symbol(at));
}


/* method for message write, inlet 0 */

static void
pbank_write(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  pbank_write_file(this->data, fts_get_symbol(at));
}


/****************************************************************************/
/*                                                                          */
/*                      System  methods                                     */
/*                                                                          */
/****************************************************************************/


/* columns rows  memory-name receiveName<optional> */
/* note: memory-name is optional when no receiveName argument is specified */  
/* New method */

static void
pbank_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;
  int rows, columns;
  int i;
  
  ac--; at++;			/* throw away class name argument */

  this->name = 0;
  this->receives = 0;

  columns = fts_get_long_arg(ac, at, 0, 0);
  rows = fts_get_long_arg(ac, at, 1, 0);

  if (columns <= 0)
    {
      columns = DEFAULT_COLUMNS;
      post("warning: pbank: columns argument out of range, setting to %d\n", columns);
    }
       
  if (rows <= 0)
    {
      rows = DEFAULT_ROWS;
      post("warning: pbank: rows argument out of range, setting to %d\n",rows);
    }

  /* get the name */

  if (ac >= 3 && (fts_get_symbol(at + 2) != fts_new_symbol("\"\"")))
    this->name = fts_get_symbol(at + 2);
	
  /* get the receives, if present */

  if (ac == 4)	
    {
      /* ac == 4: establish output pointers to receive objects  */

      this->receives = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t ) * columns);
    
      for (i = 0; i < columns; i++) 
	{
	  char buf[256];

	  sprintf(buf, "%d-%s", i, fts_symbol_name(fts_get_symbol(at+3)));
	  this->receives[i] = fts_new_symbol_copy(buf);
	}
    }

  this->out_list = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * columns);
  
  this->data = pbank_data_new(this->name, columns, rows);
  if(this->name)
    pbank_read_file(this->data, this->name); /* read in data from file (at least try it) */

  this->data->dirty = 0;
}

/* method delete, system inlet */

static void
pbank_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pbank_t *this = (pbank_t *)o;

  if (this->data->dirty) 
    {
      if(this->name)
	post("pbank: %s: unsaved data\n", fts_symbol_name(this->name));
      else
	post("pbank: unsaved data\n");	
    }

  pbank_data_release(this->data, this->name);

  if (this->receives)
    fts_free((void *)this->receives);

  fts_free((void *)this->out_list);
}

/****************************************************************************/
/*                                                                          */
/*                      instantiate & config                                */
/*                                                                          */
/****************************************************************************/

static fts_status_t
pbank_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  /* initialize the class */

  fts_class_init(cl, sizeof(pbank_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_symbol;
  a[4] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, pbank_init, 5, a, 3);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, pbank_delete, 0, 0);

  /* Pbank args */

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_anything;
  fts_method_define_optargs(cl, 0, fts_s_list, pbank_list, 3, a, 2);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), pbank_set);

  fts_method_define_varargs(cl, 0, fts_new_symbol("put"), pbank_put);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, pbank_int, 1, a);

  fts_method_define_varargs(cl, 0, fts_new_symbol("get"), pbank_get);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("recall"), pbank_recall, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("store"), pbank_store, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("write"), pbank_write, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("read"), pbank_read, 1, a);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);


  return fts_Success;
}

void
pbank_config(void)
{
  fts_hash_table_init(&pbank_data_table);

  fts_metaclass_create(fts_new_symbol("pbank"),pbank_instantiate, fts_always_equiv);
}
