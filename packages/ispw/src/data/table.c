#include <stdlib.h>

#include "fts.h"
#include "table.h"

/* Naming and reference counting of fts_intvec_t is done here, locally
   and privately for tables; tables store intvec in the table only
   if named, otherwise allocate directly the vecint.
   */
  

static fts_hash_table_t table_intvec_table; /* the name binding table */
static fts_heap_t *table_intvec_heap;

typedef struct table_intvec
{
  fts_symbol_t name;
  int refcount;
  fts_intvec_t *v;
} table_intvec_t;


static fts_intvec_t *
table_intvec_get(fts_symbol_t name, int size)
{
  table_intvec_t *this;
  fts_atom_t d;

  if (fts_hash_table_lookup(&table_intvec_table, name, &d))
    {
      this = (table_intvec_t *) fts_get_ptr(&d);
      this->refcount++;
    }
  else
    {
      this = (table_intvec_t *) fts_heap_alloc(table_intvec_heap);
      this->name = name;
      this->v = fts_intvec_new(size);
      this->refcount = 1;

      fts_set_ptr(&d, this);
      fts_hash_table_insert(&table_intvec_table, name, &d);
    }

  return this->v;
}


static void
table_intvec_release(fts_symbol_t name)
{
  fts_atom_t d;
  table_intvec_t *this;

  if (fts_hash_table_lookup(&table_intvec_table, name, &d))
    {
      this = (table_intvec_t *) fts_get_ptr(&d);
      this->refcount--;
    }
  else
    return;

  if (this->refcount == 0)
    {
      fts_intvec_delete(this->v);
      fts_hash_table_remove(&table_intvec_table, this->name);
      fts_heap_free((char *) this, table_intvec_heap);
    }
}

/* Find a  fts_intvec by name */


fts_intvec_t *
table_intvec_get_by_name(fts_symbol_t name)
{
  fts_atom_t d;
  table_intvec_t *this;

  if (fts_hash_table_lookup(&table_intvec_table, name, &d))
    {
      this = (table_intvec_t *) fts_get_ptr(&d);
      return this->v;
    }
  else
    return 0;
}


typedef struct 
{
  fts_object_t ob;
  int new_value;
  int new_value_valid;
  fts_symbol_t name;
  fts_intvec_t *table_vec;
} table_t;


static void
table_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  fts_outlet_int(o, 0, fts_intvec_get_quantile(this->table_vec, rand() & 0x7fff));
}

static void
table_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int n = fts_get_number_arg(ac, at, 0, 0);

  if (n < 0)
    n = 0;
  else if (n >= fts_intvec_get_size(this->table_vec))
    n = fts_intvec_get_size(this->table_vec) - 1;

  if (this->new_value_valid)
    {
      fts_intvec_set_value(this->table_vec, n, this->new_value);
      this->new_value_valid = 0;
    }
  else
    fts_outlet_int(o, 0, fts_intvec_get_value(this->table_vec, n));
}


static void
table_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int n = (int) fts_get_number_arg(ac, at, 0, 0);

  this->new_value_valid = 1;
  this->new_value = n;
}

static void
table_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    table_number_1(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    table_number(o, winlet, s, 1, at + 0);
}

static void
table_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int onset;

  if (! fts_is_int(at)) 
    {
      post("table: set: bad onset\n");
      return;
    }

  onset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    fts_intvec_set(this->table_vec, onset, ac - 1, at + 1);
}


static void
table_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  fts_outlet_int(o, 0, fts_intvec_get_inv(this->table_vec, fts_get_int(&at[0])));
}

static void
table_quantile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  fts_outlet_int(o, 0, fts_intvec_get_quantile(this->table_vec, fts_get_int(&at[0])));
}

static void
table_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  fts_intvec_set_const(this->table_vec, 0);
}

static void
table_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int n = fts_get_int(&at[0]);

  fts_intvec_set_const(this->table_vec, n);
}

static void
table_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int new_size = fts_get_int(&at[0]);

  fts_intvec_set_size(this->table_vec, new_size);
}

static void
table_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  fts_outlet_int(o, 0, fts_intvec_get_sum(this->table_vec, 0, -1));
}



/* Daemon for the getting the size property */

static void
table_get_size(fts_daemon_action_t action, fts_object_t *obj,
		  int idx, fts_symbol_t property, fts_atom_t *value)
{
  table_t *this = (table_t *)obj;

  fts_set_int(value, fts_intvec_get_size(this->table_vec));
}


static void
table_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_symbol_t name;
  int size;
  
  if(ac > 1 && fts_is_symbol(at + 1))
    {
      name = fts_get_symbol(at + 1);
      size = fts_get_number_arg(ac, at, 2, FTS_TABLE_DEFAULT_SIZE);
    }
  else
    {
      name = 0;
      size = fts_get_number_arg(ac, at, 1, FTS_TABLE_DEFAULT_SIZE);
    }

  this->name = name;

  if (this->name)
    this->table_vec = table_intvec_get(name, size);
  else
    this->table_vec = fts_intvec_new(size);
  
  if(name)
    fts_register_named_object(o, name);
}


static void
table_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  if (this->name)
    table_intvec_release(this->name);
  else
    fts_intvec_delete(this->table_vec);

  if (this->name)
    fts_unregister_named_object(o, this->name);
}


static void
table_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);

  fts_intvec_save_bmax(this->table_vec, f);
}

/* Daemon for getting the property "data".
   Note that we return a pointer to the data; 
   if the request come from the client, it will be the
   kernel to handle the export of the data, not the table
   object.
 */


static void
table_get_data(fts_daemon_action_t action, fts_object_t *obj,
		 int idx, fts_symbol_t property, fts_atom_t *value)
{
  table_t *this = (table_t *)obj;

  fts_set_data(value, (fts_data_t *) this->table_vec);
}

/* Class instantiate */


static fts_status_t
table_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(table_t), 2, 1, 0);

  a[0] = fts_s_symbol; /* class */
  a[1] = fts_s_anything; /* opt: name or size */
  a[2] = fts_s_int; /* opt: size */
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, table_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, table_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, table_save_bmax, 1, a);

  /* Inlet 0 */

  fts_method_define(cl, 0, fts_s_bang, table_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, table_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, table_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, table_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("inv"), table_inv, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("quantile"), table_quantile, 1, a);

  fts_method_define(cl, 0, fts_new_symbol("sum"), table_sum, 0, 0);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), table_set);

  fts_method_define(cl, 0, fts_s_clear, table_clear, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("const"), table_const, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("size"), table_size, 1, a);

  /* Inlet 1 */

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, table_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, table_number_1, 1, a);

  /* outlet typing */

  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  /* daemon for name property */

  fts_class_add_daemon(cl, obj_property_get, fts_s_size, table_get_size); /* ???? @@@@ */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, table_get_data);

  return fts_Success;
}


void
table_config(void)
{
  table_intvec_heap = fts_heap_new(sizeof(table_intvec_t));
  fts_hash_table_init(&table_intvec_table);
  fts_metaclass_create(fts_new_symbol("table"),table_instantiate, fts_always_equiv);
}


