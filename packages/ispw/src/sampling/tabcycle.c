>#include "fts.h"

#include "sampbuf.h"

static fts_symbol_t tabcycle_fun_symbol = 0;


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct
{
  long size;			/* cycle size */
  long offset;			/* sample offset (grows by the vectorSize) */
  sampbuf_t *buf;		/* samp tab buffer */
} tabcycle_ctl_t;

typedef struct
{
  fts_object_t obj; 
  ftl_data_t tabcycle_data;
  fts_symbol_t tab_name;		/* symbol bound to table we'll use */
  float value;			/* value to write ot samptab */
  int state;			/* inlet state - table style */
  long size;			/* cache info: cycle size */
} tabcycle_t;


static void
tabcycle_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 1, 0);
  long size = fts_get_long(at + 2);

  this->tab_name = tab_name;
  this->value = 0;
  this->state = 0;
  this->size = size;

  this->tabcycle_data = ftl_data_new(tabcycle_ctl_t);

  dsp_list_insert(o); /* just put object in list */
}


static void
tabcycle_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;

  ftl_data_free(this->tabcycle_data);
  dsp_list_remove(o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static void
tabcycle_dsp_function(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_ptr(argv);
  tabcycle_ctl_t *ctl = (tabcycle_ctl_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long offset;

  offset = ctl->offset;
  fts_vecx_fcpy(ctl->buf->samples + offset, out, n_tick);
  ctl->offset = (ctl->offset + n_tick) % ctl->size;
}

static void
tabcycle_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  sampbuf_t *buf;
  long n_tick;
  long l;

  buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      n_tick = fts_dsp_get_input_size(dsp, 0);

      if(this->size % n_tick || this->size <= 0)
	{
	  post("tabcycle~: %s: size must be a multiple of %d\n", fts_symbol_name(this->tab_name), n_tick);
	  return;
	}

      if (buf->size < this->size)
	{
	  post("tabcycle~: %s: table to short\n", fts_symbol_name(this->tab_name));
	  return;
	}
      
      l = this->size;
      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, size, &l);

      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, buf, &buf);

      l = 0;
      ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);

      fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv + 1, this->tabcycle_data);
      fts_set_long(argv + 2, n_tick);
      dsp_add_funcall(tabcycle_fun_symbol, 3, argv);
    }
  else
    post("tabcycle~: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}

/******************************************************************
 *
 *    user methods
 *
 */

static void
tabcycle_bang(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;
  const long  l = 0;

  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);
}

static void
tabcycle_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  tabcycle_t *this = (tabcycle_t *)o;

  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      if (buf->size >= this->size)
	{
	  const long  l = 0;
	  
	  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, buf, &buf);
	  ftl_data_set(tabcycle_ctl_t, this->tabcycle_data, offset, &l);
	  this->tab_name = tab_name;
	}
    }
  else
    post("tabcycle~: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(tabcycle_t), 1, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, tabcycle_init, 3, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, tabcycle_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, tabcycle_put);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang, tabcycle_bang, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("set"), tabcycle_set, 1, a);

  /* Type the outlet */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  tabcycle_fun_symbol = fts_new_symbol("tabcycle");
  dsp_declare_function(tabcycle_fun_symbol, tabcycle_dsp_function);

  return fts_Success;
}

void
tabcycle_config(void)
{
  fts_metaclass_create(fts_new_symbol("tabcycle~"),class_instantiate, fts_always_equiv);
}


